#include "state_manager.hpp"
#include "States/main_menu_state.hpp"
#include "States/settings_state.hpp"
#include "States/general_settings_state.hpp"
#include "States/display_settings_state.hpp"
#include "States/palettes_state.hpp"
#include "States/modify_palette_state.hpp"
#include "States/color_picker_state.hpp"
#include "States/controls_state.hpp"
#include "States/input_select_state.hpp"
#include "States/remap_binds_state.hpp"
#include "States/insert_rom_state.hpp"
#include "States/emulation_state.hpp"
#include "States/paused_state.hpp"


namespace UI {

    // My Emulator uses an app manager for the UI
    // The app manager is managers the emulator state stack in which the top stores the current state the user is within
    // Switching to a new state will push a new state onto the state stack and going back to the previous will pop from the state stack
    StateManager::StateManager(Gameboy& _gameboy) :
        gameboy(_gameboy) {
        reset();
    }


    void StateManager::reset() {
        current_state_stack = std::stack<std::shared_ptr<State>>();
        current_state_stack.push(std::make_shared<MainMenuState>(gameboy));
    }


    void StateManager::run() {
        while (!current_state_stack.empty() && gameboy.window.isOpen()) {
            current_state = current_state_stack.top();
            gameboy.update_fps();
            if (current_state->is_running && current_state->screen_fade_opacity > 0) fade_in_or_out_of_state(true);
            else if (!current_state->is_running && current_state->screen_fade_opacity < 255) fade_in_or_out_of_state(false);
            else if (current_state->is_running) run_current_state();
            else switch_current_state();
        }
    }


    void StateManager::fade_in_or_out_of_state(bool is_fading_in) {
        if (is_fading_in) current_state->fade_in();
        else current_state->fade_out();
        while (gameboy.window.pollEvent(gameboy.event)) {}
        current_state->perform_logic();
        current_state->render();
    }


    void StateManager::run_current_state() {
        current_state->handle_events();
        current_state->perform_logic();
        current_state->render();
    }


    void StateManager::switch_current_state() {
        if (current_state->has_entered_new_state) push_new_state(current_state->transition_state);

        else {
            current_state_stack.pop();
            current_state_stack.top()->reset();
        }
    }


    void StateManager::push_new_state(int state_type) {
        switch (state_type) {
            case MAIN_MENU: reset(); break;
            case SETTINGS: current_state_stack.push(std::make_shared<SettingsState>(gameboy)); break;
            case GENERAL_SETTINGS: current_state_stack.push(std::make_shared<GeneralSettingsState>(gameboy)); break;
            case DISPLAY_SETTINGS: current_state_stack.push(std::make_shared<DisplaySettingsState>(gameboy)); break;
            case PALETTES: current_state_stack.push(std::make_shared<PalettesState>(gameboy)); break;
            case MODIFY_PALETTE: current_state_stack.push(std::make_shared<ModifyPaletteState>(gameboy)); break;
            case COLOR_PICKER: current_state_stack.push(std::make_shared<ColorPickerState>(gameboy)); break;
            case CONTROLS: current_state_stack.push(std::make_shared<ControlsState>(gameboy)); break;
            case INPUT_SELECT: current_state_stack.push(std::make_shared<InputSelectState>(gameboy)); break;
            case REMAP_BINDS: current_state_stack.push(std::make_shared<RemapBindsState>(gameboy)); break;
            case INSERT_ROM: current_state_stack.push(std::make_shared<InsertRomState>(gameboy)); break;
            case EMULATION: current_state_stack.push(std::make_shared<EmulationState>(gameboy)); break;
            case PAUSED: current_state_stack.push(std::make_shared<PausedState>(gameboy)); break;
            default: current_state_stack.push(std::make_shared<State>(gameboy, "UNDEFINED STATE")); break;
        }
    }
}