#include <cstdint>
#include <fstream>
#include <sstream>
#include "cpu.hpp"
#include "mmu.hpp"
#include "../Opcodes/alu_opcodes.hpp"
#include "../Opcodes/load_opcodes.hpp"
#include "../Opcodes/func_opcodes.hpp"
#include "../Opcodes/jump_opcodes.hpp"
#include "../Opcodes/misc_opcodes.hpp"
#include "../Opcodes/bitmanip_opcodes.hpp"
#include "../Utilities/misc.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>


namespace Hardware {

    // Executes instructions, as well as handling interrupts
    // All other components are clocked by the ticks taken by the CPU's last instruction
    CPU::CPU(MMU& _mmu) :
        mmu(_mmu),
        clock_speed(4194304) {
        reset();
    };


    void CPU::reset() {
        ticks = 0;
        is_interrupt_master_enabled = false;
        stack_pointer = 0xFFFE;
        is_halted = false;
        interrupt_enabled = 0;
        interrupt_flag = 0;
        is_interrupt_master_enabled = false;
        can_enable_interrupts = false;
        write_combined_register(AF, 0x01B0);
        write_combined_register(BC, 0x0013);
        write_combined_register(DE, 0x00D8);
        write_combined_register(HL, 0x014D);
    }


    int CPU::run() {
        if (is_halted) return 4;
        U8 opcode = fetch();
        return execute(opcode);
    }



    U16 CPU::read_combined_register(CombinedRegister combined_register) {
        switch (combined_register) {
            case AF: return (U16)A << 8 | F;
            case BC: return (U16)B << 8 | C;
            case DE: return (U16)D << 8 | E;
            case HL: return (U16)H << 8 | L;
            default: return 0;
        }
    }


    void CPU::write_combined_register(CombinedRegister combined_register, U16 u16) {
        switch (combined_register) {
            case AF: A = u16 >> 8; F = u16 & 0xF0; break;
            case BC: B = u16 >> 8; C = u16 & 0xFF; break;
            case DE: D = u16 >> 8; E = u16 & 0xFF; break;
            case HL: H = u16 >> 8; L = u16 & 0xFF; break;
        }
    }


    void CPU::set_flag(Flag flag, bool state) {
        switch (flag) {
            case ZERO: Utilities::set_bit_u8(F, 7, state); break;
            case SUB: Utilities::set_bit_u8(F, 6, state); break;
            case HALF_CARRY: Utilities::set_bit_u8(F, 5, state); break;
            case CARRY: Utilities::set_bit_u8(F, 4, state); break;
        }
    }


    bool CPU::get_flag(Flag flag) {
        switch (flag) {
            case ZERO: return Utilities::get_bit_u8(F, 7);
            case SUB: return Utilities::get_bit_u8(F, 6);
            case HALF_CARRY: return Utilities::get_bit_u8(F, 5);
            case CARRY: return Utilities::get_bit_u8(F, 4);
            default: return 0;
        }
    }


    void CPU::push_onto_stack(U16 u16) {
        stack_pointer -= 2;
        mmu.write_u16(stack_pointer, u16);
    }


    U16 CPU::pop_off_stack() {
        U16 u16 = mmu.read_u16(stack_pointer);
        stack_pointer += 2;
        return u16;
    }


    void CPU::set_interrupt(U8 interrupt_bit, bool state) {Utilities::set_bit_u8(interrupt_flag, interrupt_bit, state);}


    void CPU::handle_interrupts() {
        U8 current_interrupts = interrupt_flag & interrupt_enabled; // Masks off any triggered interrupts which aren't enabled in the interrupt enabled register
        if (current_interrupts == 0) return; // Returns if there are no interrupts
        is_halted = false; // CPU exits halt mode when an interrupt occurs
        if (!is_interrupt_master_enabled) return; // Ignores a triggered interrupt when the IME is disabled
        if (Utilities::get_bit_u8(current_interrupts, 0)) call_interrupt_service_routine(0); // VBLANK interrupt
        else if (Utilities::get_bit_u8(current_interrupts, 1)) call_interrupt_service_routine(1); // LCD STATUS interrupt
        else if (Utilities::get_bit_u8(current_interrupts, 2)) call_interrupt_service_routine(2); // TIMER interrupt
        else if (Utilities::get_bit_u8(current_interrupts, 4)) call_interrupt_service_routine(4); // JOYPAD interrupt
    }


    void CPU::call_interrupt_service_routine(U8 interrupt_bit) {
        is_interrupt_master_enabled = false;
        push_onto_stack(program_counter);
        set_interrupt(interrupt_bit, false);

        switch (interrupt_bit) {
            case 0: program_counter = 0x40; break;
            case 1: program_counter = 0x48; break;
            case 2: program_counter = 0x50; break;
            case 4: program_counter = 0x60; break;
        }
    }


    U8 CPU::fetch() {
        U8 opcode = mmu.read_u8(program_counter);
        program_counter++;
        return opcode;
    }


