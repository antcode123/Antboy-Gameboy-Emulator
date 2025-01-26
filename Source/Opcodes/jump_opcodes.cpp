
#include "jump_opcodes.hpp"
#include "../Utilities/misc.hpp"
#include "../Hardware/cpu.hpp"
#include "opcodes_helper.hpp"
#include <iostream>


namespace Opcodes {
    int JP_u16(Hardware::CPU& cpu) {
        cpu.program_counter = cpu.mmu.read_u16(cpu.program_counter);
        return 16;
    }


    int JP_HL(Hardware::CPU& cpu) {
        cpu.program_counter = cpu.read_combined_register(cpu.HL);
        return 4;
    }


    int JP_cc_u16(Hardware::CPU& cpu, Hardware::CPU::FlagCondition flag_condition) {
        if (check_flag_condition(cpu, flag_condition)) return JP_u16(cpu);

        else {
            cpu.program_counter += 2;
            return 12;
        }

    }


    int JR_s8(Hardware::CPU& cpu) {
        U8 s8 = cpu.mmu.read_u8(cpu.program_counter);
        cpu.program_counter++;
        std::tuple<U16, bool, bool> result;
        if (Utilities::get_bit_u8(s8, 7)) result = add_u16(cpu.program_counter, (U16)s8 | 0xFF00, 7, 3);
        else result = add_u16(cpu.program_counter, s8, 7, 3);
        cpu.program_counter = std::get<0>(result);
        return 12;
    }


    int JR_cc_s8(Hardware::CPU& cpu, Hardware::CPU::FlagCondition flag_condition) {
        if (check_flag_condition(cpu, flag_condition)) return JR_s8(cpu);

        else {
            cpu.program_counter++;
            return 8;
        }

    }
};