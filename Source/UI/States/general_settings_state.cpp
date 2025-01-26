#include "general_settings_state.hpp"


namespace UI {
    GeneralSettingsState::GeneralSettingsState(Gameboy& _gameboy) :
        State(_gameboy, "GENERAL SETTINGS") {
        emulation_speed_options = {"50%", "100%", "150%", "200%"};
        bootstrap_options = {"ON", "OFF"};
        emulation_speed_to_value["50%"] = 50;
        emulation_speed_to_value["100%"] = 100;
        emulation_speed_to_value["150%"] = 150;
        emulation_speed_to_value["200%"] = 200;
        value_to_emulation_speed[50] = "50%";
        value_to_emulation_speed[100] = "100%";
        value_to_emulation_speed[150] = "150%";
        value_to_emulation_speed[200] = "200%";
        reset();
    }


    void GeneralSettingsState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();
        ui_elements.push_back(std::make_unique<ArrowSelector>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 0), true, "EMULATION SPEED", emulation_speed_options, value_to_emulation_speed[gameboy.emulation_speed], gameboy.font));
        ui_elements.push_back(std::make_unique<ArrowSelector>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 1), true, "BOOTSTRAP", bootstrap_options, gameboy.is_bootstrap_enabled ? "ON" : "OFF", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 2), ui_element_width, ui_element_height, [&](){restore_default_settings();}, true, true, "RESTORE DEFAULTS", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 3), ui_element_width, ui_element_height, [&](){return_to_previous_state();}, true, true, "<- BACK", gameboy.font));
    }


    void GeneralSettingsState::perform_logic() {
        animate_ui_elements();
        update_general_settings();
    }


    void GeneralSettingsState::update_general_settings() {
        ArrowSelector* emulation_speed_arrow_selector = (ArrowSelector*)ui_elements[0].get();
        ArrowSelector* bootstrap_arrow_selector = (ArrowSelector*)ui_elements[1].get();
        gameboy.emulation_speed = emulation_speed_to_value[emulation_speed_arrow_selector->current_selection];
        gameboy.is_bootstrap_enabled = bootstrap_arrow_selector->current_selection == "ON" ? true : false;
    }


    void GeneralSettingsState::restore_default_settings() {
        gameboy.restore_default_settings();
        screen_fade_color = (*gameboy.selected_palette)[3];
        configure_ui_elements();
    }
}