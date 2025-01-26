#include <tuple>
#include "opcodes_helper.hpp"
#include "../Hardware/cpu.hpp"


namespace Opcodes {
    std::tuple<U8, bool, bool> add_u8(U8 a, U8 b, int carry_bit, int half_carry_bit) {
        U8 sum = a + b;
        bool is_half_carry = check_carry_u8(a, b, half_carry_bit);
        bool is_carry = check_carry_u8(a, b, carry_bit);
        return std::make_tuple(sum, is_half_carry, is_carry);
    }


    std::tuple<U8, bool, bool> sub_u8(U8 a, U8 b, int half_borrow_bit) {
        U8 difference = a - b;
        bool is_half_borrow = check_borrow_u8(a, b, half_borrow_bit);
        bool is_borrow = check_borrow_u8(a, b, 7);
        return std::make_tuple(difference, is_half_borrow, is_borrow);
    }


    std::tuple<U16, bool, bool> add_u16(U16 a, U16 b, int carry_bit, int half_carry_bit) {
        U16 sum = a + b;
        bool is_half_carry = check_carry_u16(a, b, half_carry_bit);
        bool is_carry = check_carry_u16(a, b, carry_bit);
        return std::make_tuple(sum, is_half_carry, is_carry);
    }


    bool check_carry_u8(U8 a, U8 b, int carry_bit) {
        U8 mask = 1;
        for (int i = 0; i < carry_bit; i++) mask = (mask << 1) | 1;
        return ((a & mask) + (b & mask) > mask) ? true : false;
    }


    bool check_borrow_u8(U8 a, U8 b, int borrow_bit) {
        U8 mask = 1;
        for (int i = 0; i < borrow_bit; i++) mask = (mask << 1) | 1;
        return ((a & mask) < (b & mask)) ? true : false;
    }


    bool check_carry_u16(U16 a, U16 b, int carry_bit) {
        U16 mask = 1;
        for (int i = 0; i < carry_bit; i++) mask = (mask << 1) | 1;
        return ((a & mask) + (b & mask) > mask) ? true : false;
    }


    bool check_flag_condition(Hardware::CPU& cpu, Hardware::CPU::FlagCondition flag_condition) {
        switch (flag_condition) {
            case Hardware::CPU::FlagCondition::IS_ZERO: return cpu.get_flag(Hardware::CPU::Flag::ZERO);
            case Hardware::CPU::FlagCondition::IS_NOT_ZERO: return !cpu.get_flag(Hardware::CPU::Flag::ZERO);
            case Hardware::CPU::FlagCondition::IS_CARRY: return cpu.get_flag(Hardware::CPU::Flag::CARRY);
            case Hardware::CPU::FlagCondition::IS_NOT_CARRY: return !cpu.get_flag(Hardware::CPU::Flag::CARRY);
            default: return 0;
        }
    }
}