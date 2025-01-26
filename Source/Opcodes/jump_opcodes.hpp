#pragma once


#include <cstdint>
#include "../Hardware/cpu.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Opcodes {
    int JP_u16(Hardware::CPU& cpu);
    int JP_HL(Hardware::CPU& cpu);
    int JP_cc_u16(Hardware::CPU& cpu, Hardware::CPU::FlagCondition flag_condition);
    int JR_s8(Hardware::CPU& cpu);
    int JR_cc_s8(Hardware::CPU& cpu, Hardware::CPU::FlagCondition flag_condition);
};