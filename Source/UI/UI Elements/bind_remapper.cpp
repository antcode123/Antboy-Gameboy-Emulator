#include <cmath>
#include "bind_remapper.hpp"


namespace UI {
    BindRemapper::BindRemapper(Utilities::Vector _position, std::string _input_type, std::string _bind_type, int _bind_code, bool _is_in_view, sf::Font& _font) :
        UIElement(_position, 134, 8, nullptr, _is_in_view, false, "", _font),
        input_type(_input_type),
        is_binding(false),
        bind_type(_bind_type),
        bind_code(_bind_code) {
        binding_animation_counter = M_PI;
        key_map[sf::Keyboard::A] = "A";
        key_map[sf::Keyboard::B] = "B";
        key_map[sf::Keyboard::C] = "C";
        key_map[sf::Keyboard::D] = "D";
        key_map[sf::Keyboard::E] = "E";
        key_map[sf::Keyboard::F] = "F";
        key_map[sf::Keyboard::G] = "G";
        key_map[sf::Keyboard::H] = "H";
        key_map[sf::Keyboard::I] = "I";
        key_map[sf::Keyboard::J] = "J";
        key_map[sf::Keyboard::K] = "K";
        key_map[sf::Keyboard::L] = "L";
        key_map[sf::Keyboard::M] = "M";
        key_map[sf::Keyboard::N] = "N";
        key_map[sf::Keyboard::O] = "O";
        key_map[sf::Keyboard::P] = "P";
        key_map[sf::Keyboard::Q] = "Q";
        key_map[sf::Keyboard::R] = "R";
        key_map[sf::Keyboard::S] = "S";
        key_map[sf::Keyboard::T] = "T";
        key_map[sf::Keyboard::U] = "U";
        key_map[sf::Keyboard::V] = "V";
        key_map[sf::Keyboard::W] = "W";
        key_map[sf::Keyboard::X] = "X";
        key_map[sf::Keyboard::Y] = "Y";
        key_map[sf::Keyboard::Z] = "Z";
        key_map[sf::Keyboard::Num0] = "0";
        key_map[sf::Keyboard::Num1] = "1";
        key_map[sf::Keyboard::Num2] = "2";
        key_map[sf::Keyboard::Num3] = "3";
        key_map[sf::Keyboard::Num4] = "4";
        key_map[sf::Keyboard::Num5] = "5";
        key_map[sf::Keyboard::Num6] = "6";
        key_map[sf::Keyboard::Num7] = "7";
        key_map[sf::Keyboard::Num8] = "8";
        key_map[sf::Keyboard::Num9] = "9";
        key_map[sf::Keyboard::Escape] = "ESC";
        key_map[sf::Keyboard::LControl] = "L-CTRL";
        key_map[sf::Keyboard::LShift] = "L-SHIFT";
        key_map[sf::Keyboard::LAlt] = "L-ALT";
        key_map[sf::Keyboard::LSystem] = "L-SYSTEM";
        key_map[sf::Keyboard::RControl] = "R-CTRL";
        key_map[sf::Keyboard::RShift] = "R-SHIFT";
        key_map[sf::Keyboard::RAlt] = "R-ALT";
        key_map[sf::Keyboard::RSystem] = "R-SYSTEM";
        key_map[sf::Keyboard::Menu] = "MENU";
        key_map[sf::Keyboard::LBracket] = "[";
        key_map[sf::Keyboard::RBracket] = "]";
        key_map[sf::Keyboard::SemiColon] = ";";
        key_map[sf::Keyboard::Comma] = ",";
        key_map[sf::Keyboard::Period] = ".";
        key_map[sf::Keyboard::Quote] = "\"";
        key_map[sf::Keyboard::Slash] = "/";
        key_map[sf::Keyboard::BackSlash] = "\\";
        key_map[sf::Keyboard::Tilde] = "~";
        key_map[sf::Keyboard::Equal] = "=";
        key_map[sf::Keyboard::Dash] = "-";
        key_map[sf::Keyboard::Space] = "SPACE";
        key_map[sf::Keyboard::Return] = "ENTER";
        key_map[sf::Keyboard::BackSpace] = "BACKSPACE";
        key_map[sf::Keyboard::Tab] = "TAB";
        key_map[sf::Keyboard::PageUp] = "PAGEUP";
        key_map[sf::Keyboard::PageDown] = "PAGEDOWN";
        key_map[sf::Keyboard::End] = "END";
        key_map[sf::Keyboard::Home] = "HOME";
        key_map[sf::Keyboard::Insert] = "INSERT";
        key_map[sf::Keyboard::Delete] = "DELETE";
        key_map[sf::Keyboard::Add] = "ADD";
        key_map[sf::Keyboard::Subtract] = "SUBTRACT";
        key_map[sf::Keyboard::Multiply] = "MULTIPLY";
        key_map[sf::Keyboard::Divide] = "DIVIDE";
        key_map[sf::Keyboard::Left] = "LEFT";
        key_map[sf::Keyboard::Right] = "RIGHT";
        key_map[sf::Keyboard::Up] = "UP";
        key_map[sf::Keyboard::Down] = "DOWN";
        key_map[sf::Keyboard::Numpad0] = "NUMPAD0";
        key_map[sf::Keyboard::Numpad1] = "NUMPAD1";
        key_map[sf::Keyboard::Numpad2] = "NUMPAD2";
        key_map[sf::Keyboard::Numpad3] = "NUMPAD3";
        key_map[sf::Keyboard::Numpad4] = "NUMPAD4";
        key_map[sf::Keyboard::Numpad5] = "NUMPAD5";
        key_map[sf::Keyboard::Numpad6] = "NUMPAD6";
        key_map[sf::Keyboard::Numpad7] = "NUMPAD7";
        key_map[sf::Keyboard::Numpad8] = "NUMPAD8";
        key_map[sf::Keyboard::Numpad9] = "NUMPAD9";
        key_map[sf::Keyboard::F1] = "F1";
        key_map[sf::Keyboard::F2] = "F2";
        key_map[sf::Keyboard::F3] = "F3";
        key_map[sf::Keyboard::F4] = "F4";
        key_map[sf::Keyboard::F5] = "F5";
        key_map[sf::Keyboard::F6] = "F6";
        key_map[sf::Keyboard::F7] = "F7";
        key_map[sf::Keyboard::F8] = "F8";
        key_map[sf::Keyboard::F9] = "F9";
        key_map[sf::Keyboard::F10] = "F10";
        key_map[sf::Keyboard::F11] = "F11";
        key_map[sf::Keyboard::F12] = "F12";
        controller_map[0] = "A";
        controller_map[1] = "B";
        controller_map[2] = "X";
        controller_map[3] = "Y";
        controller_map[4] = "LB";
        controller_map[5] = "RB";
        controller_map[6] = "VIEW";
        controller_map[7] = "MENU";
        controller_map[8] = "L3";
        controller_map[9] = "R3";
        controller_map[10] = "HOME";
        controller_map[11] = "SHARE";
    }


