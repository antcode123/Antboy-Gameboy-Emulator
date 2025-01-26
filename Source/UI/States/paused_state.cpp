#include "paused_state.hpp"


namespace UI {
    PausedState::PausedState(Gameboy& _gameboy) :
        State(_gameboy, "PAUSED ") {
        reset();
    }


    void PausedState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 0), ui_element_width, ui_element_height, [&](){return_to_previous_state();}, true, true, "RESUME", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 1), ui_element_width, ui_element_height, [&](){(enter_new_state(SETTINGS));}, true, true, "SETTINGS", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 2), ui_element_width, ui_element_height, [&](){return_to_home();}, true, true, "RETURN TO HOME", gameboy.font));
    }


    void PausedState::return_to_home() {
        gameboy.cartridge.save_ram();
        enter_new_state(MAIN_MENU);
    }
}