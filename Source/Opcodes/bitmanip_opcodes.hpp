#pragma once

#include <cstdint>
#include <tuple>
#include "../Hardware/cpu.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Opcodes {
    void BIT_b_n(Hardware::CPU& cpu, int b, U8 u8);
    int BIT_b_r(Hardware::CPU& cpu, int b, U8 reg);
    int BIT_b_ptr_HL(Hardware::CPU& cpu, int b);
    int SET_b_r(Hardware::CPU& cpu, int b, U8& reg);
    int SET_b_ptr_HL(Hardware::CPU& cpu, int b);
    int RES_b_r(Hardware::CPU& cpu, int b, U8& reg);
    int RES_b_ptr_HL(Hardware::CPU& cpu, int b);
    void SWAP_n(Hardware::CPU& cpu, U8& u8);
    int SWAP_r(Hardware::CPU& cpu, U8& reg);
    int SWAP_ptr_HL(Hardware::CPU& cpu);
    void RL_n(Hardware::CPU& cpu, U8& u8);
    int RL_r(Hardware::CPU& cpu, U8& reg);
    int RL_ptr_HL(Hardware::CPU& cpu);
    void RLC_n(Hardware::CPU& cpu, U8& u8);
    int RLC_r(Hardware::CPU& cpu, U8& reg);
    int RLC_ptr_HL(Hardware::CPU& cpu);
    void RR_n(Hardware::CPU& cpu, U8& u8);
    int RR_r(Hardware::CPU& cpu, U8& reg);
    int RR_ptr_HL(Hardware::CPU& cpu);
    void RRC_n(Hardware::CPU& cpu, U8& u8);
    int RRC_r(Hardware::CPU& cpu, U8& reg);
    int RRC_ptr_HL(Hardware::CPU& cpu);
    int RLA(Hardware::CPU& cpu);
    int RLCA(Hardware::CPU& cpu);
    int RRA(Hardware::CPU& cpu);
    int RRCA(Hardware::CPU& cpu);
    void SLA_n(Hardware::CPU& cpu, U8& u8);
    int SLA_r(Hardware::CPU& cpu, U8& reg);
    int SLA_ptr_HL(Hardware::CPU& cpu);
    void SRA_n(Hardware::CPU& cpu, U8& u8);
    int SRA_r(Hardware::CPU& cpu, U8& reg);
    int SRA_ptr_HL(Hardware::CPU& cpu);
    void SRL_n(Hardware::CPU& cpu, U8& u8);
    int SRL_r(Hardware::CPU& cpu, U8& reg);
    int SRL_ptr_HL(Hardware::CPU& cpu);
};