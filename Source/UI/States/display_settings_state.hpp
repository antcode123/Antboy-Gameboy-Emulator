#pragma once

#include <unordered_map>
#include "state.hpp"


namespace UI {
    class DisplaySettingsState : public State {
    public:
        std::vector<std::string> target_fps_options;
        std::vector<std::string> display_fps_options;
        std::vector<std::string> scale_factor_options;
        std::vector<std::string> retro_mode_options;
        std::vector<std::string> frame_blend_strength_options;

        std::unordered_map<std::string, int> target_fps_to_value;
        std::unordered_map<std::string, int> scale_factor_to_value;
        std::unordered_map<std::string, int> frame_blend_strength_to_value;

        std::unordered_map<int, std::string> value_to_target_fps;
        std::unordered_map<int, std::string> value_to_scale_factor;
        std::unordered_map<int, std::string> value_to_frame_blend_strength;


        DisplaySettingsState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void perform_logic() override;
        void update_display_settings();
    };
}