#include "mbc.hpp"
#include "../Utilities/misc.hpp"


namespace Hardware {

    // I just want to give credit to https://hacktix.github.io/GBEDG/mbcs/mbc1/ for the invaluabe documentation on MBCs
    MBC::MBC(int _type) :
        type(_type),
        is_ram_enabled(false),
        is_rom_bank_mode(true),
        rom_bank(1),
        ram_bank(0) {}


    void MBC::write(U16 address, U8 u8) {}


    void MBC::modify_rom_bank(U16 address, U8& temporary_rom_bank) {}


    MBC1::MBC1() : MBC(1) {}


    void MBC1::write(U16 address, U8 u8) {
        if (address < 0x2000) is_ram_enabled = (u8 & 0xF) == 0xA; // Enables/disables RAM

        else if (address < 0x4000) {
            u8 &= 0b11111;
            if (u8 == 0) u8++; // Makes sure lower five bits of ROM bank is not 0
            rom_bank = (rom_bank & 0b1100000) | u8; // Modifies the lower five bits of the ROM bank
        }

        else if (address < 0x6000) {
            if (!is_rom_bank_mode) ram_bank = u8 & 0b11; // Modifies the RAM bank
            else rom_bank = ((u8 & 0b11) << 5) | (rom_bank & 0b11111); // Modifies bits 5 and 6 of the ROM bank
        }

        else if (address < 0x8000) is_rom_bank_mode = !Utilities::get_bit_u8(u8, 0); // Switches MBC1 bank mode
    }


    void MBC1::modify_rom_bank(U16 address, U8& temporary_rom_bank) {
        if (is_rom_bank_mode || total_rom_banks < 32) return;
        if (total_rom_banks == 64 || total_rom_banks == 128) Utilities::set_bit_u8(temporary_rom_bank, 5, Utilities::get_bit_u8(ram_bank, 0));
        if (total_rom_banks == 128) Utilities::set_bit_u8(temporary_rom_bank, 6, Utilities::get_bit_u8(ram_bank, 1));
    }


    MBC2::MBC2() : MBC(2) {}


    void MBC2::write(U16 address, U8 u8) {
        if (address >= 0x4000) return;
        if (Utilities::get_bit_u16(address, 8)) rom_bank = u8 & 0xF;
        else is_ram_enabled = (u8 & 0xF) == 0xA;
        if (rom_bank == 0) rom_bank++;
    }
}
