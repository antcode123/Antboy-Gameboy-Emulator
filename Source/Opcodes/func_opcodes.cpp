
#include "func_opcodes.hpp"
#include "../Hardware/cpu.hpp"
#include "opcodes_helper.hpp"


namespace Opcodes {
    int PUSH_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register) {
        cpu.push_onto_stack(cpu.read_combined_register(combined_register));
        return 16;
    }


    int POP_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register) {;
        cpu.write_combined_register(combined_register, cpu.pop_off_stack());
        return 12;
    }


    int CALL_u16(Hardware::CPU& cpu) {
        cpu.push_onto_stack(cpu.program_counter + 2);
        cpu.program_counter = cpu.mmu.read_u16(cpu.program_counter);
        return 24;
    }


    int CALL_cc_u16(Hardware::CPU& cpu, Hardware::CPU::FlagCondition flag_condition) {
        if (check_flag_condition(cpu, flag_condition)) return CALL_u16(cpu);

        else {
            cpu.program_counter += 2;
            return 12;
        }

    }


    int RET(Hardware::CPU& cpu) {
        cpu.program_counter = cpu.pop_off_stack();
        return 16;
    }


    int RET_cc(Hardware::CPU& cpu, Hardware::CPU::FlagCondition flag_condition) {
        if (check_flag_condition(cpu, flag_condition)) {
            RET(cpu);
            return 20;
        }

        return 8;

    }


    int RETI(Hardware::CPU& cpu) {
        cpu.is_interrupt_master_enabled = true;
        cpu.program_counter = cpu.pop_off_stack();
        return 16;
    }


    int RST_n(Hardware::CPU& cpu, U16 n) {
        cpu.push_onto_stack(cpu.program_counter);
        cpu.program_counter = n * 8;
        return 16;
    }
}