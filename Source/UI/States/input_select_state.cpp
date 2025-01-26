#include "input_select_state.hpp"


namespace UI {
    InputSelectState::InputSelectState(Gameboy& _gameboy) :
        State(_gameboy, "INPUT SELECT") {
        reset();
    }


    void InputSelectState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 0), ui_element_width, ui_element_height, [&](){enter_remap_binds_state("KEYBOARD");}, true, true, "KEYBOARD", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 1), ui_element_width, ui_element_height, [&](){enter_remap_binds_state("CONTROLLER");}, true, true, "CONTROLLER", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 2), ui_element_width, ui_element_height, [&](){return_to_previous_state();}, true, true, "<- BACK", gameboy.font));
    }


    void InputSelectState::enter_remap_binds_state(std::string input_type) {
        gameboy.modifying_input_type = input_type;
        enter_new_state(REMAP_BINDS);
    }
}