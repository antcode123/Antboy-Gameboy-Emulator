#pragma once


#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>
#include "../gameboy.hpp"
#include "States/state.hpp"


namespace UI {
    class StateManager {
    public:
        enum StateType {
            MAIN_MENU, INSERT_ROM, EMULATION, PAUSED, SETTINGS, GENERAL_SETTINGS, DISPLAY_SETTINGS,
            PALETTES, MODIFY_PALETTE, COLOR_PICKER, CONTROLS, INPUT_SELECT, REMAP_BINDS
        };

        Gameboy& gameboy;
        std::shared_ptr<State> current_state;
        std::stack<std::shared_ptr<State>> current_state_stack;

        StateManager(Gameboy& _gameboy);
        void reset();
        void run();
        void fade_in_or_out_of_state(bool is_fading_in);
        void run_current_state();
        void switch_current_state();
        void push_new_state(int state_type);
    };
}