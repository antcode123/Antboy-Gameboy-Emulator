#include <memory>
#include <cstring>
#include <fstream>
#include <iostream>
#include "mmu.hpp"
#include "../Utilities/misc.hpp"


namespace Hardware {

    // The interface for all components to access different regions of memory.
    // The MMU forwards reads and writes to addresses to the correct region of memory, whether that be a form of RAM, ROM or an I/O register.
    MMU::MMU(CPU& _cpu, PPU& _ppu, Cartridge& _cartridge, Joypad& _joypad,  Timer& _timer, std::string _exe_path) :
        cpu(_cpu),
        ppu(_ppu),
        cartridge(_cartridge),
        joypad(_joypad),
        timer(_timer),
        exe_path(_exe_path),
        work_ram(std::make_unique<U8[]>(8192)),
        high_ram(std::make_unique<U8[]>(127)) {
        load_bootstrap();
    }


    void MMU::reset() {
        std::memset(work_ram.get(), 0, 8192);
        std::memset(high_ram.get(), 0, 127);
    }


    void MMU::load_bootstrap() {
        std::ifstream file(exe_path + "\\Assets\\bootstrap.gb", std::ios::binary);
        file.read((char*)bootstrap, 256);
        file.close();
    }


    U8 MMU::read_u8(U16 address) {
        // Initialially I used a single array to store the entire Gameboy memory map,
        // however realised that it is much more optimal, readable and convenient to
        // map memory address to different regions/components of the Gameboy instead.

        if (is_bootstrap_enabled && address < 0x100) return bootstrap[address]; // Bootstrap interception
        if (address < 0x8000) return cartridge.read_rom(address); // Cartridge ROM access
        if (address < 0xA000) return ppu.video_ram[address - 0x8000]; // Video RAM access
        if (address < 0xC000) return cartridge.read_ram(address); // Cartridge RAM access
        if (address < 0xE000) return work_ram[address - 0xC000]; // Work RAM access
        if (address >= 0xFE00 && address < 0xFEA0) return ppu.oam[address - 0xFE00]; // OAM access
        if (address == 0xFF00) return joypad.read(); // Joypad register access
        if (address >= 0xFF04 && address < 0xFF08) return timer.read(address); // Timer access
        if (address == 0xFF0F) return cpu.interrupt_flag; // Interrupt flag register access
        if (address >= 0xFF10 && address < 0xFF30) return 0; // Even though the APU is unimplemented this fixes APU related bugs in some games such as Mole Mania
        if (address >= 0xFF40 && address < 0xFF50) return ppu.read(address); //PPU access
        if (address >= 0xFF80 && address < 0xFFFF) return high_ram[address - 0xFF80]; // High RAM access
        if (address == 0xFFFF) return cpu.interrupt_enabled; // Interrupt enabled register access
        else return 0xFF; // Default read for unmapped addresses
    }


    U16 MMU::read_u16(U16 address) {
        return ((U16)read_u8(address + 1) << 8) | read_u8(address); // Gameboy uses little endian (MSB is at the high address, LSB is at the low address)
    }


    void MMU::write_u8(U16 address, U8 u8) {
        if (address < 0x8000) cartridge.mbc->write(address, u8);
        else if (address < 0xA000) ppu.video_ram[address - 0x8000] = u8;
        else if (address < 0xC000) cartridge.write_ram(address, u8);
        else if (address < 0xE000) work_ram[address - 0xC000] = u8;
        else if (address >=  0xFE00 && address < 0xFEA0) ppu.oam[address - 0xFE00] = u8;
        else if (address == 0xFF00) joypad.joypad = u8 & 0xF0;
        else if (address >= 0xFF04 && address < 0xFF08) timer.write(address, u8);
        else if (address == 0xFF0F) cpu.interrupt_flag = u8;

        else if (address >= 0xFF40 && address < 0xFF50) {
            if (address == 0xFF46) perform_dma_transfer(u8); // DMA transfer
            else ppu.write(address, u8);
        }

        else if (address == 0xFF50) is_bootstrap_enabled = false; // Unmapping bootstrap
        else if (address >= 0xFF80 && address < 0xFFFF) high_ram[address - 0xFF80] = u8;
        else if (address == 0xFFFF) cpu.interrupt_enabled = u8;
    }


    void MMU::write_u16(U16 address, U16 u16) {
        write_u8(address + 1, (u16 & 0xFF00) >> 8);
        write_u8(address, u16 & 0xFF);
    }


    void MMU::perform_dma_transfer(U8 encoded_source_address) {

        // The source address encoded as the source address bit shifted right by 8 so it can fit into a single byte.
        // Performing a bit shift left by 8 will return the actual source address
        U16 source_address = (U16)encoded_source_address << 8;
        for (int i = 0; i < 160; i++ ) write_u8(0xFE00 + i, read_u8(source_address + i));
    }
}
