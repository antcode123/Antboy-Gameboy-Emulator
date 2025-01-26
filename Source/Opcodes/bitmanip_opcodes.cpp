
#include "bitmanip_opcodes.hpp"
#include "../Utilities/misc.hpp"
#include "../Hardware/cpu.hpp"
#include "opcodes_helper.hpp"


namespace Opcodes {
    void BIT_b_n(Hardware::CPU& cpu, int b, U8 u8) {
        cpu.set_flag(Hardware::CPU::Flag::ZERO, (u8 & (1 << b)) == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, true);
    }


    int BIT_b_r(Hardware::CPU& cpu, int b, U8 reg) {
        BIT_b_n(cpu, b, reg);
        return 8;
    }


    int BIT_b_ptr_HL(Hardware::CPU& cpu, int b) {
        BIT_b_n(cpu, b, cpu.mmu.read_u8(cpu.read_combined_register(cpu.HL)));
        return 12;
    }


    int SET_b_r(Hardware::CPU& cpu, int b, U8& reg) {
        reg |= (1 << b);
        return 8;
    }


    int SET_b_ptr_HL(Hardware::CPU& cpu, int b) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer) | (1 << b);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 16;
    }


    int RES_b_r(Hardware::CPU& cpu, int b, U8& reg) {
        reg &= ~(1 << b);
        return 8;
    }


    int RES_b_ptr_HL(Hardware::CPU& cpu, int b) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer) & ~(1 << b);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 16;
    }


    void SWAP_n(Hardware::CPU& cpu, U8& u8) {
        u8  = ((u8 & 0xF) << 4) | ((u8 & 0xF0) >> 4);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, u8 == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, false);
    }


    int SWAP_r(Hardware::CPU& cpu, U8& reg) {
        SWAP_n(cpu, reg);
        return 8;
    }


    int SWAP_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer);
        SWAP_n(cpu, u8);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 16;
    }


    void RL_n(Hardware::CPU& cpu, U8& u8) {
        bool mostSigBit = Utilities::get_bit_u8(u8, 7);;
        u8 = (u8 << 1) | cpu.get_flag(Hardware::CPU::Flag::CARRY);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, u8 == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, mostSigBit);
    }


    int RL_r(Hardware::CPU& cpu, U8& reg) {
        RL_n(cpu, reg);
        return 8;
    }


    int RL_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer);
        RL_n(cpu, u8);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 16;
    }


    void RLC_n(Hardware::CPU& cpu, U8& u8) {
        U8 mostSigBit = u8 >> 7;
        u8 = (u8 << 1) | mostSigBit;
        cpu.set_flag(Hardware::CPU::Flag::ZERO, u8 == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, (bool)mostSigBit);
    }


    int RLC_r(Hardware::CPU& cpu, U8& reg) {
        RLC_n(cpu, reg);
        return 8;
    }


    int RLC_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer);
        RLC_n(cpu, u8);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 16;
    }


    void RR_n(Hardware::CPU& cpu, U8& u8) {
        bool leastSigBit = Utilities::get_bit_u8(u8, 0);
        u8 = (u8 >> 1) | ((U8)cpu.get_flag(Hardware::CPU::Flag::CARRY) << 7);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, u8 == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, leastSigBit);
    }


    int RR_r(Hardware::CPU& cpu, U8& reg) {
        RR_n(cpu, reg);
        return 8;
    }


    int RR_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer);
        RR_n(cpu, u8);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 16;
    }


    void RRC_n(Hardware::CPU& cpu, U8& u8) {
        U8 leastSigBit = u8 & 1;
        u8 = (u8 >> 1) | (leastSigBit << 7);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, u8 == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, (bool)leastSigBit);
    }


    int RRC_r(Hardware::CPU& cpu, U8& reg) {
        RRC_n(cpu, reg);
        return 8;
    }


    int RRC_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer);
        RRC_n(cpu, u8);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 16;
    }


    int RLA(Hardware::CPU& cpu) {
        bool mostSigBit = Utilities::get_bit_u8(cpu.A, 7);
        cpu.A = (cpu.A << 1) | cpu.get_flag(Hardware::CPU::Flag::CARRY);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, false);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, mostSigBit);
        return 4;
    }


    int RLCA(Hardware::CPU& cpu) {
        U8 mostSigBit = cpu.A >> 7;
        cpu.A = (cpu.A << 1) | mostSigBit;
        cpu.set_flag(Hardware::CPU::Flag::ZERO, false);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, (bool)mostSigBit);
        return 4;
    }


    int RRA(Hardware::CPU& cpu) {
        bool leastSigBit = Utilities::get_bit_u8(cpu.A, 0);
        cpu.A = (cpu.A >> 1) | ((U8)cpu.get_flag(Hardware::CPU::Flag::CARRY)) << 7;
        cpu.set_flag(Hardware::CPU::Flag::ZERO, false);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, leastSigBit);
        return 4;
    }


    int RRCA(Hardware::CPU& cpu) {
        U8 leastSigBit = cpu.A & 1;
        cpu.A = (cpu.A >> 1) | (leastSigBit << 7);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, false);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, (bool)leastSigBit);
        return 4;
    }


    void SLA_n(Hardware::CPU& cpu, U8& u8) {
        bool mostSigBit = Utilities::get_bit_u8(u8, 7);;
        u8 <<= 1;
        cpu.set_flag(Hardware::CPU::Flag::ZERO, u8 == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, mostSigBit);
    }


    int SLA_r(Hardware::CPU& cpu, U8& reg) {
        SLA_n(cpu, reg);
        return 8;
    }


    int SLA_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer);
        SLA_n(cpu, u8);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 16;
    }


    void SRA_n(Hardware::CPU& cpu, U8& u8) {
        U8 mostSigBit = u8 >> 7;
        bool leastSigBit = Utilities::get_bit_u8(u8, 0);
        u8 = (u8 >> 1) | (mostSigBit << 7);
        cpu.set_flag(Hardware::CPU::Flag::ZERO, u8 == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, leastSigBit);
    }


    int SRA_r(Hardware::CPU& cpu, U8& reg) {
        SRA_n(cpu, reg);
        return 8;
    }


    int SRA_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer);
        SRA_n(cpu, u8);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 16;
    }


    void SRL_n(Hardware::CPU& cpu, U8& u8) {
        bool leastSigBit = Utilities::get_bit_u8(u8, 0);
        u8 >>= 1;
        cpu.set_flag(Hardware::CPU::Flag::ZERO, u8 == 0);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        cpu.set_flag(Hardware::CPU::Flag::CARRY, leastSigBit);
    }


    int SRL_r(Hardware::CPU& cpu, U8& reg) {
        SRL_n(cpu, reg);
        return 8;
    }


    int SRL_ptr_HL(Hardware::CPU& cpu) {
        U16 hl_pointer = cpu.read_combined_register(cpu.HL);
        U8 u8 = cpu.mmu.read_u8(hl_pointer);
        SRL_n(cpu, u8);
        cpu.mmu.write_u8(hl_pointer, u8);
        return 16;
    }
}