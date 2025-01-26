#pragma once


#include "state.hpp"


namespace UI {
    class SettingsState : public State {
    public:
        SettingsState(Gameboy& _gameboy);
        void configure_ui_elements() override;
    };
}
