#include "palettes_state.hpp"
#include "../UI Elements/palette.hpp"


namespace UI {
    PalettesState::PalettesState(Gameboy& _gameboy) :
        State(_gameboy, "PALETTES") {
        reset();
    }


    void PalettesState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();

        for (int i = 0; i < gameboy.palettes.size(); i++) {
            ui_elements.push_back(std::make_unique<Palette>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * i), ui_elements.size() < 9, [&](){customise_palette_segment();}, gameboy.palettes[i], gameboy.font));
        }

        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), ui_element_width, ui_element_height, [&](){add_palette();}, ui_elements.size() < 9, true, "(+) ADD PALETTE", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), ui_element_width, ui_element_height, [&](){return_to_previous_state();}, ui_elements.size() < 9, true, "<- BACK", gameboy.font));
    }


    void PalettesState::perform_logic() {
        animate_ui_elements();
        update_selected_palette();
        update_scrollbar();
    }


    // Updates the palette used as the emulator theme, not the selected ui element - I could think of any better identifier :)
    void PalettesState::update_selected_palette() {
        if (cursor >= ui_elements.size() - 2) return;
        Palette *selected_palette_ui_element = (Palette*)ui_elements[cursor].get(); // Getting the palette ui element in which the cursor is currently pointing to - not the currently selected gameboy palette

        if (selected_palette_ui_element->is_selected_as_new_palette) {
            gameboy.selected_palette_pointer = cursor;
            gameboy.selected_palette = selected_palette_ui_element->selected_palette;
            selected_palette_ui_element->is_selected_as_new_palette = false;
            screen_fade_color = (*gameboy.selected_palette)[3];
        }
    }


    void PalettesState::customise_palette_segment() {
        Palette *selected_palette_ui_element = (Palette*)ui_elements[cursor].get();
        gameboy.modifying_palette = selected_palette_ui_element->selected_palette;
        enter_new_state(MODIFY_PALETTE);
    }


    void PalettesState::add_palette() {
        gameboy.palettes.push_back(std::make_shared<std::array<sf::Color, 4>>(std::array<sf::Color, 4>{
            sf::Color(255, 255, 255),
            sf::Color(170, 170, 170),
            sf::Color(85, 85, 85),
            sf::Color(0, 0, 0)
        }));

        cursor++;
        configure_ui_elements();
        update_cursor_after_ui_element_reconfiguration();
    }
}