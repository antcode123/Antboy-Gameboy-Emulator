#include "controls_state.hpp"


namespace UI {
    ControlsState::ControlsState(Gameboy& _gameboy) :
        State(_gameboy, "CONTROLS") {
        reset();
    }


    void ControlsState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 0), ui_element_width, ui_element_height, [&](){enter_input_select_state("SYSTEM");}, true, true, "SYSTEM", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 1), ui_element_width, ui_element_height, [&](){enter_input_select_state("GAME");}, true, true, "GAME", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 2), ui_element_width, ui_element_height, [&](){return_to_previous_state();}, true, true, "<- BACK", gameboy.font));
    }


    void ControlsState::enter_input_select_state(std::string control_type) {
        gameboy.modifying_control_type = control_type;
        enter_new_state(INPUT_SELECT);
    }
}