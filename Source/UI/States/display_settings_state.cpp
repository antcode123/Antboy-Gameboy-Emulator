#include "display_settings_state.hpp"


namespace UI {
    DisplaySettingsState::DisplaySettingsState(Gameboy& _gameboy) :
        State(_gameboy, "DISPLAY SETTINGS") {
        target_fps_options = {"30", "60", "120", "144", "165", "240", "360", "UNLIMITED"};
        display_fps_options = {"ON", "OFF"};
        scale_factor_options = {"X3", "X4", "X5", "X6", "FULLSCREEN"};
        retro_mode_options = {"ON", "OFF"};
        frame_blend_strength_options = {"WEAK", "MEDIUM", "STRONG"};
        target_fps_to_value["30"] = 30;
        target_fps_to_value["60"] = 60;
        target_fps_to_value["120"] = 120;
        target_fps_to_value["144"] = 144;
        target_fps_to_value["165"] = 165;
        target_fps_to_value["240"] = 240;
        target_fps_to_value["360"] = 360;
        target_fps_to_value["UNLIMITED"] = 1000;
        scale_factor_to_value["X3"] = 3;
        scale_factor_to_value["X4"] = 4;
        scale_factor_to_value["X5"] = 5;
        scale_factor_to_value["X6"] = 6;
        scale_factor_to_value["FULLSCREEN"] = gameboy.lcd.full_screen_scale_factor;
        frame_blend_strength_to_value["WEAK"] = 0;
        frame_blend_strength_to_value["MEDIUM"] = 1;
        frame_blend_strength_to_value["STRONG"] = 2;
        value_to_target_fps[30] = "30";
        value_to_target_fps[60] = "60";
        value_to_target_fps[120] = "120";
        value_to_target_fps[144] = "144";
        value_to_target_fps[165] = "165";
        value_to_target_fps[240] = "240";
        value_to_target_fps[360] = "360";
        value_to_target_fps[1000] = "UNLIMITED";
        value_to_scale_factor[3] = "X3";
        value_to_scale_factor[4] = "X4";
        value_to_scale_factor[5] = "X5";
        value_to_scale_factor[6] = "X6";
        value_to_scale_factor[gameboy.lcd.full_screen_scale_factor] = "FULLSCREEN";
        value_to_frame_blend_strength[0] = "WEAK";
        value_to_frame_blend_strength[1] = "MEDIUM";
        value_to_frame_blend_strength[2] = "STRONG";

        reset();
    }


    void DisplaySettingsState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();
        ui_elements.push_back(std::make_unique<ArrowSelector>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 0), true, "TARGET FPS", target_fps_options, value_to_target_fps[gameboy.target_fps], gameboy.font));
        ui_elements.push_back(std::make_unique<ArrowSelector>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 1), true, "DISPLAY FPS", display_fps_options, gameboy.is_display_fps_enabled ? "ON" : "OFF", gameboy.font));
        ui_elements.push_back(std::make_unique<ArrowSelector>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 2), true, "SCALE FACTOR", scale_factor_options, value_to_scale_factor[gameboy.lcd.scale_factor], gameboy.font));
        ui_elements.push_back(std::make_unique<ArrowSelector>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 3), true, "RETRO MODE", retro_mode_options, gameboy.lcd.is_retro_mode_enabled ? "ON" : "OFF", gameboy.font));
        ui_elements.push_back(std::make_unique<ArrowSelector>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 4), true, "FRAME BLEND STRENGTH", frame_blend_strength_options, value_to_frame_blend_strength[gameboy.lcd.frame_blend_strength], gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 5), ui_element_width, ui_element_height, [&](){return_to_previous_state();}, true, true, "<- BACK", gameboy.font));
    }


    void DisplaySettingsState::perform_logic() {
        animate_ui_elements();
        update_display_settings();
    }


    void DisplaySettingsState::update_display_settings() {
        ArrowSelector* target_fps_arrow_selector = (ArrowSelector*)ui_elements[0].get();
        ArrowSelector* display_fps_arrow_selector = (ArrowSelector*)ui_elements[1].get();
        ArrowSelector* scale_factor_arrow_selector = (ArrowSelector*)ui_elements[2].get();
        ArrowSelector* retro_mode_arrow_selector = (ArrowSelector*)ui_elements[3].get();
        ArrowSelector* frame_blend_strength_arrow_selector = (ArrowSelector*)ui_elements[4].get();
        int previous_scale_factor = gameboy.lcd.scale_factor;
        gameboy.target_fps = target_fps_to_value[target_fps_arrow_selector->current_selection];
        gameboy.is_display_fps_enabled = display_fps_arrow_selector->current_selection == "ON" ? true : false;
        gameboy.lcd.scale_factor = scale_factor_to_value[scale_factor_arrow_selector->current_selection];
        gameboy.lcd.is_retro_mode_enabled = retro_mode_arrow_selector->current_selection == "ON" ? true : false;
        gameboy.lcd.frame_blend_strength = frame_blend_strength_to_value[frame_blend_strength_arrow_selector->current_selection];

        if (previous_scale_factor != gameboy.lcd.scale_factor) {
            gameboy.resize_window();
            configure_ui_elements();
            update_cursor_after_ui_element_reconfiguration();
        }
    }

}