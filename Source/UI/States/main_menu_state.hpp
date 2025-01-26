#pragma once


#include "state.hpp"


namespace UI {
    class MainMenuState : public State {
    public:
        MainMenuState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void configure_ui_element_parameters() override;
        void display_heading() override;
    };
}