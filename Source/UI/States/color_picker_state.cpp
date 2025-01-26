#include "color_picker_state.hpp"


namespace UI {
    ColorPickerState::ColorPickerState(Gameboy& _gameboy) :
        State(_gameboy, "Color Picker"),
        current_color((*gameboy.modifying_palette)[gameboy.modifying_palette_segment]) {
        reset();
    }


    void ColorPickerState::configure_ui_elements() {
        ui_elements.clear();
        ui_elements.push_back(std::make_unique<ColorPicker>(gameboy.lcd.position + Utilities::Vector(80, 82), gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(gameboy.lcd.position + Utilities::Vector(54, 120), 45, 8, [&](){save();}, true, true, "Save", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(gameboy.lcd.position + Utilities::Vector(106, 120), 45, 8, [&](){discard();}, true, true, "Discard", gameboy.font));
    }


    void ColorPickerState::handle_keyboard_events() {
        if (gameboy.event.type != sf::Event::KeyPressed) return;
        ColorPicker *color_picker = (ColorPicker*)ui_elements[0].get();

        if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["SELECT"]) {
            if (cursor == 0) {
                color_picker->is_enabled = !color_picker->is_enabled;
                if (!color_picker->is_enabled) update_cursor(LEFT);
                return;
            }

            ui_elements[cursor]->handle_select_event();
        }

        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["BACK"] && cursor == 0) {
            color_picker->is_enabled = false;
            update_cursor(LEFT);
        }

        else if (color_picker->is_enabled) {
            if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["LEFT"]) color_picker->handle_left_event();
            else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["RIGHT"]) color_picker->handle_right_event();
            else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["UP"]) color_picker->handle_up_event();
            else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["DOWN"]) color_picker->handle_down_event();
        }

        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["LEFT"]) update_cursor(LEFT);
        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["RIGHT"]) update_cursor(RIGHT);
        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["DOWN"]) update_cursor(LEFT);

        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["UP"]) {
            update_cursor(UP);
            color_picker->is_enabled = true;
        }
    }


    void ColorPickerState::handle_controller_events() {
        ColorPicker *color_picker = (ColorPicker*)ui_elements[0].get();

        if (gameboy.event.type == sf::Event::JoystickButtonPressed) {
            if (gameboy.event.joystickButton.button == gameboy.controller_binds["SYSTEM"]["SELECT"]) {
                if (cursor == 0) {
                    color_picker->is_enabled = !color_picker->is_enabled;
                    if (!color_picker->is_enabled) update_cursor(LEFT);
                    return;
                }

                ui_elements[cursor]->handle_select_event();
            }

            else if (gameboy.event.joystickButton.button == gameboy.controller_binds["SYSTEM"]["BACK"]) {
                color_picker->is_enabled = false;
                update_cursor(LEFT);
            }
        }

        else if (!gameboy.event.type == sf::Event::JoystickMoved) return;

        else if (gameboy.event.joystickMove.axis == sf::Joystick::PovX) {
            if (color_picker->is_enabled) {
                if (gameboy.event.joystickMove.position < -50) color_picker->handle_left_event();
                else if (gameboy.event.joystickMove.position > 50) color_picker->handle_right_event();
            }

            else if (gameboy.event.joystickMove.position < -50) update_cursor(LEFT);
            else if (gameboy.event.joystickMove.position > 50) update_cursor(RIGHT);
        }

        else if (gameboy.event.joystickMove.axis == sf::Joystick::PovY) {
            if (color_picker->is_enabled) {
                if (gameboy.event.joystickMove.position > 50) color_picker->handle_up_event();
                else if (gameboy.event.joystickMove.position < -50) color_picker->handle_down_event();
            }

            else if (gameboy.event.joystickMove.position > 50) {
                update_cursor(UP);
                color_picker->is_enabled = true;
            }

            else if (gameboy.event.joystickMove.position < -50) update_cursor(LEFT);
        }
    }


    void ColorPickerState::update_cursor(int direction) {
        switch (direction) {
            case UP: cursor = 0; break;
            case LEFT: cursor = 1; break;
            case RIGHT: cursor = 2; break;
        }

        update_selected_ui_element();
    }


    void ColorPickerState::perform_logic() {
        animate_ui_elements();
        update_current_color();
    }


    void ColorPickerState::update_current_color() {
        ColorPicker *color_picker = (ColorPicker*)ui_elements[0].get();
        current_color = color_picker->color_grid[color_picker->cursor.y][color_picker->cursor.x];
    }


    void ColorPickerState::display_miscellaneous() {
        display_current_color();
    }


    void ColorPickerState::display_current_color() {
        float current_color_width = gameboy.lcd.scale_factor * 105;
        float current_color_height = gameboy.lcd.scale_factor * 10;
        float current_color_border_radius = current_color_height * 0.25;
        Utilities::Vector current_color_position = (gameboy.lcd.position + Utilities::Vector(80, 43)) * gameboy.lcd.scale_factor;
        gameboy.renderer.draw_rounded_rectangle(gameboy.window, current_color_position, current_color_width, current_color_height, current_color_border_radius, true, (*gameboy.selected_palette)[0]);
        gameboy.renderer.draw_rounded_rectangle(gameboy.window, current_color_position, current_color_width * 0.995, current_color_height * 0.94, current_color_border_radius, true, current_color);
    }


    void ColorPickerState::save() {
        (*gameboy.modifying_palette)[gameboy.modifying_palette_segment] = current_color;
        screen_fade_color = (*gameboy.selected_palette)[3];
        return_to_previous_state();
    }


    void ColorPickerState::discard() {
        return_to_previous_state();
    }
}