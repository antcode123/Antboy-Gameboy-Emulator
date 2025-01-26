#pragma once


#include "state.hpp"


namespace UI {
    class ModifyPaletteState : public State {
    public:

        ModifyPaletteState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void configure_ui_element_parameters() override;
        void display_miscellaneous() override;
        void display_palette();
        void enter_color_picker_state();
    };
}