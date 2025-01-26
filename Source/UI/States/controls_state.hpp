#pragma once


#include "state.hpp"


namespace UI {
    class ControlsState : public State {
    public:
        ControlsState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void enter_input_select_state(std::string control_type);
    };
}
