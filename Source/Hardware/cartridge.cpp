#include <cstring>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "cartridge.hpp"
#include "../Utilities/misc.hpp"


namespace Hardware {

    // Houses the game's ROM, RAM, MBC and sometimes the battery
    Cartridge::Cartridge() {reset();}


    Cartridge::~Cartridge() {save_ram();}


    void Cartridge::reset() {
        does_contain_battery = false;
        is_saved = false;
        rom = std::make_unique<U8[]> (2097152); // Initialised size of 2 MB so that the ROM can be loaded in regardless of size
        ram = std::make_unique<U8[]> (131072); // Same for RAM with 128 KB
    }


    U8 Cartridge::read_rom(U16 address) {
        U8 rom_bank = 0;

        // MBC bank zero access
        if (address < 0x4000) {
            mbc->modify_rom_bank(address, rom_bank);
            return rom[0x4000 * rom_bank + address];
        }

        // MBC swappable bank access. The operation (rom bank mod total banks) is performed because
        // bank numbers are wrapped if they exceed the total number of banks
        rom_bank = mbc->rom_bank % mbc->total_rom_banks;
        mbc->modify_rom_bank(address, rom_bank);
        return rom[0x4000 * rom_bank + address - 0x4000];
    }


    U8 Cartridge::read_ram(U16 address) {
        if (!mbc->is_ram_enabled) return 0xFF;

        if (mbc->type == 1) {
            U8 ram_bank = mbc->is_rom_bank_mode ? 0 : mbc->ram_bank;
            if (ram_size == 0) return 0xFF;
            if (ram_size <= 8192) return ram[(address - 0xA000) % ram_size]; // RAM address for RAM size less than or equal to 8KB are wrapped
            else return ram[0x2000 * ram_bank + address - 0xA000];
        }

        if (mbc->type == 2) return ram[address & 0x1FF] | 0xF0; // Address should only contain 9 bits and should return the value with upper 4 bits set to 1
        return 0xFF;
    }


    void Cartridge::write_ram(U16 address, U8 u8) {
        if (!mbc->is_ram_enabled) return;

        if (mbc->type == 1) {
            U8 ram_bank = mbc->is_rom_bank_mode ? 0 : mbc->ram_bank; // Can only read from RAM bank 0 in rom bank mode

            if (ram_size == 0) return;
            if (ram_size <= 8192) ram[(address - 0xA000) % ram_size] = u8;
            else ram[0x2000 * ram_bank + address - 0xA000] = u8;
        }

        else if (mbc->type == 2) ram[address & 0x1FF] = u8 | 0xF0;
    }


    void Cartridge::insert(std::string _file_path) {
        file_path = _file_path;
        std::ifstream file(file_path, std::ios::binary);
        file.seekg(0, std::ios::end);
        std::streampos size = file.tellg();
        file.seekg(0, std::ios::beg);
        file.read((char*)rom.get(), size);
        file.close();
        configure();
    }


    void Cartridge::configure() {

        // Cartridge type
        switch (rom[0x147]) {
            case 0: mbc = std::make_shared<MBC>(0); break;
            case 1: mbc = std::make_shared<MBC1>(); break;
            case 2: mbc = std::make_shared<MBC1>(); break;
            case 3: mbc = std::make_shared<MBC1>(); does_contain_battery = true; break;
            case 5: mbc = std::make_shared<MBC2>(); break;
            case 6: mbc = std::make_shared<MBC2>(); does_contain_battery = true; break;
        }

        switch (rom[0x148]) {
            case 0: rom_size = 32768; break;
            case 1: rom_size = 65536; break;
            case 2: rom_size = 131072; break;
            case 3: rom_size = 262144; break;
            case 4: rom_size = 524288; break;
            case 5: rom_size = 1048576; break;
            case 6: rom_size = 2097152; break;
            case 7: rom_size = 4194304; break;
            case 8: rom_size = 8388608; break;
        }

        mbc->total_rom_banks = 1 << (rom[0x148] + 1);

        switch (rom[0x149]) {
            case 1: ram_size = 2048; break;
            case 2: ram_size = 8192; break;
            case 3: ram_size = 32768; break;
            case 4: ram_size = 131072; break;
            case 5: ram_size = 65536; break;
            default: ram_size = 0; break;
        }

        load_ram();

        std::cout << "Cartridge name: " << Utilities::get_file_name_from_path(file_path) << std::endl;
        std::cout << "MBC: " << mbc->type << std::endl;
        std::cout << "ROM size: " << rom_size << " Bytes" << std::endl;
        std::cout << "RAM size: " << ram_size << " Bytes" << std::endl;
        std::cout << "Total ROM banks: " << mbc->total_rom_banks << std::endl << std::endl;
    }


    void Cartridge::save_ram() {
        if (!does_contain_battery || is_saved) return;
        std::string save_file_path = file_path.substr(0, file_path.length() - 3) + ".sav";
        std::ofstream file(save_file_path, std::ios::binary);
        file.write((char*)ram.get(), ram_size);
        file.close();
        is_saved = true;
    }


    void Cartridge::load_ram() {
        if (!does_contain_battery) return;
        std::string save_file_path = file_path.substr(0, file_path.length() - 3) + ".sav";
        std::ifstream file(save_file_path, std::ios::binary);
        if (!file.is_open()) return;
        file.seekg(0, std::ios::end);
        std::streampos size = file.tellg();
        file.seekg(0, std::ios::beg);
        file.read((char*)ram.get(), ram_size);
        file.close();
    }
}
