#pragma once


#include "state.hpp"


namespace UI {
    class EmulationState : public State {
    public:
        EmulationState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void handle_keyboard_events() override;
        void handle_controller_events() override;
        void perform_logic() override;
        void render() override;
        void display_full_screen_black_bars();
    };
}
