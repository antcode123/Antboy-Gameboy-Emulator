#include "settings_state.hpp"


namespace UI {
    SettingsState::SettingsState(Gameboy& _gameboy) :
        State(_gameboy, "SETTINGS") {
        reset();
    }


    void SettingsState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 0), ui_element_width, ui_element_height, [&](){enter_new_state(GENERAL_SETTINGS);}, true, true, "GENERAL", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 1), ui_element_width, ui_element_height, [&](){enter_new_state(DISPLAY_SETTINGS);}, true, true, "DISPLAY", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 2), ui_element_width, ui_element_height, [&](){enter_new_state(PALETTES);}, true, true, "PALETTES", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 3), ui_element_width, ui_element_height, [&](){enter_new_state(CONTROLS);}, true, true, "CONTROLS", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 4), ui_element_width, ui_element_height, [&](){return_to_previous_state();}, true, true, "<- BACK", gameboy.font));
    }
}