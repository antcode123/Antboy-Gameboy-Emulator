#include "timer.hpp"
#include "cpu.hpp"
#include "mmu.hpp"
#include "../Utilities/misc.hpp"


namespace Hardware {

    // Handles the Gameboy's timers - the divider and counter.
    Timer::Timer(MMU& _mmu, CPU& _cpu) :
        mmu(_mmu),
        cpu(_cpu),
        divider_period(256) {
        reset();
    }


    // Resets the timers
    void Timer::reset() {
        divider = 0;
        counter = 0;
        modulo = 0;
        clock_select = 0;
        is_counter_enabled = false;
        counter_period = 16384;
        divider_ticks = 0;
        counter_ticks = 0;
    }


    // Reads from a timer register
    U8 Timer::read(U16 address) {
        switch (address) {
            case 0xFF04: return divider;
            case 0xFF05: return counter;
            case 0xFF06: return modulo;
            case 0xFF07: return (U8)(is_counter_enabled) << 2 | clock_select;
            default: return 0xFF;
        }
    }


    void Timer::write(U16 address, U8 u8) {
        switch (address) {
            case 0xFF04: // Writes to the divider resets both the divider and counter timers
                divider = 0;
                counter = 0;
                divider_ticks = 0;
                counter_ticks = 0;
                break;

            case 0xFF05: counter = u8; break;
            case 0xFF06: modulo = u8; break;

            case 0xFF07:
                is_counter_enabled = Utilities::get_bit_u8(u8, 2);
                clock_select = u8 & 0b00000011;
                set_counter_period();
                break;
        }
    }


    void Timer::set_counter_period() {
        switch (clock_select) {
            case 0: counter_period = 1024; break;
            case 1: counter_period = 16; break;
            case 2: counter_period = 64; break;
            case 3: counter_period = 256; break;
        }
    }


    void Timer::run(int last_instruction_ticks) {
        divider_ticks += last_instruction_ticks;
        counter_ticks += last_instruction_ticks;
        update_divider();
        update_counter();
    }


    void Timer::update_divider() {
        if (divider_ticks < divider_period) return;

        divider_ticks -= divider_period;
        divider++;
    }


    void Timer::update_counter() {
        if (!is_counter_enabled) return;

        // Iteratively increments the counter until the counter ticks subceeds the counter period
        // A time where this would occur would be when the cpu has excuted an instruction which has taken many ticks
        // This could be large enough that two or more counter incrememets could occurred
        while (counter_ticks >= counter_period) {
            counter_ticks -= counter_period;
            counter++;

            if (counter == 0) {
                counter = modulo;
                cpu.set_interrupt(2, true);
            }
        }
    }
}