#include <tuple>
#include "alu_opcodes.hpp"
#include "opcodes_helper.hpp"
#include "../Hardware/cpu.hpp"
#include "../Utilities/misc.hpp"


namespace Opcodes {

    void ADD_n(Hardware::CPU& cpu, U8 u8) {
        std::tuple<U8, bool, bool> result = add_u8(cpu.A, u8, 7, 3);
        U8 sum = std::get<0>(result);
        bool is_half_carry = std::get<1>(result);
        bool is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, sum == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, is_carry);
        cpu.A = sum;
    }


    int ADD_r(Hardware::CPU& cpu, U8 cpu_register) {
        ADD_n(cpu, cpu_register);
        return 4;
    }


    int ADD_u8(Hardware::CPU& cpu) {
        ADD_n(cpu, cpu.mmu.read_u8(cpu.program_counter));
        cpu.program_counter++;
        return 8;
    }


    int ADD_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        ADD_n(cpu, cpu.mmu.read_u8(hl_pointer));
        return 8;
    }


    void ADC_n(Hardware::CPU& cpu, U8 u8) {
        std::tuple<U8, bool, bool> result = add_u8((U8)cpu.get_flag(Hardware::CPU::Flag::CARRY), u8, 7, 3);
        U8 sum = std::get<0>(result);
        bool is_half_carry = std::get<1>(result);
        bool is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, is_carry);
        result = add_u8(cpu.A, sum, 7, 3);
        sum = std::get<0>(result);
        is_half_carry = std::get<1>(result);
        is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, sum == 0);
        if (!cpu.get_flag(Hardware::CPU::Flag::HALF_CARRY)) cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        if (!cpu.get_flag(Hardware::CPU::Flag::CARRY)) cpu.set_flag(Hardware::CPU::Flag::CARRY, is_carry);
        cpu.A = sum;
    }


    int ADC_r(Hardware::CPU& cpu, U8 cpu_register) {
        ADC_n(cpu, cpu_register);
        return 4;
    }


    int ADC_u8(Hardware::CPU& cpu) {
        ADC_n(cpu, cpu.mmu.read_u8(cpu.program_counter));
        cpu.program_counter++;
        return 8;
    }


    int ADC_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        ADC_n(cpu, cpu.mmu.read_u8(hl_pointer));
        return 8;
    }


    void SUB_n(Hardware::CPU& cpu, U8 u8) {
        std::tuple<U8, bool, bool> result = sub_u8(cpu.A, u8, 3);
        U8 difference = std::get<0>(result);
        bool is_half_carry = std::get<1>(result);
        bool is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, difference == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, true);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, is_carry);
        cpu.A = difference;
    }


    int SUB_r(Hardware::CPU& cpu, U8 cpu_register) {
        SUB_n(cpu, cpu_register);
        return 4;
    }


    int SUB_u8(Hardware::CPU& cpu) {
        SUB_n(cpu, cpu.mmu.read_u8(cpu.program_counter));
        cpu.program_counter++;
        return 8;
    }


    int SUB_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        SUB_n(cpu, cpu.mmu.read_u8(hl_pointer));
        return 8;
    }


    void SBC_n(Hardware::CPU& cpu, U8 u8) {
        std::tuple<U8, bool, bool> result = add_u8((U8)cpu.get_flag(Hardware::CPU::Flag::CARRY), u8, 7, 3);
        U8 difference = std::get<0>(result);
        bool is_half_carry = std::get<1>(result);
        bool is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::SUB, true);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, is_carry);
        result = sub_u8(cpu.A, difference, 3);
        difference = std::get<0>(result);
        is_half_carry = std::get<1>(result);
        is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, difference == 0);
        if (!cpu.get_flag(Hardware::CPU::Flag::HALF_CARRY)) cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        if (!cpu.get_flag(Hardware::CPU::Flag::CARRY)) cpu.set_flag(Hardware::CPU::Flag::CARRY, is_carry);

        cpu.A = difference;
    }


    int SBC_r(Hardware::CPU& cpu, U8 cpu_register) {
        SBC_n(cpu, cpu_register);
        return 4;
    }


    int SBC_u8(Hardware::CPU& cpu) {
        SBC_n(cpu, cpu.mmu.read_u8(cpu.program_counter));
        cpu.program_counter++;
        return 8;
    }


    int SBC_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        SBC_n(cpu, cpu.mmu.read_u8(hl_pointer));
        return 8;
    }


    void INC_n(Hardware::CPU& cpu, U8& u8) {
        std::tuple<U8, bool, bool> result = add_u8(u8, 1, 7, 3);
        U8 sum = std::get<0>(result);
        bool is_half_carry = std::get<1>(result);
        bool is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, sum == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        u8 = sum;
    }


    int INC_r(Hardware::CPU& cpu, U8& cpu_register) {
        INC_n(cpu, cpu_register);
        return 4;
    }


    int INC_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer);
        INC_n(cpu, u8);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 12;
    }


    void DEC_n(Hardware::CPU& cpu, U8& u8) {
        std::tuple<U8, bool, bool> result = sub_u8(u8, 1, 3);
        U8 difference = std::get<0>(result);
        bool is_half_carry = std::get<1>(result);
        bool is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, difference == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, true);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        u8 = difference;
    }


    int DEC_r(Hardware::CPU& cpu, U8& cpu_register) {
        DEC_n(cpu, cpu_register);
        return 4;
    }


    int DEC_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer);
        DEC_n(cpu, u8);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 12;
    }


    void AND_n(Hardware::CPU& cpu, U8 u8) {
        cpu.A &= u8;
        cpu.set_flag(Hardware::CPU::Flag::ZERO, cpu.A == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, true);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, false);
    }


    int AND_r(Hardware::CPU& cpu, U8 cpu_register) {
        AND_n(cpu, cpu_register);
        return 4;
    }


    int AND_u8(Hardware::CPU& cpu) {
        AND_n(cpu, cpu.mmu.read_u8(cpu.program_counter));
        cpu.program_counter++;
        return 8;
    }


    int AND_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        AND_n(cpu, cpu.mmu.read_u8(hl_pointer));
        return 8;
    }


    void OR_n(Hardware::CPU& cpu, U8 u8) {
        cpu.A |= u8;
        cpu.set_flag(Hardware::CPU::Flag::ZERO, cpu.A == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, false);
    }


    int OR_r(Hardware::CPU& cpu, U8 cpu_register) {
        OR_n(cpu, cpu_register);
        return 4;
    }


    int OR_u8(Hardware::CPU& cpu) {
        OR_n(cpu, cpu.mmu.read_u8(cpu.program_counter));
        cpu.program_counter++;
        return 8;
    }


    int OR_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        OR_n(cpu, cpu.mmu.read_u8(hl_pointer));
        return 8;
    }


    void XOR_n(Hardware::CPU& cpu, U8 u8) {
        cpu.A ^= u8;
        cpu.set_flag(Hardware::CPU::Flag::ZERO, cpu.A == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, false);
    }


    int XOR_r(Hardware::CPU& cpu, U8 cpu_register) {
        XOR_n(cpu, cpu_register);
        return 4;
    }


    int XOR_u8(Hardware::CPU& cpu) {
        XOR_n(cpu, cpu.mmu.read_u8(cpu.program_counter));
        cpu.program_counter++;
        return 8;
    }


    int XOR_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        XOR_n(cpu, cpu.mmu.read_u8(hl_pointer));
        return 8;
    }


    void CP_n(Hardware::CPU& cpu, U8 u8) {
        std::tuple<U8, bool, bool> result = sub_u8(cpu.A, u8, 3);
        U8 difference = std::get<0>(result);
        bool is_half_carry = std::get<1>(result);
        bool is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, difference == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, true);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, is_carry);
    }


    int CP_r(Hardware::CPU& cpu, U8 cpu_register) {
        CP_n(cpu, cpu_register);
        return 4;
    }


    int CP_u8(Hardware::CPU& cpu) {
        CP_n(cpu, cpu.mmu.read_u8(cpu.program_counter));
        cpu.program_counter++;
        return 8;
    }


    int CP_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        CP_n(cpu, cpu.mmu.read_u8(hl_pointer));
        return 8;
    }


    int CPL(Hardware::CPU& cpu) {
        cpu.A = ~cpu.A;
        cpu.set_flag(Hardware::CPU::Flag::SUB, true);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, true);
        return 4;
    }


    int DEC_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register) {
        cpu.write_combined_register(combined_register, cpu.read_combined_register(combined_register) - 1);
        return 8;
    }


    int INC_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register) {
        cpu.write_combined_register(combined_register, cpu.read_combined_register(combined_register) + 1);
        return 8;
    }


    int DEC_SP(Hardware::CPU& cpu) {
        cpu.stack_pointer--;
        return 8;
    }


    int INC_SP(Hardware::CPU& cpu) {
        cpu.stack_pointer++;
        return 8;
    }


    void ADD_HL_nn(Hardware::CPU& cpu, U16 u16) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        std::tuple<U16, bool, bool> result = add_u16(hl_pointer, u16, 15, 11);
        U16 sum = std::get<0>(result);
        bool is_half_carry = std::get<1>(result);
        bool is_carry = std::get<2>(result);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, is_half_carry);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, is_carry);
        cpu.write_combined_register(cpu.HL, sum);
    }


    int ADD_HL_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register) {
        ADD_HL_nn(cpu, cpu.read_combined_register(combined_register));
        return 8;
    }


    int ADD_HL_SP(Hardware::CPU& cpu) {
        ADD_HL_nn(cpu, cpu.stack_pointer);
        return 8;
    }


    int ADD_SP_s8(Hardware::CPU& cpu) {
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
        cpu.stack_pointer = sum;
        cpu.program_counter++;
        return 16;
    }

}