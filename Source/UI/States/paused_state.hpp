#pragma once


#include "state.hpp"


namespace UI {
    class PausedState : public State {
    public:
        PausedState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void return_to_home();
    };
}
