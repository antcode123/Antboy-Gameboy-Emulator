#pragma once


#include "state.hpp"


namespace UI {
    class RemapBindsState : public State {
    public:
        RemapBindsState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void handle_keyboard_events() override;
        void handle_controller_events() override;
        bool does_already_contain_bind(int bind_code);
        void perform_logic() override;
        void update_binds();
    };
}
