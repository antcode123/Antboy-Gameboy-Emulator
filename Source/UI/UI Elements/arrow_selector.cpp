#include "arrow_selector.hpp"


namespace UI {
    ArrowSelector::ArrowSelector(Utilities::Vector _position, bool _is_in_view, std::string _text, std::vector<std::string> _options, std::string _current_selection, sf::Font& _font) :
        UIElement(_position, 134, 8, nullptr, _is_in_view, false, _text, _font),
        options(_options),
        current_selection(_current_selection) {
        update_cursor();
    }


    void ArrowSelector::handle_left_event() {
        cursor = (cursor + options.size() - 1) % (options.size()); // Toggles left
        current_selection = options[cursor];
    }


    void ArrowSelector::handle_right_event() {
        cursor = (cursor + 1) % (options.size()); // Toggles right
        current_selection = options[cursor];
    }


    void ArrowSelector::handle_select_event() {handle_right_event();}


    void ArrowSelector::update_cursor() {
        for (int i = 0; i < options.size(); i++) {
            cursor = i;
            if (current_selection == options[i]) break;
        }
    }


    void ArrowSelector::display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) {
        if (!is_in_view) return;

        sf::Color color = is_selected ? (*palette)[2] : (*palette)[1];
        Utilities::Vector scaled_position(position * scale_factor);
        double total_scale_factor = scale_factor * animation_scale_factor;

        renderer.draw_rounded_rectangle(window, scaled_position, total_scale_factor * width, total_scale_factor * height, total_scale_factor * height * 0.3, true, color);
        float text_x_offset = scaled_position.x - total_scale_factor * (width * 0.5 - (height - font_size));
        renderer.draw_text(window, Utilities::Vector(text_x_offset, scaled_position.y), text, font, total_scale_factor * font_size, is_text_centered, (*palette)[0]);

        Utilities::Vector left_arrow_position(scaled_position.x + total_scale_factor * 10, scaled_position.y);
        Utilities::Vector right_arrow_position(scaled_position.x + total_scale_factor * 60, scaled_position.y);
        Utilities::Vector option_position = Utilities::lerp_2D(left_arrow_position, right_arrow_position, 0.5);

        renderer.draw_text(window, left_arrow_position, "<-", font, total_scale_factor * font_size, true, (*palette)[0]);
        renderer.draw_text(window, right_arrow_position, "->", font, total_scale_factor * font_size, true, (*palette)[0]);
        renderer.draw_text(window, option_position, options[cursor], font, total_scale_factor * font_size, true, (*palette)[0]);
    }
}