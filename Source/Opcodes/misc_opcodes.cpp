#include "misc_opcodes.hpp"
#include "../Hardware/cpu.hpp"


namespace Opcodes {
    int EI(Hardware::CPU& cpu) {
        cpu.can_enable_interrupts = true;
        return 4;
    }


    int DI(Hardware::CPU& cpu) {
        cpu.is_interrupt_master_enabled = false;
        return 4;
    }


    int NOP(Hardware::CPU& cpu) {return 4;}


    int HALT(Hardware::CPU& cpu) {
        cpu.is_halted = true;
        return 4;
    }


    int CCF(Hardware::CPU& cpu) {
        cpu.set_flag(Hardware::CPU::Flag::CARRY, !cpu.get_flag(Hardware::CPU::Flag::CARRY));
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        return 4;
    }


    int SCF(Hardware::CPU& cpu) {
        cpu.set_flag(Hardware::CPU::Flag::CARRY, true);
        cpu.set_flag(Hardware::CPU::Flag::SUB, false);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        return 4;
    }


    //  I received help on the DAA algorithm from this page: https://blog.ollien.com/posts/gb-daa/
    int DAA(Hardware::CPU& cpu) {
        if (cpu.get_flag(Hardware::CPU::Flag::SUB)) {
            if (cpu.get_flag(Hardware::CPU::Flag::CARRY)) cpu.A -= 0x60;
            if (cpu.get_flag(Hardware::CPU::Flag::HALF_CARRY)) cpu.A -= 6;
        }

        else {
            if (cpu.get_flag(Hardware::CPU::Flag::CARRY) || cpu.A > 0x99) {
                cpu.set_flag(Hardware::CPU::Flag::CARRY, true);
                cpu.A += 0x60;
            }

            if (cpu.get_flag(Hardware::CPU::Flag::HALF_CARRY) || (cpu.A & 0xF) > 9) cpu.A += 6;
        }

        cpu.set_flag(Hardware::CPU::Flag::ZERO, cpu.A == 0);
        cpu.set_flag(Hardware::CPU::Flag::HALF_CARRY, false);
        return 4;
    }
}