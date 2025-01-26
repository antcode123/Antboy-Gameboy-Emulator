#pragma once


#include <cstdint>


typedef unsigned char U8;
typedef unsigned short U16;


namespace Hardware {
    class MMU;
    class CPU;


    class Timer {
    public:
        CPU& cpu;
        MMU& mmu;
        U8 divider;
        U8 counter;
        U8 modulo;
        U8 clock_select;
        bool is_counter_enabled;
        int counter_period;
        int counter_ticks;
        int divider_period;
        int divider_ticks;

        Timer(MMU& _mmu, CPU& _cpu);
        void reset();
        U8 read(U16 address);
        void write(U16 address, U8 u8);
        void set_counter_period();
        void run(int last_instruction_ticks);
        void update_divider();
        void update_counter();
    };
}