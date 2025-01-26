#pragma once


#include <cstdint>
#include "../Hardware/cpu.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Opcodes {
    int EI(Hardware::CPU& cpu);
    int DI(Hardware::CPU& cpu);
    int NOP(Hardware::CPU& cpu);
    int HALT(Hardware::CPU& cpu);
    int CCF(Hardware::CPU& cpu);
    int SCF(Hardware::CPU& cpu);
    int DAA(Hardware::CPU& cpu);
};