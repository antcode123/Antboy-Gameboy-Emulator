#pragma once


#include <cstdint>
#include <string>
#include <memory>
#include "cpu.hpp"
#include "ppu.hpp"
#include "cartridge.hpp"
#include "joypad.hpp"
#include "timer.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Hardware {
    class CPU;
    class PPU;
    class Cartridge;
    class Joypad;
    class Timer;


    class MMU {
    public:
        CPU& cpu;
        PPU& ppu;
        Cartridge& cartridge;
        Joypad& joypad;
        Timer& timer;
        bool is_bootstrap_enabled;
        std::string exe_path;
        U8 bootstrap[256];
        std::unique_ptr<U8[]> work_ram;
        std::unique_ptr<U8[]> high_ram;

        MMU(CPU& _cpu, PPU& _ppu, Cartridge& _cartridge, Joypad& Joypad, Timer& timer, std::string _exe_path);
        void reset();
        void load_bootstrap();
        U8 read_u8(U16 address);
        U16 read_u16(U16 address);
        void write_u8(U16 address, U8 u8);
        void write_u16(U16 address, U16 u16);
        void perform_dma_transfer(U8 encoded_source_address);
    };
}