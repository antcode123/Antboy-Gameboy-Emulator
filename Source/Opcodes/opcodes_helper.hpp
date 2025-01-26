#pragma once

#include <cstdint>
#include <tuple>
#include "../Hardware/cpu.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Opcodes {
    std::tuple<U8, bool, bool> add_u8(U8 a, U8 b, int carry_bit, int half_carry_bit);
    std::tuple<U8, bool, bool> sub_u8(U8 a, U8 b, int half_borrow_bit);
    std::tuple<U16, bool, bool> add_u16(U16 a, U16 b, int carry_bit, int half_carry_bit);
    bool check_carry_u8(U8 a, U8 b, int carry_bit);
    bool check_borrow_u8(U8 a, U8 b, int borrow_bit);
    bool check_carry_u16(U16 a, U16 b, int carry_bit);
    bool check_flag_condition(Hardware::CPU& cpu, Hardware::CPU::FlagCondition flag_condition);
};