#include "joypad.hpp"
#include "cpu.hpp"
#include "../Utilities/misc.hpp"


namespace Hardware {

    // Stores the Joypad and handles key/button to joypad mappings
    Joypad::Joypad(CPU& _cpu) : cpu(_cpu) {}


    void Joypad::reset() {
        joypad = 0;
        joypad_button_states = 0xFF;
    }


    U8 Joypad::read() {
        if (!Utilities::get_bit_u8(joypad, 5)) return joypad | (joypad_button_states & 0xF);  // Action buttons
        return joypad | (joypad_button_states >> 4); // Directional buttons
    }


    void Joypad::press_button(int button_bit) {
        Utilities::set_bit_u8(joypad_button_states, button_bit, false);
        cpu.set_interrupt(4, true);
    }


    void Joypad::check_key_pressed(sf::Event& event, nlohmann::json key_binds) {
        if (event.key.code == key_binds["DOWN"]) press_button(7);
        else if (event.key.code == key_binds["UP"]) press_button(6);
        else if (event.key.code == key_binds["LEFT"]) press_button(5);
        else if (event.key.code == key_binds["RIGHT"]) press_button(4);
        else if (event.key.code == key_binds["START"]) press_button(3);
        else if (event.key.code == key_binds["SELECT"]) press_button(2);
        else if (event.key.code == key_binds["B"]) press_button(1);
        else if (event.key.code == key_binds["A"]) press_button(0);
    }


    void Joypad::check_key_released(sf::Event& event, nlohmann::json key_binds) {
        if (event.key.code == key_binds["DOWN"]) Utilities::set_bit_u8(joypad_button_states, 7, true);
        else if (event.key.code == key_binds["UP"]) Utilities::set_bit_u8(joypad_button_states, 6, true);
        else if (event.key.code == key_binds["LEFT"]) Utilities::set_bit_u8(joypad_button_states, 5, true);
        else if (event.key.code == key_binds["RIGHT"]) Utilities::set_bit_u8(joypad_button_states, 4, true);
        else if (event.key.code == key_binds["START"]) Utilities::set_bit_u8(joypad_button_states, 3, true);
        else if (event.key.code == key_binds["SELECT"]) Utilities::set_bit_u8(joypad_button_states, 2, true);
        else if (event.key.code == key_binds["B"]) Utilities::set_bit_u8(joypad_button_states, 1, true);
        else if (event.key.code == key_binds["A"]) Utilities::set_bit_u8(joypad_button_states, 0, true);
    }


    void Joypad::check_controller_button_pressed(sf::Event& event, nlohmann::json controller_binds) {
        if (event.joystickButton.button == controller_binds["START"]) press_button(3);
        else if (event.joystickButton.button == controller_binds["SELECT"]) press_button(2);
        else if (event.joystickButton.button == controller_binds["B"]) press_button(1);
        else if (event.joystickButton.button == controller_binds["A"]) press_button(0);
    }


    void Joypad::check_controller_button_released(sf::Event& event, nlohmann::json controller_binds) {
        if (event.joystickButton.button == controller_binds["START"]) Utilities::set_bit_u8(joypad_button_states, 3, true);
        else if (event.joystickButton.button == controller_binds["SELECT"]) Utilities::set_bit_u8(joypad_button_states, 2, true);
        else if (event.joystickButton.button == controller_binds["B"]) Utilities::set_bit_u8(joypad_button_states, 1, true);
        else if (event.joystickButton.button == controller_binds["A"]) Utilities::set_bit_u8(joypad_button_states, 0, true);
    }


    void Joypad::check_controller_dpad_pressed(sf::Event& event, nlohmann::json controller_binds) {
        if (event.joystickMove.axis == sf::Joystick::PovX) {
            Utilities::set_bit_u8(joypad_button_states, 4, true);
            Utilities::set_bit_u8(joypad_button_states, 5, true);
            if (event.joystickMove.position > 25) press_button(4);
            else if (event.joystickMove.position < -25) press_button(5);
        }

        else if (event.joystickMove.axis == sf::Joystick::PovY) {
            Utilities::set_bit_u8(joypad_button_states, 6, true);
            Utilities::set_bit_u8(joypad_button_states, 7, true);
            if (event.joystickMove.position > 25) press_button(6);
            else if (event.joystickMove.position < -25) press_button(7);
        }
    }
}
