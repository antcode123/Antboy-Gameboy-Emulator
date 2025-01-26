#pragma once

#include <unordered_map>
#include "state.hpp"


namespace UI {
    class GeneralSettingsState : public State {
    public:
        std::vector<std::string> emulation_speed_options;
        std::vector<std::string> bootstrap_options;
        std::unordered_map<std::string, int> emulation_speed_to_value;
        std::unordered_map<int, std::string> value_to_emulation_speed;

        GeneralSettingsState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void perform_logic() override;
        void update_general_settings();
        void restore_default_settings();
    };
}