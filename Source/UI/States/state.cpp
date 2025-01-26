#include <cmath>
#include "state.hpp"

int screenshot_count = 0;

namespace UI {
    State::State(Gameboy& _gameboy, std::string _name) :
        gameboy(_gameboy),
        has_entered_new_state(false),
        is_running(true),
        name(_name),
        cursor(0) {
        reset();
    }


    void State::reset() {
        is_running = true;
        cursor = 0;
        screen_fade_opacity = 255;
        screen_fade_color = (*gameboy.selected_palette)[3];
        configure_ui_elements();
        configure_scrollbar();
        update_selected_ui_element();
    }


    void State::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset, ui_element_width, ui_element_height, [&](){return_to_previous_state();}, true, true, "<- BACK", gameboy.font));
    }


    void State::configure_ui_element_parameters() {
        length_between_ui_elements = 11;
        ui_element_offset = gameboy.lcd.position + Utilities::Vector(gameboy.lcd.width * 0.5, 39);
        ui_element_width = 134;
        ui_element_height = 8;
    }


    void State::configure_scrollbar() {
        scrollbar_height = 103;
        scrollbar_width = 1;
        scrollbar_size = (4.0 / ui_elements.size()) * scrollbar_height;
        scrollbar_start = gameboy.lcd.position + Utilities::Vector(156, 33 + scrollbar_size * 0.5);
        scrollbar_end = scrollbar_start + Utilities::Vector(0, scrollbar_height - scrollbar_size);
    }


    void State::handle_events() {
        while (gameboy.window.pollEvent(gameboy.event)) {
            if (gameboy.event.type == sf::Event::Closed) exit_app();
            if (gameboy.event.type == sf::Event::MouseButtonPressed && gameboy.event.mouseButton.button == 2) {
                // Create a texture to hold the contents of the window
                sf::Texture texture;
                if (texture.create(gameboy.window.getSize().x, gameboy.window.getSize().y)) {
                    // Copy the window's current frame into the texture
                    texture.update(gameboy.window);

                    // Get the texture's image
                    sf::Image screenshot = texture.copyToImage();

                    // Save the screenshot as a file
                    if (screenshot.saveToFile("insert_rom_menu.png")) {
                        std::cout << "Screenshot saved as screenshot.png" << std::endl;
                    } else {
                        std::cerr << "Failed to save screenshot!" << std::endl;
                    }
                    screenshot_count++;
                } else {
                    std::cerr << "Failed to create texture for screenshot!" << std::endl;
                }
            }

            handle_keyboard_events();
            handle_controller_events();
        }
    }


    void State::exit_app() {
        is_running = false;
        gameboy.window.close();
    }


    void State::handle_keyboard_events() {
        if (gameboy.event.type != sf::Event::KeyPressed) return;
        if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["UP"]) update_cursor(-1);
        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["DOWN"]) update_cursor(1);
        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["SELECT"]) ui_elements[cursor]->handle_select_event();
        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["LEFT"]) ui_elements[cursor]->handle_left_event();
        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["RIGHT"]) ui_elements[cursor]->handle_right_event();
        else if (gameboy.event.key.code == gameboy.key_binds["SYSTEM"]["BACK"]) jump_to_last_button();
    }


    void State::handle_controller_events() {
        if (gameboy.event.type == sf::Event::JoystickButtonPressed) {
            if (gameboy.event.joystickButton.button == gameboy.controller_binds["SYSTEM"]["SELECT"]) ui_elements[cursor]->handle_select_event();
            else if (gameboy.event.joystickButton.button == gameboy.controller_binds["SYSTEM"]["BACK"]) jump_to_last_button();
        }

        else if (gameboy.event.type != sf::Event::JoystickMoved) return;

        if (gameboy.event.joystickMove.axis == sf::Joystick::PovX) {
            if (gameboy.event.joystickMove.position < -50) ui_elements[cursor]->handle_left_event();
            else if (gameboy.event.joystickMove.position > 50) ui_elements[cursor]->handle_right_event();
        }

        else if (gameboy.event.joystickMove.axis == sf::Joystick::PovY) {
            if (gameboy.event.joystickMove.position > 50) update_cursor(-1);
            else if (gameboy.event.joystickMove.position < -50) update_cursor(1);
        }
    }


    void State::jump_to_last_button() {
        ui_elements[cursor]->reset();
        int previous_cursor = ui_elements.size() - 1;
        int range = previous_cursor - cursor;
        for (int i = 0; i < range; i++) update_cursor(1);
        update_selected_ui_element();
    }


    void State::perform_logic() {
        animate_ui_elements();
        update_scrollbar();
    }


    void State::update_scrollbar() {

        // Interpolating the scrollbar position with the cursora
        double t = (double)cursor / (ui_elements.size() - 1);
        scrollbar_position = Utilities::lerp_2D(scrollbar_start, scrollbar_end, t);
    }


    void State::animate_ui_elements() {for (std::unique_ptr<UIElement>& ui_element : ui_elements) ui_element->animate(gameboy.delta_time);}


    void State::render() {
        gameboy.window.clear((*gameboy.selected_palette)[3]);
        display_heading();
        display_fps();
        display_ui_elements();
        display_scrollbar();
        display_miscellaneous();
        apply_screen_fade();
        gameboy.window.display();
    }


    void State::display_ui_elements() {for (auto& ui_element : ui_elements) ui_element->display(gameboy.window, gameboy.renderer, gameboy.lcd.scale_factor, gameboy.selected_palette);}


    void State::display_fps() {
        if (!gameboy.is_display_fps_enabled) return;

        if (gameboy.lcd.scale_factor == gameboy.lcd.full_screen_scale_factor) {
            gameboy.renderer.draw_text(gameboy.window, Utilities::Vector(3, 4) * gameboy.lcd.scale_factor, "FPS: " + std::to_string((int)(gameboy.fps)), gameboy.font, gameboy.lcd.scale_factor * 3, false, (*gameboy.selected_palette)[0]);
        }

        else {
            gameboy.renderer.draw_rectangle(gameboy.window, gameboy.lcd.position * gameboy.lcd.scale_factor, gameboy.lcd.scale_factor * 22, gameboy.lcd.scale_factor * 8, false, (*gameboy.selected_palette)[3]);
            gameboy.renderer.draw_text(gameboy.window, (gameboy.lcd.position + Utilities::Vector(3, 4)) * gameboy.lcd.scale_factor, "FPS: " + std::to_string((int)(gameboy.fps)), gameboy.font, gameboy.lcd.scale_factor * 3, false, (*gameboy.selected_palette)[0]);
        }
    }


    void State::display_heading() {
        if (gameboy.lcd.scale_factor == gameboy.lcd.full_screen_scale_factor) {
            gameboy.renderer.draw_rectangle(gameboy.window, Utilities::Vector(0, gameboy.lcd.position.y + 13) * gameboy.lcd.scale_factor, gameboy.full_screen_mode.width, gameboy.lcd.scale_factor * 10, false, (*gameboy.selected_palette)[2]);
        }

        else {
            gameboy.renderer.draw_rectangle(gameboy.window, (gameboy.lcd.position + Utilities::Vector(0, 13)) * gameboy.lcd.scale_factor, gameboy.lcd.width * gameboy.lcd.scale_factor, gameboy.lcd.scale_factor * 10, false, (*gameboy.selected_palette)[2]);
        }

        gameboy.renderer.draw_text(gameboy.window, (gameboy.lcd.position + Utilities::Vector(gameboy.lcd.width * 0.5, 18)) * gameboy.lcd.scale_factor, name, gameboy.font, gameboy.lcd.scale_factor * 4, true, (*gameboy.selected_palette)[0]);
    }


    void State::display_scrollbar() {
        if (ui_elements.size() <= 9) return;

        // Draws scrollbar poll
        Utilities::Vector scrollbar_poll_position = Utilities::Vector(gameboy.lcd.position + Utilities::Vector(156, 84)) * gameboy.lcd.scale_factor;
        gameboy.renderer.draw_rounded_rectangle(gameboy.window, scrollbar_poll_position, gameboy.lcd.scale_factor * scrollbar_width, gameboy.lcd.scale_factor * scrollbar_height, gameboy.lcd.scale_factor * scrollbar_width * 0.5, true, (*gameboy.selected_palette)[1]);

        // Draws scrollbar mover
        Utilities::Vector scrollbar_mover_position(scrollbar_position * gameboy.lcd.scale_factor);
        gameboy.renderer.draw_rounded_rectangle(gameboy.window, scrollbar_mover_position, gameboy.lcd.scale_factor * scrollbar_width * 2, gameboy.lcd.scale_factor * scrollbar_size * 1.1, gameboy.lcd.scale_factor * 0.5, true, (*gameboy.selected_palette)[2]);
    }


    void State::display_miscellaneous() {}


    void State::update_cursor(int direction) {
        ui_elements[cursor]->reset();
        cursor += direction;
        cursor = (cursor < 0) ? 0 : cursor;  // Makes sure the cursor cannot be below zero
        cursor = (cursor == ui_elements.size()) ? cursor - 1 : cursor;  // Makes suer the cursor cannot be the size of the UI elements
        if (direction == 1 && cursor > 4 && cursor < ui_elements.size() - 4) scroll_ui_elements(-1);  // Scroll the UI elements up when the cursor is the 5th button from the 9 active ui_elements
        else if (direction == -1 && cursor > 3 && cursor < ui_elements.size() - 5) scroll_ui_elements(1); // Scroll the UI elements down when the cursor is the 5th button from the 9 active ui_elements
        update_selected_ui_element();
    }


    void State::update_cursor_after_ui_element_reconfiguration() {
        int previous_cursor = cursor;
        cursor = 0;
        for (int i = 0; i < previous_cursor; i++) update_cursor(1); // Moves the cursor to the position before reconfiguration
    }


    void State::scroll_ui_elements(int direction) {
        for (int i = 0; i < ui_elements.size(); i++) {
            ui_elements[i]->position.y += direction * length_between_ui_elements;
            ui_elements[i]->is_in_view = (i >= cursor - 4 && i <= cursor + 4) ? true : false;
        }
    }


    void State::update_selected_ui_element() {for (int i = 0; i < ui_elements.size(); i++) ui_elements[i]->is_selected = i == cursor ? true : false;}


    void State::enter_new_state(int state_type) {
        is_running = false;
        has_entered_new_state = true;
        transition_state = state_type;
    }


    void State::fade_in() {
        float animation_rate = 750 * gameboy.delta_time;
        screen_fade_opacity -= animation_rate;
        screen_fade_opacity = std::max(0.0, screen_fade_opacity);
    }


    void State::fade_out() {
        float animation_rate = 700 * gameboy.delta_time;
        screen_fade_opacity += animation_rate;
        screen_fade_opacity = std::min(255.0, screen_fade_opacity);
    }


    void State::apply_screen_fade() {
        if ((is_running && screen_fade_opacity >= 0) || (!is_running && screen_fade_opacity <= 255)) {
            screen_fade_color.a = (int)screen_fade_opacity;
            if (gameboy.lcd.scale_factor == gameboy.lcd.full_screen_scale_factor) gameboy.renderer.draw_rectangle(gameboy.window, Utilities::Vector(0, 0), gameboy.full_screen_mode.width, gameboy.full_screen_mode.height, false, screen_fade_color);
            else gameboy.renderer.draw_rectangle(gameboy.window, gameboy.lcd.position * gameboy.lcd.scale_factor, gameboy.lcd.scale_factor * gameboy.lcd.width, gameboy.lcd.scale_factor * gameboy.lcd.height, false, screen_fade_color);
        }
    }


    void State::return_to_previous_state() {
        is_running = false;
        has_entered_new_state = false;
    }

}
