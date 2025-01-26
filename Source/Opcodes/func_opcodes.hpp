#pragma once


#include <cstdint>
#include "../Hardware/cpu.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Opcodes {
    int PUSH_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register);
    int POP_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register);
    int CALL_u16(Hardware::CPU& cpu);
    int CALL_cc_u16(Hardware::CPU& cpu, Hardware::CPU::FlagCondition flag_condition);
    int RET(Hardware::CPU& cpu);
    int RET_cc(Hardware::CPU& cpu, Hardware::CPU::FlagCondition flag_condition);
    int RETI(Hardware::CPU& cpu);
    int RST_n(Hardware::CPU& cpu, U16 n);
};