    int CPU::execute(U8 opcode) {

        // Enable interrupts after delay
        if (can_enable_interrupts) {
            is_interrupt_master_enabled = true;
            can_enable_interrupts = false;
        }

        // Decoding prefixed opcodes
        if (opcode == 0xCB) {

            // Fetch a second time to get the prefixed opcode
            switch (fetch()) {
                case 0x00: return Opcodes::RLC_r(*this, B);
                case 0x01: return Opcodes::RLC_r(*this, C);
                case 0x02: return Opcodes::RLC_r(*this, D);
                case 0x03: return Opcodes::RLC_r(*this, E);
                case 0x04: return Opcodes::RLC_r(*this, H);
                case 0x05: return Opcodes::RLC_r(*this, L);
                case 0x06: return Opcodes::RLC_ptr_HL(*this);
                case 0x07: return Opcodes::RLC_r(*this, A);
                case 0x08: return Opcodes::RRC_r(*this, B);
                case 0x09: return Opcodes::RRC_r(*this, C);
                case 0x0A: return Opcodes::RRC_r(*this, D);
                case 0x0B: return Opcodes::RRC_r(*this, E);
                case 0x0C: return Opcodes::RRC_r(*this, H);
                case 0x0D: return Opcodes::RRC_r(*this, L);
                case 0x0E: return Opcodes::RRC_ptr_HL(*this);
                case 0x0F: return Opcodes::RRC_r(*this, A);
                case 0x10: return Opcodes::RL_r(*this, B);
                case 0x11: return Opcodes::RL_r(*this, C);
                case 0x12: return Opcodes::RL_r(*this, D);
                case 0x13: return Opcodes::RL_r(*this, E);
                case 0x14: return Opcodes::RL_r(*this, H);
                case 0x15: return Opcodes::RL_r(*this, L);
                case 0x16: return Opcodes::RL_ptr_HL(*this);
                case 0x17: return Opcodes::RL_r(*this, A);
                case 0x18: return Opcodes::RR_r(*this, B);
                case 0x19: return Opcodes::RR_r(*this, C);
                case 0x1A: return Opcodes::RR_r(*this, D);
                case 0x1B: return Opcodes::RR_r(*this, E);
                case 0x1C: return Opcodes::RR_r(*this, H);
                case 0x1D: return Opcodes::RR_r(*this, L);
                case 0x1E: return Opcodes::RR_ptr_HL(*this);
                case 0x1F: return Opcodes::RR_r(*this, A);
                case 0x20: return Opcodes::SLA_r(*this, B);
                case 0x21: return Opcodes::SLA_r(*this, C);
                case 0x22: return Opcodes::SLA_r(*this, D);
                case 0x23: return Opcodes::SLA_r(*this, E);
                case 0x24: return Opcodes::SLA_r(*this, H);
                case 0x25: return Opcodes::SLA_r(*this, L);
                case 0x26: return Opcodes::SLA_ptr_HL(*this);
                case 0x27: return Opcodes::SLA_r(*this, A);
                case 0x28: return Opcodes::SRA_r(*this, B);
                case 0x29: return Opcodes::SRA_r(*this, C);
                case 0x2A: return Opcodes::SRA_r(*this, D);
                case 0x2B: return Opcodes::SRA_r(*this, E);
                case 0x2C: return Opcodes::SRA_r(*this, H);
                case 0x2D: return Opcodes::SRA_r(*this, L);
                case 0x2E: return Opcodes::SRA_ptr_HL(*this);
                case 0x2F: return Opcodes::SRA_r(*this, A);
                case 0x30: return Opcodes::SWAP_r(*this, B);
                case 0x31: return Opcodes::SWAP_r(*this, C);
                case 0x32: return Opcodes::SWAP_r(*this, D);
                case 0x33: return Opcodes::SWAP_r(*this, E);
                case 0x34: return Opcodes::SWAP_r(*this, H);
                case 0x35: return Opcodes::SWAP_r(*this, L);
                case 0x36: return Opcodes::SWAP_ptr_HL(*this);
                case 0x37: return Opcodes::SWAP_r(*this, A);
                case 0x38: return Opcodes::SRL_r(*this, B);
                case 0x39: return Opcodes::SRL_r(*this, C);
                case 0x3A: return Opcodes::SRL_r(*this, D);
                case 0x3B: return Opcodes::SRL_r(*this, E);
                case 0x3C: return Opcodes::SRL_r(*this, H);
                case 0x3D: return Opcodes::SRL_r(*this, L);
                case 0x3E: return Opcodes::SRL_ptr_HL(*this);
                case 0x3F: return Opcodes::SRL_r(*this, A);
                case 0x40: return Opcodes::BIT_b_r(*this, 0, B);
                case 0x41: return Opcodes::BIT_b_r(*this, 0, C);
                case 0x42: return Opcodes::BIT_b_r(*this, 0, D);
                case 0x43: return Opcodes::BIT_b_r(*this, 0, E);
                case 0x44: return Opcodes::BIT_b_r(*this, 0, H);
                case 0x45: return Opcodes::BIT_b_r(*this, 0, L);
                case 0x46: return Opcodes::BIT_b_ptr_HL(*this, 0);
                case 0x47: return Opcodes::BIT_b_r(*this, 0, A);
                case 0x48: return Opcodes::BIT_b_r(*this, 1, B);
                case 0x49: return Opcodes::BIT_b_r(*this, 1, C);
                case 0x4A: return Opcodes::BIT_b_r(*this, 1, D);
                case 0x4B: return Opcodes::BIT_b_r(*this, 1, E);
                case 0x4C: return Opcodes::BIT_b_r(*this, 1, H);
                case 0x4D: return Opcodes::BIT_b_r(*this, 1, L);
                case 0x4E: return Opcodes::BIT_b_ptr_HL(*this, 1);
                case 0x4F: return Opcodes::BIT_b_r(*this, 1, A);
                case 0x50: return Opcodes::BIT_b_r(*this, 2, B);
                case 0x51: return Opcodes::BIT_b_r(*this, 2, C);
                case 0x52: return Opcodes::BIT_b_r(*this, 2, D);
                case 0x53: return Opcodes::BIT_b_r(*this, 2, E);
                case 0x54: return Opcodes::BIT_b_r(*this, 2, H);
                case 0x55: return Opcodes::BIT_b_r(*this, 2, L);
                case 0x56: return Opcodes::BIT_b_ptr_HL(*this, 2);
                case 0x57: return Opcodes::BIT_b_r(*this, 2, A);
                case 0x58: return Opcodes::BIT_b_r(*this, 3, B);
                case 0x59: return Opcodes::BIT_b_r(*this, 3, C);
                case 0x5A: return Opcodes::BIT_b_r(*this, 3, D);
                case 0x5B: return Opcodes::BIT_b_r(*this, 3, E);
                case 0x5C: return Opcodes::BIT_b_r(*this, 3, H);
                case 0x5D: return Opcodes::BIT_b_r(*this, 3, L);
                case 0x5E: return Opcodes::BIT_b_ptr_HL(*this, 3);
                case 0x5F: return Opcodes::BIT_b_r(*this, 3, A);
                case 0x60: return Opcodes::BIT_b_r(*this, 4, B);
                case 0x61: return Opcodes::BIT_b_r(*this, 4, C);
                case 0x62: return Opcodes::BIT_b_r(*this, 4, D);
                case 0x63: return Opcodes::BIT_b_r(*this, 4, E);
                case 0x64: return Opcodes::BIT_b_r(*this, 4, H);
                case 0x65: return Opcodes::BIT_b_r(*this, 4, L);
                case 0x66: return Opcodes::BIT_b_ptr_HL(*this, 4);
                case 0x67: return Opcodes::BIT_b_r(*this, 4, A);
                case 0x68: return Opcodes::BIT_b_r(*this, 5, B);
                case 0x69: return Opcodes::BIT_b_r(*this, 5, C);
                case 0x6A: return Opcodes::BIT_b_r(*this, 5, D);
                case 0x6B: return Opcodes::BIT_b_r(*this, 5, E);
                case 0x6C: return Opcodes::BIT_b_r(*this, 5, H);
                case 0x6D: return Opcodes::BIT_b_r(*this, 5, L);
                case 0x6E: return Opcodes::BIT_b_ptr_HL(*this, 5);
                case 0x6F: return Opcodes::BIT_b_r(*this, 5, A);
                case 0x70: return Opcodes::BIT_b_r(*this, 6, B);
                case 0x71: return Opcodes::BIT_b_r(*this, 6, C);
                case 0x72: return Opcodes::BIT_b_r(*this, 6, D);
                case 0x73: return Opcodes::BIT_b_r(*this, 6, E);
                case 0x74: return Opcodes::BIT_b_r(*this, 6, H);
                case 0x75: return Opcodes::BIT_b_r(*this, 6, L);
                case 0x76: return Opcodes::BIT_b_ptr_HL(*this, 6);
                case 0x77: return Opcodes::BIT_b_r(*this, 6, A);
                case 0x78: return Opcodes::BIT_b_r(*this, 7, B);
                case 0x79: return Opcodes::BIT_b_r(*this, 7, C);
                case 0x7A: return Opcodes::BIT_b_r(*this, 7, D);
                case 0x7B: return Opcodes::BIT_b_r(*this, 7, E);
                case 0x7C: return Opcodes::BIT_b_r(*this, 7, H);
                case 0x7D: return Opcodes::BIT_b_r(*this, 7, L);
                case 0x7E: return Opcodes::BIT_b_ptr_HL(*this, 7);
                case 0x7F: return Opcodes::BIT_b_r(*this, 7, A);
                case 0x80: return Opcodes::RES_b_r(*this, 0, B);
                case 0x81: return Opcodes::RES_b_r(*this, 0, C);
                case 0x82: return Opcodes::RES_b_r(*this, 0, D);
                case 0x83: return Opcodes::RES_b_r(*this, 0, E);
                case 0x84: return Opcodes::RES_b_r(*this, 0, H);
                case 0x85: return Opcodes::RES_b_r(*this, 0, L);
                case 0x86: return Opcodes::RES_b_ptr_HL(*this, 0);
                case 0x87: return Opcodes::RES_b_r(*this, 0, A);
                case 0x88: return Opcodes::RES_b_r(*this, 1, B);
                case 0x89: return Opcodes::RES_b_r(*this, 1, C);
                case 0x8A: return Opcodes::RES_b_r(*this, 1, D);
                case 0x8B: return Opcodes::RES_b_r(*this, 1, E);
                case 0x8C: return Opcodes::RES_b_r(*this, 1, H);
                case 0x8D: return Opcodes::RES_b_r(*this, 1, L);
                case 0x8E: return Opcodes::RES_b_ptr_HL(*this, 1);
                case 0x8F: return Opcodes::RES_b_r(*this, 1, A);
                case 0x90: return Opcodes::RES_b_r(*this, 2, B);
                case 0x91: return Opcodes::RES_b_r(*this, 2, C);
                case 0x92: return Opcodes::RES_b_r(*this, 2, D);
                case 0x93: return Opcodes::RES_b_r(*this, 2, E);
                case 0x94: return Opcodes::RES_b_r(*this, 2, H);
                case 0x95: return Opcodes::RES_b_r(*this, 2, L);
                case 0x96: return Opcodes::RES_b_ptr_HL(*this, 2);
                case 0x97: return Opcodes::RES_b_r(*this, 2, A);
                case 0x98: return Opcodes::RES_b_r(*this, 3, B);
                case 0x99: return Opcodes::RES_b_r(*this, 3, C);
                case 0x9A: return Opcodes::RES_b_r(*this, 3, D);
                case 0x9B: return Opcodes::RES_b_r(*this, 3, E);
                case 0x9C: return Opcodes::RES_b_r(*this, 3, H);
                case 0x9D: return Opcodes::RES_b_r(*this, 3, L);
                case 0x9E: return Opcodes::RES_b_ptr_HL(*this, 3);
                case 0x9F: return Opcodes::RES_b_r(*this, 3, A);
                case 0xA0: return Opcodes::RES_b_r(*this, 4, B);
                case 0xA1: return Opcodes::RES_b_r(*this, 4, C);
                case 0xA2: return Opcodes::RES_b_r(*this, 4, D);
                case 0xA3: return Opcodes::RES_b_r(*this, 4, E);
                case 0xA4: return Opcodes::RES_b_r(*this, 4, H);
                case 0xA5: return Opcodes::RES_b_r(*this, 4, L);
                case 0xA6: return Opcodes::RES_b_ptr_HL(*this, 4);
                case 0xA7: return Opcodes::RES_b_r(*this, 4, A);
                case 0xA8: return Opcodes::RES_b_r(*this, 5, B);
                case 0xA9: return Opcodes::RES_b_r(*this, 5, C);
                case 0xAA: return Opcodes::RES_b_r(*this, 5, D);
                case 0xAB: return Opcodes::RES_b_r(*this, 5, E);
                case 0xAC: return Opcodes::RES_b_r(*this, 5, H);
                case 0xAD: return Opcodes::RES_b_r(*this, 5, L);
                case 0xAE: return Opcodes::RES_b_ptr_HL(*this, 5);
                case 0xAF: return Opcodes::RES_b_r(*this, 5, A);
                case 0xB0: return Opcodes::RES_b_r(*this, 6, B);
                case 0xB1: return Opcodes::RES_b_r(*this, 6, C);
                case 0xB2: return Opcodes::RES_b_r(*this, 6, D);
                case 0xB3: return Opcodes::RES_b_r(*this, 6, E);
                case 0xB4: return Opcodes::RES_b_r(*this, 6, H);
                case 0xB5: return Opcodes::RES_b_r(*this, 6, L);
                case 0xB6: return Opcodes::RES_b_ptr_HL(*this, 6);
                case 0xB7: return Opcodes::RES_b_r(*this, 6, A);
                case 0xB8: return Opcodes::RES_b_r(*this, 7, B);
                case 0xB9: return Opcodes::RES_b_r(*this, 7, C);
                case 0xBA: return Opcodes::RES_b_r(*this, 7, D);
                case 0xBB: return Opcodes::RES_b_r(*this, 7, E);
                case 0xBC: return Opcodes::RES_b_r(*this, 7, H);
                case 0xBD: return Opcodes::RES_b_r(*this, 7, L);
                case 0xBE: return Opcodes::RES_b_ptr_HL(*this, 7);
                case 0xBF: return Opcodes::RES_b_r(*this, 7, A);
                case 0xC0: return Opcodes::SET_b_r(*this, 0, B);
                case 0xC1: return Opcodes::SET_b_r(*this, 0, C);
                case 0xC2: return Opcodes::SET_b_r(*this, 0, D);
                case 0xC3: return Opcodes::SET_b_r(*this, 0, E);
                case 0xC4: return Opcodes::SET_b_r(*this, 0, H);
                case 0xC5: return Opcodes::SET_b_r(*this, 0, L);
                case 0xC6: return Opcodes::SET_b_ptr_HL(*this, 0);
                case 0xC7: return Opcodes::SET_b_r(*this, 0, A);
                case 0xC8: return Opcodes::SET_b_r(*this, 1, B);
                case 0xC9: return Opcodes::SET_b_r(*this, 1, C);
                case 0xCA: return Opcodes::SET_b_r(*this, 1, D);
                case 0xCB: return Opcodes::SET_b_r(*this, 1, E);
                case 0xCC: return Opcodes::SET_b_r(*this, 1, H);
                case 0xCD: return Opcodes::SET_b_r(*this, 1, L);
                case 0xCE: return Opcodes::SET_b_ptr_HL(*this, 1);
                case 0xCF: return Opcodes::SET_b_r(*this, 1, A);
                case 0xD0: return Opcodes::SET_b_r(*this, 2, B);
                case 0xD1: return Opcodes::SET_b_r(*this, 2, C);
                case 0xD2: return Opcodes::SET_b_r(*this, 2, D);
                case 0xD3: return Opcodes::SET_b_r(*this, 2, E);
                case 0xD4: return Opcodes::SET_b_r(*this, 2, H);
                case 0xD5: return Opcodes::SET_b_r(*this, 2, L);
                case 0xD6: return Opcodes::SET_b_ptr_HL(*this, 2);
                case 0xD7: return Opcodes::SET_b_r(*this, 2, A);
                case 0xD8: return Opcodes::SET_b_r(*this, 3, B);
                case 0xD9: return Opcodes::SET_b_r(*this, 3, C);
                case 0xDA: return Opcodes::SET_b_r(*this, 3, D);
                case 0xDB: return Opcodes::SET_b_r(*this, 3, E);
                case 0xDC: return Opcodes::SET_b_r(*this, 3, H);
                case 0xDD: return Opcodes::SET_b_r(*this, 3, L);
                case 0xDE: return Opcodes::SET_b_ptr_HL(*this, 3);
                case 0xDF: return Opcodes::SET_b_r(*this, 3, A);
                case 0xE0: return Opcodes::SET_b_r(*this, 4, B);
                case 0xE1: return Opcodes::SET_b_r(*this, 4, C);
                case 0xE2: return Opcodes::SET_b_r(*this, 4, D);
                case 0xE3: return Opcodes::SET_b_r(*this, 4, E);
                case 0xE4: return Opcodes::SET_b_r(*this, 4, H);
                case 0xE5: return Opcodes::SET_b_r(*this, 4, L);
                case 0xE6: return Opcodes::SET_b_ptr_HL(*this, 4);
                case 0xE7: return Opcodes::SET_b_r(*this, 4, A);
                case 0xE8: return Opcodes::SET_b_r(*this, 5, B);
                case 0xE9: return Opcodes::SET_b_r(*this, 5, C);
                case 0xEA: return Opcodes::SET_b_r(*this, 5, D);
                case 0xEB: return Opcodes::SET_b_r(*this, 5, E);
                case 0xEC: return Opcodes::SET_b_r(*this, 5, H);
                case 0xED: return Opcodes::SET_b_r(*this, 5, L);
                case 0xEE: return Opcodes::SET_b_ptr_HL(*this, 5);
                case 0xEF: return Opcodes::SET_b_r(*this, 5, A);
                case 0xF0: return Opcodes::SET_b_r(*this, 6, B);
                case 0xF1: return Opcodes::SET_b_r(*this, 6, C);
                case 0xF2: return Opcodes::SET_b_r(*this, 6, D);
                case 0xF3: return Opcodes::SET_b_r(*this, 6, E);
                case 0xF4: return Opcodes::SET_b_r(*this, 6, H);
                case 0xF5: return Opcodes::SET_b_r(*this, 6, L);
                case 0xF6: return Opcodes::SET_b_ptr_HL(*this, 6);
                case 0xF7: return Opcodes::SET_b_r(*this, 6, A);
                case 0xF8: return Opcodes::SET_b_r(*this, 7, B);
                case 0xF9: return Opcodes::SET_b_r(*this, 7, C);
                case 0xFA: return Opcodes::SET_b_r(*this, 7, D);
                case 0xFB: return Opcodes::SET_b_r(*this, 7, E);
                case 0xFC: return Opcodes::SET_b_r(*this, 7, H);
                case 0xFD: return Opcodes::SET_b_r(*this, 7, L);
                case 0xFE: return Opcodes::SET_b_ptr_HL(*this, 7);
                case 0xFF: return Opcodes::SET_b_r(*this, 7, A);
                default: return 0;
            }
        }

        // Decoding unprefixed opcodes
        else {
            switch (opcode) {
                case 0x00: return Opcodes::NOP(*this);
                case 0x01: return Opcodes::LD_rr_u16(*this, BC);
                case 0x02: return Opcodes::LD_ptr_rr_A(*this, BC);
                case 0x03: return Opcodes::INC_rr(*this, BC);
                case 0x04: return Opcodes::INC_r(*this, B);
                case 0x05: return Opcodes::DEC_r(*this, B);
                case 0x06: return Opcodes::LD_r_u8(*this, B);
                case 0x07: return Opcodes::RLCA(*this);
                case 0x08: return Opcodes::LD_ptr_u16_SP(*this);
                case 0x09: return Opcodes::ADD_HL_rr(*this, BC);
                case 0x0A: return Opcodes::LD_A_ptr_rr(*this, BC);
                case 0x0B: return Opcodes::DEC_rr(*this, BC);
                case 0x0C: return Opcodes::INC_r(*this, C);
                case 0x0D: return Opcodes::DEC_r(*this, C);
                case 0x0E: return Opcodes::LD_r_u8(*this, C);
                case 0x0F: return Opcodes::RRCA(*this);
                case 0x11: return Opcodes::LD_rr_u16(*this, DE);
                case 0x12: return Opcodes::LD_ptr_rr_A(*this, DE);
                case 0x13: return Opcodes::INC_rr(*this, DE);
                case 0x14: return Opcodes::INC_r(*this, D);
                case 0x15: return Opcodes::DEC_r(*this, D);
                case 0x16: return Opcodes::LD_r_u8(*this, D);
                case 0x17: return Opcodes::RLA(*this);
                case 0x18: return Opcodes::JR_s8(*this);
                case 0x19: return Opcodes::ADD_HL_rr(*this, DE);
                case 0x1A: return Opcodes::LD_A_ptr_rr(*this, DE);
                case 0x1B: return Opcodes::DEC_rr(*this, DE);
                case 0x1C: return Opcodes::INC_r(*this, E);
                case 0x1D: return Opcodes::DEC_r(*this, E);
                case 0x1E: return Opcodes::LD_r_u8(*this, E);
                case 0x1F: return Opcodes::RRA(*this);
                case 0x20: return Opcodes::JR_cc_s8(*this, IS_NOT_ZERO);
                case 0x21: return Opcodes::LD_rr_u16(*this, HL);
                case 0x22: return Opcodes::LDI_ptr_HL_A(*this);
                case 0x23: return Opcodes::INC_rr(*this, HL);
                case 0x24: return Opcodes::INC_r(*this, H);
                case 0x25: return Opcodes::DEC_r(*this, H);
                case 0x26: return Opcodes::LD_r_u8(*this, H);
                case 0x27: return Opcodes::DAA(*this);
                case 0x28: return Opcodes::JR_cc_s8(*this, IS_ZERO);
                case 0x29: return Opcodes::ADD_HL_rr(*this, HL);
                case 0x2A: return Opcodes::LDI_A_ptr_HL(*this);
                case 0x2B: return Opcodes::DEC_rr(*this, HL);
                case 0x2C: return Opcodes::INC_r(*this, L);
                case 0x2D: return Opcodes::DEC_r(*this, L);
                case 0x2E: return Opcodes::LD_r_u8(*this, L);
                case 0x2F: return Opcodes::CPL(*this);
                case 0x30: return Opcodes::JR_cc_s8(*this, IS_NOT_CARRY);
                case 0x31: return Opcodes::LD_SP_u16(*this);
                case 0x32: return Opcodes::LDD_ptr_HL_A(*this);
                case 0x33: return Opcodes::INC_SP(*this);
                case 0x34: return Opcodes::INC_ptr_HL(*this);
                case 0x35: return Opcodes::DEC_ptr_HL(*this);
                case 0x36: return Opcodes::LD_ptr_HL_u8(*this);
                case 0x37: return Opcodes::SCF(*this);
                case 0x38: return Opcodes::JR_cc_s8(*this, IS_CARRY);
                case 0x39: return Opcodes::ADD_HL_SP(*this);
                case 0x3A: return Opcodes::LDD_A_ptr_HL(*this);
                case 0x3B: return Opcodes::DEC_SP(*this);
                case 0x3C: return Opcodes::INC_r(*this, A);
                case 0x3D: return Opcodes::DEC_r(*this, A);
                case 0x3E: return Opcodes::LD_r_u8(*this, A);
                case 0x3F: return Opcodes::CCF(*this);
                case 0x40: return Opcodes::LD_r_r(*this, B, B);
                case 0x41: return Opcodes::LD_r_r(*this, B, C);
                case 0x42: return Opcodes::LD_r_r(*this, B, D);
                case 0x43: return Opcodes::LD_r_r(*this, B, E);
                case 0x44: return Opcodes::LD_r_r(*this, B, H);
                case 0x45: return Opcodes::LD_r_r(*this, B, L);
                case 0x46: return Opcodes::LD_r_ptr_HL(*this, B);
                case 0x47: return Opcodes::LD_r_r(*this, B, A);
                case 0x48: return Opcodes::LD_r_r(*this, C, B);
                case 0x49: return Opcodes::LD_r_r(*this, C, C);
                case 0x4A: return Opcodes::LD_r_r(*this, C, D);
                case 0x4B: return Opcodes::LD_r_r(*this, C, E);
                case 0x4C: return Opcodes::LD_r_r(*this, C, H);
                case 0x4D: return Opcodes::LD_r_r(*this, C, L);
                case 0x4E: return Opcodes::LD_r_ptr_HL(*this, C);
                case 0x4F: return Opcodes::LD_r_r(*this, C, A);
                case 0x50: return Opcodes::LD_r_r(*this, D, B);
                case 0x51: return Opcodes::LD_r_r(*this, D, C);
                case 0x52: return Opcodes::LD_r_r(*this, D, D);
                case 0x53: return Opcodes::LD_r_r(*this, D, E);
                case 0x54: return Opcodes::LD_r_r(*this, D, H);
                case 0x55: return Opcodes::LD_r_r(*this, D, L);
                case 0x56: return Opcodes::LD_r_ptr_HL(*this, D);
                case 0x57: return Opcodes::LD_r_r(*this, D, A);
                case 0x58: return Opcodes::LD_r_r(*this, E, B);
                case 0x59: return Opcodes::LD_r_r(*this, E, C);
                case 0x5A: return Opcodes::LD_r_r(*this, E, D);
                case 0x5B: return Opcodes::LD_r_r(*this, E, E);
                case 0x5C: return Opcodes::LD_r_r(*this, E, H);
                case 0x5D: return Opcodes::LD_r_r(*this, E, L);
                case 0x5E: return Opcodes::LD_r_ptr_HL(*this, E);
                case 0x5F: return Opcodes::LD_r_r(*this, E, A);
                case 0x60: return Opcodes::LD_r_r(*this, H, B);
                case 0x61: return Opcodes::LD_r_r(*this, H, C);
                case 0x62: return Opcodes::LD_r_r(*this, H, D);
                case 0x63: return Opcodes::LD_r_r(*this, H, E);
                case 0x64: return Opcodes::LD_r_r(*this, H, H);
                case 0x65: return Opcodes::LD_r_r(*this, H, L);
                case 0x66: return Opcodes::LD_r_ptr_HL(*this, H);
                case 0x67: return Opcodes::LD_r_r(*this, H, A);
                case 0x68: return Opcodes::LD_r_r(*this, L, B);
                case 0x69: return Opcodes::LD_r_r(*this, L, C);
                case 0x6A: return Opcodes::LD_r_r(*this, L, D);
                case 0x6B: return Opcodes::LD_r_r(*this, L, E);
                case 0x6C: return Opcodes::LD_r_r(*this, L, H);
                case 0x6D: return Opcodes::LD_r_r(*this, L, L);
                case 0x6E: return Opcodes::LD_r_ptr_HL(*this, L);
                case 0x6F: return Opcodes::LD_r_r(*this, L, A);
                case 0x70: return Opcodes::LD_ptr_HL_r(*this, B);
                case 0x71: return Opcodes::LD_ptr_HL_r(*this, C);
                case 0x72: return Opcodes::LD_ptr_HL_r(*this, D);
                case 0x73: return Opcodes::LD_ptr_HL_r(*this, E);
                case 0x74: return Opcodes::LD_ptr_HL_r(*this, H);
                case 0x75: return Opcodes::LD_ptr_HL_r(*this, L);
                case 0x76: return Opcodes::HALT(*this);
                case 0x77: return Opcodes::LD_ptr_HL_r(*this, A);
                case 0x78: return Opcodes::LD_r_r(*this, A, B);
                case 0x79: return Opcodes::LD_r_r(*this, A, C);
                case 0x7A: return Opcodes::LD_r_r(*this, A, D);
                case 0x7B: return Opcodes::LD_r_r(*this, A, E);
                case 0x7C: return Opcodes::LD_r_r(*this, A, H);
                case 0x7D: return Opcodes::LD_r_r(*this, A, L);
                case 0x7E: return Opcodes::LD_r_ptr_HL(*this, A);
                case 0x7F: return Opcodes::LD_r_r(*this, A, A);
                case 0x80: return Opcodes::ADD_r(*this, B);
                case 0x81: return Opcodes::ADD_r(*this, C);
                case 0x82: return Opcodes::ADD_r(*this, D);
                case 0x83: return Opcodes::ADD_r(*this, E);
                case 0x84: return Opcodes::ADD_r(*this, H);
                case 0x85: return Opcodes::ADD_r(*this, L);
                case 0x86: return Opcodes::ADD_ptr_HL(*this);
                case 0x87: return Opcodes::ADD_r(*this, A);
                case 0x88: return Opcodes::ADC_r(*this, B);
                case 0x89: return Opcodes::ADC_r(*this, C);
                case 0x8A: return Opcodes::ADC_r(*this, D);
                case 0x8B: return Opcodes::ADC_r(*this, E);
                case 0x8C: return Opcodes::ADC_r(*this, H);
                case 0x8D: return Opcodes::ADC_r(*this, L);
                case 0x8E: return Opcodes::ADC_ptr_HL(*this);
                case 0x8F: return Opcodes::ADC_r(*this, A);
                case 0x90: return Opcodes::SUB_r(*this, B);
                case 0x91: return Opcodes::SUB_r(*this, C);
                case 0x92: return Opcodes::SUB_r(*this, D);
                case 0x93: return Opcodes::SUB_r(*this, E);
                case 0x94: return Opcodes::SUB_r(*this, H);
                case 0x95: return Opcodes::SUB_r(*this, L);
                case 0x96: return Opcodes::SUB_ptr_HL(*this);
                case 0x97: return Opcodes::SUB_r(*this, A);
                case 0x98: return Opcodes::SBC_r(*this, B);
                case 0x99: return Opcodes::SBC_r(*this, C);
                case 0x9A: return Opcodes::SBC_r(*this, D);
                case 0x9B: return Opcodes::SBC_r(*this, E);
                case 0x9C: return Opcodes::SBC_r(*this, H);
                case 0x9D: return Opcodes::SBC_r(*this, L);
                case 0x9E: return Opcodes::SBC_ptr_HL(*this);
                case 0x9F: return Opcodes::SBC_r(*this, A);
                case 0xA0: return Opcodes::AND_r(*this, B);
                case 0xA1: return Opcodes::AND_r(*this, C);
                case 0xA2: return Opcodes::AND_r(*this, D);
                case 0xA3: return Opcodes::AND_r(*this, E);
                case 0xA4: return Opcodes::AND_r(*this, H);
                case 0xA5: return Opcodes::AND_r(*this, L);
                case 0xA6: return Opcodes::AND_ptr_HL(*this);
                case 0xA7: return Opcodes::AND_r(*this, A);
                case 0xA8: return Opcodes::XOR_r(*this, B);
                case 0xA9: return Opcodes::XOR_r(*this, C);
                case 0xAA: return Opcodes::XOR_r(*this, D);
                case 0xAB: return Opcodes::XOR_r(*this, E);
                case 0xAC: return Opcodes::XOR_r(*this, H);
                case 0xAD: return Opcodes::XOR_r(*this, L);
                case 0xAE: return Opcodes::XOR_ptr_HL(*this);
                case 0xAF: return Opcodes::XOR_r(*this, A);
                case 0xB0: return Opcodes::OR_r(*this, B);
                case 0xB1: return Opcodes::OR_r(*this, C);
                case 0xB2: return Opcodes::OR_r(*this, D);
                case 0xB3: return Opcodes::OR_r(*this, E);
                case 0xB4: return Opcodes::OR_r(*this, H);
                case 0xB5: return Opcodes::OR_r(*this, L);
                case 0xB6: return Opcodes::OR_ptr_HL(*this);
                case 0xB7: return Opcodes::OR_r(*this, A);
                case 0xB8: return Opcodes::CP_r(*this, B);
                case 0xB9: return Opcodes::CP_r(*this, C);
                case 0xBA: return Opcodes::CP_r(*this, D);
                case 0xBB: return Opcodes::CP_r(*this, E);
                case 0xBC: return Opcodes::CP_r(*this, H);
                case 0xBD: return Opcodes::CP_r(*this, L);
                case 0xBE: return Opcodes::CP_ptr_HL(*this);
                case 0xBF: return Opcodes::CP_r(*this, A);
                case 0xC0: return Opcodes::RET_cc(*this, IS_NOT_ZERO);
                case 0xC1: return Opcodes::POP_rr(*this, BC);
                case 0xC2: return Opcodes::JP_cc_u16(*this, IS_NOT_ZERO);
                case 0xC3: return Opcodes::JP_u16(*this);
                case 0xC4: return Opcodes::CALL_cc_u16(*this, IS_NOT_ZERO);
                case 0xC5: return Opcodes::PUSH_rr(*this, BC);
                case 0xC6: return Opcodes::ADD_u8(*this);
                case 0xC7: return Opcodes::RST_n(*this, 0);
                case 0xC8: return Opcodes::RET_cc(*this, IS_ZERO);
                case 0xC9: return Opcodes::RET(*this);
                case 0xCA: return Opcodes::JP_cc_u16(*this, IS_ZERO);
                case 0xCC: return Opcodes::CALL_cc_u16(*this, IS_ZERO);
                case 0xCD: return Opcodes::CALL_u16(*this);
                case 0xCE: return Opcodes::ADC_u8(*this);
                case 0xCF: return Opcodes::RST_n(*this, 1);
                case 0xD0: return Opcodes::RET_cc(*this, IS_NOT_CARRY);
                case 0xD1: return Opcodes::POP_rr(*this, DE);
                case 0xD2: return Opcodes::JP_cc_u16(*this, IS_NOT_CARRY);
                case 0xD4: return Opcodes::CALL_cc_u16(*this, IS_NOT_CARRY);
                case 0xD5: return Opcodes::PUSH_rr(*this, DE);
                case 0xD6: return Opcodes::SUB_u8(*this);
                case 0xD7: return Opcodes::RST_n(*this, 2);
                case 0xD8: return Opcodes::RET_cc(*this, IS_CARRY);
                case 0xD9: return Opcodes::RETI(*this);
                case 0xDA: return Opcodes::JP_cc_u16(*this, IS_CARRY);
                case 0xDC: return Opcodes::CALL_cc_u16(*this, IS_CARRY);
                case 0xDE: return Opcodes::SBC_u8(*this);
                case 0xDF: return Opcodes::RST_n(*this, 3);
                case 0xE0: return Opcodes::LDH_ptr_u8_A(*this);
                case 0xE1: return Opcodes::POP_rr(*this, HL);
                case 0xE2: return Opcodes::LDH_ptr_C_A(*this);
                case 0xE5: return Opcodes::PUSH_rr(*this, HL);
                case 0xE6: return Opcodes::AND_u8(*this);
                case 0xE7: return Opcodes::RST_n(*this, 4);
                case 0xE8: return Opcodes::ADD_SP_s8(*this);
                case 0xE9: return Opcodes::JP_HL(*this);
                case 0xEA: return Opcodes::LD_ptr_u16_A(*this);
                case 0xEE: return Opcodes::XOR_u8(*this);
                case 0xEF: return Opcodes::RST_n(*this, 5);
                case 0xF0: return Opcodes::LDH_A_ptr_u8(*this);
                case 0xF1: return Opcodes::POP_rr(*this, AF);
                case 0xF2: return Opcodes::LDH_A_ptr_C(*this);
                case 0xF3: return Opcodes::DI(*this);
                case 0xF5: return Opcodes::PUSH_rr(*this, AF);
                case 0xF6: return Opcodes::OR_u8(*this);
                case 0xF7: return Opcodes::RST_n(*this, 6);
                case 0xF8: return Opcodes::LD_HL_SP_s8(*this);
                case 0xF9: return Opcodes::LD_SP_HL(*this);
                case 0xFA: return Opcodes::LD_A_ptr_u16(*this);
                case 0xFB: return Opcodes::EI(*this);
                case 0xFE: return Opcodes::CP_u8(*this);
                case 0xFF: return Opcodes::RST_n(*this, 7);
                default: return 0;
            }
        }
    }
}