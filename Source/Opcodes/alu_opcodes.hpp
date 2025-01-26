#pragma once


#include <cstdint>
#include "../Hardware/cpu.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Opcodes {
    void ADD_n(Hardware::CPU& cpu, U8 u8);
    int ADD_r(Hardware::CPU& cpu, U8 cpu_register);
    int ADD_u8(Hardware::CPU& cpu);
    int ADD_ptr_HL(Hardware::CPU& cpu);
    void ADC_n(Hardware::CPU& cpu, U8 u8);
    int ADC_r(Hardware::CPU& cpu, U8 cpu_register);
    int ADC_u8(Hardware::CPU& cpu);
    int ADC_ptr_HL(Hardware::CPU& cpu);
    void SUB_n(Hardware::CPU& cpu, U8 u8);
    int SUB_r(Hardware::CPU& cpu, U8 cpu_register);
    int SUB_u8(Hardware::CPU& cpu);
    int SUB_ptr_HL(Hardware::CPU& cpu);
    void SBC_n(Hardware::CPU& cpu, U8 u8);
    int SBC_r(Hardware::CPU& cpu, U8 cpu_register);
    int SBC_u8(Hardware::CPU& cpu);
    int SBC_ptr_HL(Hardware::CPU& cpu);
    void INC_n(Hardware::CPU& cpu, U8& u8);
    int INC_r(Hardware::CPU& cpu, U8& cpu_register);
    int INC_ptr_HL(Hardware::CPU& cpu);
    void DEC_n(Hardware::CPU& cpu, U8& u8);
    int DEC_r(Hardware::CPU& cpu, U8& cpu_register);
    int DEC_ptr_HL(Hardware::CPU& cpu);
    void AND_n(Hardware::CPU& cpu, U8 u8);
    int AND_r(Hardware::CPU& cpu, U8 cpu_register);
    int AND_u8(Hardware::CPU& cpu);
    int AND_ptr_HL(Hardware::CPU& cpu);
    void OR_n(Hardware::CPU& cpu, U8 u8);
    int OR_r(Hardware::CPU& cpu, U8 cpu_register);
    int OR_u8(Hardware::CPU& cpu);
    int OR_ptr_HL(Hardware::CPU& cpu);
    void XOR_n(Hardware::CPU& cpu, U8 u8);
    int XOR_r(Hardware::CPU& cpu, U8 cpu_register);
    int XOR_u8(Hardware::CPU& cpu);
    int XOR_ptr_HL(Hardware::CPU& cpu);
    void CP_n(Hardware::CPU& cpu, U8 u8);
    int CP_r(Hardware::CPU& cpu, U8 cpu_register);
    int CP_u8(Hardware::CPU& cpu);
    int CP_ptr_HL(Hardware::CPU& cpu);
    int CPL(Hardware::CPU& cpu);
    int DEC_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register);
    int INC_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register);
    int DEC_SP(Hardware::CPU& cpu);
    int INC_SP(Hardware::CPU& cpu);
    void ADD_HL_nn(Hardware::CPU& cpu, U16 u16);
    int ADD_HL_rr(Hardware::CPU& cpu, Hardware::CPU::CombinedRegister combined_register);
    int ADD_HL_SP(Hardware::CPU& cpu);
    int ADD_SP_s8(Hardware::CPU& cpu);
};