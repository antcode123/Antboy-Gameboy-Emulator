#include <cctype>
#include "emulation_state.hpp"


namespace UI {
    EmulationState::EmulationState(Gameboy& _gameboy) :
        State(_gameboy, "INSERT ROM") {
        reset();
    }


    void EmulationState::configure_ui_elements() {
        ui_elements.clear();
    }


    void EmulationState::handle_keyboard_events() {
         if (gameboy.event.type == sf::Event::KeyPressed) {
            if (gameboy.event.key.code == gameboy.key_binds["GAME"]["PAUSE"]) enter_new_state(PAUSED);
            else gameboy.joypad.check_key_pressed(gameboy.event, gameboy.key_binds["GAME"]);
         }

         else if (gameboy.event.type == sf::Event::KeyReleased) gameboy.joypad.check_key_released(gameboy.event, gameboy.key_binds["GAME"]);
    }


    void EmulationState::handle_controller_events() {
        if (gameboy.event.type == sf::Event::JoystickButtonPressed) {
            if (gameboy.event.joystickButton.button == gameboy.controller_binds["GAME"]["PAUSE"]) enter_new_state(PAUSED);
            else gameboy.joypad.check_controller_button_pressed(gameboy.event, gameboy.controller_binds["GAME"]);
        }

        else if (gameboy.event.type == sf::Event::JoystickButtonReleased) gameboy.joypad.check_controller_button_released(gameboy.event, gameboy.controller_binds["GAME"]);
        else if (gameboy.event.type == sf::Event::JoystickMoved) gameboy.joypad.check_controller_dpad_pressed(gameboy.event, gameboy.controller_binds["GAME"]);
    }


    void EmulationState::perform_logic() {
        gameboy.emulate();
    }


    void EmulationState::render() {
        sf::Color lcd_background_color = Utilities::lerp_rgb((*gameboy.selected_palette)[0], (*gameboy.selected_palette)[1], 0.5);
        lcd_background_color = Utilities::lerp_rgb(lcd_background_color, (*gameboy.selected_palette)[3], 0.8);
        gameboy.window.clear(lcd_background_color);
        gameboy.lcd.display(gameboy.window, gameboy.selected_palette);
        display_full_screen_black_bars();
        display_fps();
        apply_screen_fade();
        gameboy.window.display();
    }


    void EmulationState::display_full_screen_black_bars() {
        if (gameboy.lcd.scale_factor != gameboy.lcd.full_screen_scale_factor) return;

        // Left black bar
        Utilities::Vector left_black_bar_position = Utilities::Vector(0, 0);
        gameboy.renderer.draw_rectangle(gameboy.window, left_black_bar_position, gameboy.lcd.scale_factor * gameboy.lcd.black_bar_width, gameboy.full_screen_mode.height, false, sf::Color::Black);

        // Right black bar
        Utilities::Vector right_black_bar_position = Utilities::Vector(gameboy.lcd.position.x + gameboy.lcd.width, 0) * gameboy.lcd.scale_factor;
        if (gameboy.lcd.is_retro_mode_enabled) right_black_bar_position = right_black_bar_position + Utilities::Vector(1, 0);
        gameboy.renderer.draw_rectangle(gameboy.window, right_black_bar_position, gameboy.lcd.scale_factor * gameboy.lcd.black_bar_width + 5, gameboy.full_screen_mode.height, false, sf::Color::Black);

        // Top black bar
        Utilities::Vector top_black_bar_position = Utilities::Vector(gameboy.lcd.position.x, 0) * gameboy.lcd.scale_factor;
        gameboy.renderer.draw_rectangle(gameboy.window, top_black_bar_position, gameboy.lcd.scale_factor * gameboy.lcd.width + 1, gameboy.lcd.scale_factor * gameboy.lcd.gap_height * 0.5, false, sf::Color::Black);

        // Bottom black bar
        Utilities::Vector bottom_black_bar_position = Utilities::Vector(gameboy.lcd.position.x, gameboy.lcd.gap_height * 0.5 + gameboy.lcd.height) * gameboy.lcd.scale_factor;
        if (gameboy.lcd.is_retro_mode_enabled) bottom_black_bar_position = bottom_black_bar_position + Utilities::Vector(0, 1);
        gameboy.renderer.draw_rectangle(gameboy.window, bottom_black_bar_position, gameboy.lcd.scale_factor * gameboy.lcd.width + 1, gameboy.lcd.scale_factor * gameboy.lcd.gap_height, false, sf::Color::Black);
    }
}