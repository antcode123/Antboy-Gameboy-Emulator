#pragma once


#include <cstdint>
#include <string>
#include <vector>
#include "mmu.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Hardware {
    class MMU;


    class CPU {
    public:
        enum Flag {ZERO, SUB, HALF_CARRY, CARRY};
        enum FlagCondition {IS_ZERO, IS_NOT_ZERO, IS_CARRY, IS_NOT_CARRY};
        enum CombinedRegister {AF, BC, DE, HL};
        U8 A, B, C, D, E, F, H, L;
        const int clock_speed;
        int ticks;
        bool is_halted;
        U16 program_counter;
        U16 stack_pointer;
        MMU& mmu;
        std::vector<std::string> logs;
        U8 interrupt_enabled;
        U8 interrupt_flag;
        bool is_interrupt_master_enabled;
        bool can_enable_interrupts;
        U8 last_opcode;

        CPU(MMU& _mmu);
        void reset();
        void set_interrupt(U8 interrupt_bit, bool state);
        void handle_interrupts();
        void call_interrupt_service_routine(U8 interrupt_bit);
        int run();
        U16 read_combined_register(CombinedRegister combined_register);
        void write_combined_register(CombinedRegister combined_register, U16 u16);
        void set_flag(Flag flag, bool state);
        bool get_flag(Flag flag);
        void push_onto_stack(U16 u16);
        U16 pop_off_stack();
        U8 fetch();
        int execute(U8 opcode);
    };
}
