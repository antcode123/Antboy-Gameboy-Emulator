#pragma once


#include <SFML/Graphics.hpp>
#include <json.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>


typedef unsigned char U8;
typedef unsigned short U16;


namespace Hardware {
    class CPU;


    class Joypad {
    public:
        CPU& cpu;
        U8 joypad;
        U8 joypad_button_states;

        Joypad(CPU& _interupt_controller);
        void reset();
        U8 read();
        void press_button(int button_bit);
        void check_key_pressed(sf::Event& event, nlohmann::json key_binds);
        void check_key_released(sf::Event& event, nlohmann::json key_binds);
        void check_controller_button_pressed(sf::Event& event, nlohmann::json controller_binds);
        void check_controller_button_released(sf::Event& event, nlohmann::json controller_binds);
        void check_controller_dpad_pressed(sf::Event& event, nlohmann::json controller_binds);
    };
}
