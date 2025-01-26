#pragma once


#include "state.hpp"
#include "../UI Elements/color_picker.hpp"


namespace UI {
    class ColorPickerState : public State {
    public:
        enum DIRECTION {LEFT, RIGHT, UP};
        sf::Color current_color;

        ColorPickerState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void handle_keyboard_events() override;
        void handle_controller_events() override;
        void update_cursor(int direction) override;
        void perform_logic() override;
        void update_current_color();
        void display_miscellaneous() override;
        void display_current_color();
        void save();
        void discard();
    };
}
