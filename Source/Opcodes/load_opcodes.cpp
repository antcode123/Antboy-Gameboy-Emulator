#include "load_opcodes.hpp"
#include "../Hardware/cpu.hpp"
#include "../Utilities/misc.hpp"
#include "opcodes_helper.hpp"
#include <iostream>


namespace Opcodes {
    int LD_r_u8(Hardware::CPU& cpu, U8& reg) {
        reg = cpu.mmu.read_u8(cpu.program_counter);
        cpu.program_counter++;
        return 8;
    }


    int LD_r_r(Hardware::CPU& cpu, U8& reg1, U8 reg2) {
        reg1 = reg2;
        return 4;
    }


    int LD_r_ptr_HL(Hardware::CPU& cpu, U8& reg) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        reg = cpu.mmu.read_u8(hl_pointer);
        return 8;
    }


    int LD_ptr_HL_r(Hardware::CPU& cpu, U8 reg) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        cpu.mmu.write_u8(hl_pointer, reg);
        return 8;
    }


    int LD_ptr_HL_u8(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        cpu.mmu.write_u8(hl_pointer, cpu.mmu.read_u8(cpu.program_counter));
        cpu.program_counter++;
        return 12;
    }


    int LD_A_ptr_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register) {
        U16 pointer = cpu.read_combined_register(combined_register);
        cpu.A = cpu.mmu.read_u8(pointer);
        return 8;
    }


    int LD_ptr_rr_A(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register) {
        U16 pointer = cpu.read_combined_register(combined_register);
        cpu.mmu.write_u8(pointer, cpu.A);
        return 8;
    }


    int LD_A_ptr_u16(Hardware::CPU& cpu) {
        U16 pointer = cpu.mmu.read_u16(cpu.program_counter);
        cpu.A = cpu.mmu.read_u8(pointer);
        cpu.program_counter += 2;
        return 16;
    }


    int LD_ptr_u16_A(Hardware::CPU& cpu) {
        U16 pointer = cpu.mmu.read_u16(cpu.program_counter);
        cpu.mmu.write_u8(pointer, cpu.A);
        cpu.program_counter += 2;
        return 16;
    }


    int LDH_A_ptr_C(Hardware::CPU& cpu) {
        cpu.A = cpu.mmu.read_u8(0xFF00 + cpu.C);
        return 8;
    }


    int LDH_ptr_C_A(Hardware::CPU& cpu) {
        cpu.mmu.write_u8(0xFF00 + cpu.C, cpu.A);
        return 8;
    }


    int LDH_A_ptr_u8(Hardware::CPU& cpu) {
        U16 pointer = 0xFF00 + cpu.mmu.read_u8(cpu.program_counter);
        cpu.A = cpu.mmu.read_u8(pointer);
        cpu.program_counter++;
        return 12;
    }


    int LDH_ptr_u8_A(Hardware::CPU& cpu) {
        U16 pointer = 0xFF00 + cpu.mmu.read_u8(cpu.program_counter);
        cpu.mmu.write_u8(pointer, cpu.A);
        cpu.program_counter++;
        return 12;
    }


    int LDD_A_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        cpu.A = cpu.mmu.read_u8(hl_pointer);
        cpu.write_combined_register(cpu.HL, hl_pointer - 1);
        return 8;
    }


    int LDD_ptr_HL_A(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        cpu.mmu.write_u8(hl_pointer, cpu.A);
        cpu.write_combined_register(cpu.HL, hl_pointer - 1);
        return 8;
    }


    int LDI_A_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        cpu.A = cpu.mmu.read_u8(hl_pointer);
        cpu.write_combined_register(cpu.HL, hl_pointer + 1);
        return 8;
    }


    int LDI_ptr_HL_A(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        cpu.mmu.write_u8(hl_pointer, cpu.A);
        cpu.write_combined_register(cpu.HL, hl_pointer + 1);
        return 8;
    }


    int LD_rr_u16(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register) {
        cpu.write_combined_register(combined_register, cpu.mmu.read_u16(cpu.program_counter));
        cpu.program_counter += 2;
        return 12;
    }


    int LD_SP_u16(Hardware::CPU& cpu) {
        cpu.stack_pointer = cpu.mmu.read_u16(cpu.program_counter);
        cpu.program_counter += 2;
        return 12;
    }


    int LD_ptr_u16_SP(Hardware::CPU& cpu) {
        U16 pointer = cpu.mmu.read_u16(cpu.program_counter);
        cpu.mmu.write_u16(pointer, cpu.stack_pointer);
        cpu.program_counter += 2;
        return 20;
    }


    int LD_SP_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        cpu.stack_pointer = hl_pointer;
        return 8;
    }


    int LD_HL_SP_s8(Hardware::CPU& cpu) {
        U8 s8 = cpu.mmu.read_u8(cpu.program_counter);
        std::tuple<U16, bool, bool> result;
        if (Utilities::get_bit_u8(s8, 7)) result = add_u16(cpu.stack_pointer, (U16)s8 | 0xFF00, 7, 3);
        else result = add_u16(cpu.stack_pointer, s8, 7, 3);
        U16 sum = std::get<0>(result);
        bool is_half_carry = std::get<1>(result);
        bool is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, false);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, is_carry);
        cpu.write_combined_register(cpu.HL, sum);
        cpu.program_counter++;
        return 12;
    }
};