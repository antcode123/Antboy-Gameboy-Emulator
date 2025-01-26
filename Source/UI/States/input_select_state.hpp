#pragma once


#include "state.hpp"


namespace UI {
    class InputSelectState : public State {
    public:
        InputSelectState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void enter_remap_binds_state(std::string input_type);
    };
}
