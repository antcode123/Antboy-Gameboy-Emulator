#pragma once


#include <cstdint>
#include "../Hardware/cpu.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Opcodes {
    int LD_r_u8(Hardware::CPU& cpu, U8& reg);
    int LD_r_r(Hardware::CPU& cpu, U8& reg1, U8 reg2);
    int LD_r_ptr_HL(Hardware::CPU& cpu, U8& reg);
    int LD_ptr_HL_r(Hardware::CPU& cpu, U8 reg);
    int LD_ptr_HL_u8(Hardware::CPU& cpu);
    int LD_A_ptr_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register);
    int LD_ptr_rr_A(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register);
    int LD_A_ptr_u16(Hardware::CPU& cpu);
    int LD_ptr_u16_A(Hardware::CPU& cpu);
    int LDH_A_ptr_C(Hardware::CPU& cpu);
    int LDH_ptr_C_A(Hardware::CPU& cpu);
    int LDH_A_ptr_u8(Hardware::CPU& cpu);
    int LDH_ptr_u8_A(Hardware::CPU& cpu);
    int LDD_A_ptr_HL(Hardware::CPU& cpu);
    int LDD_ptr_HL_A(Hardware::CPU& cpu);
    int LDI_A_ptr_HL(Hardware::CPU& cpu);
    int LDI_ptr_HL_A(Hardware::CPU& cpu);
    int LD_rr_u16(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register);
    int LD_SP_u16(Hardware::CPU& cpu);
    int LD_ptr_u16_SP(Hardware::CPU& cpu);
    int LD_SP_HL(Hardware::CPU& cpu);
    int LD_HL_SP_s8(Hardware::CPU& cpu);
};