    void BindRemapper::handle_select_event() {is_binding = true;}


    void BindRemapper::remap_bind(int _bind_code) {
        bind_code = _bind_code;
        is_binding = false;
    }


    void BindRemapper::animate(double delta_time) {
        double animation_rate = delta_time * 0.75;
        animation_scale_factor = is_selected ? std::min(animation_scale_factor + animation_rate, 1.08) : std::max(animation_scale_factor - animation_rate, 1.0);

        // "PRESS ANY KEY" pulsing animation
        if (is_binding) {
            binding_animation_counter += delta_time * 4;
            if (binding_animation_counter > 2 * M_PI) binding_animation_counter = 0;
            binding_animation_opacity = 127.5 * (cos(binding_animation_counter) + 1); // Maps cosinusoidal output between alpha values 0 - 255
        }

        else binding_animation_counter = M_PI;
    }


    void BindRemapper::display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) {
        if (!is_in_view) return;

        sf::Color color = is_selected ? (*palette)[2] : (*palette)[1];
        Utilities::Vector scaled_position(position * scale_factor);
        double total_scale_factor = scale_factor * animation_scale_factor;

        renderer.draw_rounded_rectangle(window, scaled_position, total_scale_factor * width, total_scale_factor * height, total_scale_factor * height * 0.3, true, color);
        float text_x_offset = scaled_position.x - total_scale_factor * (width * 0.5 - (height - font_size));
        renderer.draw_text(window, Utilities::Vector(text_x_offset, scaled_position.y), bind_type, font, total_scale_factor * font_size, is_text_centered, (*palette)[0]);

        Utilities::Vector left_bracket_position(scaled_position.x + total_scale_factor * 17, scaled_position.y);
        Utilities::Vector right_bracket_position(scaled_position.x + total_scale_factor * 60, scaled_position.y);
        Utilities::Vector arrow_position(scaled_position.x - total_scale_factor * 20, scaled_position.y);
        Utilities::Vector option_position = Utilities::lerp_2D(left_bracket_position, right_bracket_position, 0.5);

        renderer.draw_text(window, left_bracket_position, "<", font, total_scale_factor * font_size, true, (*palette)[0]);
        renderer.draw_text(window, right_bracket_position, ">", font, total_scale_factor * font_size, true, (*palette)[0]);
        renderer.draw_text(window, arrow_position, "->", font, total_scale_factor * font_size, true, (*palette)[0]);

        std::string bind_code_alias = input_type == "KEYBOARD" ? key_map[bind_code] : controller_map[bind_code];

        if (is_binding) {
            sf::Color binding_color = (*palette)[0];
            binding_color.a = binding_animation_opacity;
            std::string key_button = input_type == "KEYBOARD" ? "KEY" : "BUTTON";
            bind_code_alias = "PRESS ANY " + key_button;
            renderer.draw_text(window, option_position, "PRESS ANY " + key_button, font, total_scale_factor * font_size, true, binding_color);
        }

        else renderer.draw_text(window, option_position, bind_code_alias, font, total_scale_factor * font_size, true, (*palette)[0]);
    }
}