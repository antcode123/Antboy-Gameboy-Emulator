#include "modify_palette_state.hpp"


namespace UI {
    ModifyPaletteState::ModifyPaletteState(Gameboy& _gameboy) :
        State(_gameboy, "MODIFY PALETTE") {
        reset();
    }


    void ModifyPaletteState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();

        for (int i = 0; i < 4; i++) {
            ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * i), ui_element_width, ui_element_height, [&](){enter_color_picker_state();}, true, true, "MODIFY", gameboy.font));
        }

        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(ui_element_width * 1.5, length_between_ui_elements * 4), ui_element_width * 4, ui_element_height * 8 / 9, [&](){return_to_previous_state();}, true, true, "<- BACK", gameboy.font));
    }


    void ModifyPaletteState::configure_ui_element_parameters() {
        length_between_ui_elements = 20;
        ui_element_width = 134 * 0.25;
        ui_element_offset = gameboy.lcd.position + Utilities::Vector(gameboy.lcd.width * 0.5 - ui_element_width * 1.5, 44);
        ui_element_height = 8;
    }


    void ModifyPaletteState::display_miscellaneous() {display_palette();}


    void ModifyPaletteState::display_palette() {
        float segment_width = gameboy.lcd.scale_factor * 100;
        float segment_height = gameboy.lcd.scale_factor * ui_element_height * 1.6;

        for (int i = 0; i < 4; i++) {
            sf::Color outline_color = ui_elements[i]->is_selected ? (*gameboy.selected_palette)[2] : (*gameboy.selected_palette)[1];
            Utilities::Vector segment_position = (ui_element_offset + Utilities::Vector(70, length_between_ui_elements * i)) * gameboy.lcd.scale_factor;
            gameboy.renderer.draw_rounded_rectangle(gameboy.window, segment_position, segment_width, segment_height, segment_height * 0.3, true, outline_color);
            gameboy.renderer.draw_rounded_rectangle(gameboy.window, segment_position, segment_width * 0.99, segment_height * 0.92, segment_height * 0.3 * 0.9, true, (*gameboy.modifying_palette)[i]);
        }
    }


    void ModifyPaletteState::enter_color_picker_state() {
        gameboy.modifying_palette_segment = cursor;
        enter_new_state(COLOR_PICKER);
    }
}