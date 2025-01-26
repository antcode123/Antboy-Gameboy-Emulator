#include "remap_binds_state.hpp"
#include "../UI Elements/bind_remapper.hpp"


namespace UI {
    RemapBindsState::RemapBindsState(Gameboy& _gameboy) :
        State(_gameboy, "REMAP " + _gameboy.modifying_control_type + " " + _gameboy.modifying_input_type + " BINDS") {
        reset();
    }


    void RemapBindsState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();

        if (gameboy.modifying_control_type == "SYSTEM" && gameboy.modifying_input_type == "KEYBOARD") {
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "SELECT", gameboy.key_binds["SYSTEM"]["SELECT"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "BACK", gameboy.key_binds["SYSTEM"]["BACK"], true, gameboy.font));
        }

        else if (gameboy.modifying_control_type == "GAME" && gameboy.modifying_input_type == "KEYBOARD") {
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "SELECT", gameboy.key_binds["GAME"]["SELECT"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "START", gameboy.key_binds["GAME"]["START"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "A", gameboy.key_binds["GAME"]["A"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "B", gameboy.key_binds["GAME"]["B"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "PAUSE", gameboy.key_binds["GAME"]["PAUSE"], true, gameboy.font));
        }

        else if (gameboy.modifying_control_type == "SYSTEM" && gameboy.modifying_input_type == "CONTROLLER") {
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "CONTROLLER", "SELECT", gameboy.controller_binds["SYSTEM"]["SELECT"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "CONTROLLER", "BACK", gameboy.controller_binds["SYSTEM"]["BACK"], true, gameboy.font));
        }

        else if (gameboy.modifying_control_type == "GAME" && gameboy.modifying_input_type == "CONTROLLER") {
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "CONTROLLER", "SELECT", gameboy.controller_binds["GAME"]["SELECT"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "CONTROLLER", "START", gameboy.controller_binds["GAME"]["START"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "CONTROLLER", "A", gameboy.controller_binds["GAME"]["A"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "CONTROLLER", "B", gameboy.controller_binds["GAME"]["B"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "CONTROLLER", "PAUSE", gameboy.controller_binds["GAME"]["PAUSE"], true, gameboy.font));
        }

        if (gameboy.modifying_input_type == "KEYBOARD") {
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "LEFT", gameboy.key_binds[gameboy.modifying_control_type]["LEFT"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "RIGHT", gameboy.key_binds[gameboy.modifying_control_type]["RIGHT"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "UP", gameboy.key_binds[gameboy.modifying_control_type]["UP"], true, gameboy.font));
            ui_elements.push_back(std::make_unique<BindRemapper>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), "KEYBOARD", "DOWN", gameboy.key_binds[gameboy.modifying_control_type]["DOWN"], true, gameboy.font));
        }


        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), ui_element_width, ui_element_height, [&](){return_to_previous_state();}, ui_elements.size() < 9, true, "<- BACK", gameboy.font));
    }


    void RemapBindsState::handle_keyboard_events() {
        if (gameboy.event.type != sf::Event::KeyPressed) return;

        // Edge case where the cursor is at the back button so trying to get the selected bind remapper would lead to undefined behaviour
        if (cursor == ui_elements.size() - 1) {
            if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["UP"]) update_cursor(-1);
            else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["DOWN"]) update_cursor(1);
            else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["SELECT"]) ui_elements[cursor]->handle_select_event();
            return;
        }

        BindRemapper* selected_bind_remapper = (BindRemapper*)ui_elements[cursor].get();

        if (selected_bind_remapper->is_binding && selected_bind_remapper->key_map.count(gameboy.event.key.code) && gameboy.modifying_input_type == "KEYBOARD") {
            if (!does_already_contain_bind(gameboy.event.key.code)) selected_bind_remapper->remap_bind(gameboy.event.key.code);
            selected_bind_remapper->is_binding = false;
        }

        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["UP"]) {
            update_cursor(-1);
            selected_bind_remapper->is_binding = false;
        }

        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["DOWN"]) {
            update_cursor(1);
            selected_bind_remapper->is_binding = false;
        }

        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["SELECT"]) ui_elements[cursor]->handle_select_event();
        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["LEFT"]) ui_elements[cursor]->handle_left_event();
        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["RIGHT"]) ui_elements[cursor]->handle_right_event();

        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["BACK"]) {
            jump_to_last_button();
            selected_bind_remapper->is_binding = false;
        }
    }


    void RemapBindsState::handle_controller_events() {

        // Same edge case as for keyboard events
        if (cursor == ui_elements.size() - 1) {
            if (gameboy.event.type == sf::Event::JoystickButtonPressed && gameboy.event.joystickButton.button == gameboy.controller_binds["SYSTEM"]["SELECT"]) {
                ui_elements[cursor]->handle_select_event();
            }

            else if (gameboy.event.joystickMove.axis == sf::Joystick::PovY) {
                if (gameboy.event.joystickMove.position > 50) update_cursor(-1);
                else if (gameboy.event.joystickMove.position < -50) update_cursor(1);
            }

            return;
        }

        BindRemapper* selected_bind_remapper = (BindRemapper*)ui_elements[cursor].get();

        if (gameboy.event.type == sf::Event::JoystickButtonPressed) {
            if (selected_bind_remapper->is_binding && selected_bind_remapper->controller_map.count(gameboy.event.joystickButton.button) && gameboy.modifying_input_type == "CONTROLLER") {
                if (!does_already_contain_bind(gameboy.event.joystickButton.button)) selected_bind_remapper->remap_bind(gameboy.event.joystickButton.button);
                selected_bind_remapper->is_binding = false;
            }

            else if (gameboy.event.joystickButton.button == gameboy.controller_binds["SYSTEM"]["SELECT"]) ui_elements[cursor]->handle_select_event();

            else if (gameboy.event.joystickButton.button == gameboy.controller_binds["SYSTEM"]["BACK"]) {
                jump_to_last_button();
                selected_bind_remapper->is_binding = false;
            }
        }

        else if (gameboy.event.type != sf::Event::JoystickMoved) return;

        if (gameboy.event.joystickMove.axis == sf::Joystick::PovX) {
            if (gameboy.event.joystickMove.position < -50) ui_elements[cursor]->handle_left_event();
            else if (gameboy.event.joystickMove.position > 50) ui_elements[cursor]->handle_right_event();
        }

        else if (gameboy.event.joystickMove.axis == sf::Joystick::PovY) {
            if (gameboy.event.joystickMove.position > 50) {
                update_cursor(-1);
                selected_bind_remapper->is_binding = false;
            }

            else if (gameboy.event.joystickMove.position < -50) {
                update_cursor(1);
                selected_bind_remapper->is_binding = false;
            }
        }
    }


    bool RemapBindsState::does_already_contain_bind(int bind_code)  {
        if (gameboy.modifying_input_type == "KEYBOARD") {
            for (const auto& item : gameboy.key_binds[gameboy.modifying_control_type].items()) {
                if (item.value() == bind_code) return true;
            }

            return false;
        }

        for (const auto& item : gameboy.controller_binds[gameboy.modifying_control_type].items()) {
            if (item.value() == bind_code) return true;
        }

        return false;
    }


    void RemapBindsState::perform_logic() {
        animate_ui_elements();
        update_scrollbar();
        update_binds();
    }


    void RemapBindsState::update_binds() {
        if (cursor == ui_elements.size() - 1) return;
        BindRemapper* selected_bind_remapper = (BindRemapper*)ui_elements[cursor].get();
        if (gameboy.modifying_input_type == "KEYBOARD") gameboy.key_binds[gameboy.modifying_control_type][selected_bind_remapper->bind_type] = selected_bind_remapper->bind_code;
        else gameboy.controller_binds[gameboy.modifying_control_type][selected_bind_remapper->bind_type] = selected_bind_remapper->bind_code;
    }
}