#include "button.hpp"


namespace UI {
    Button::Button(Utilities::Vector _position, float _width, float _height, std::function<void()> _command, bool _is_in_view, bool _is_text_centered, std::string _text, sf::Font& _font) :
        UIElement(_position, _width, _height, _command, _is_in_view, _is_text_centered, _text, _font) {}


    void Button::handle_select_event() {command();}


    void Button::display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) {
        if (!is_in_view) return;
        sf::Color color = is_selected ? (*palette)[2] : (*palette)[1];
        Utilities::Vector scaled_position(position * scale_factor);
        double total_scale_factor = scale_factor * animation_scale_factor;
        renderer.draw_rounded_rectangle(window, scaled_position, total_scale_factor * width, total_scale_factor * height, total_scale_factor * height * 0.3, true, color);
        float text_x_offset = is_text_centered ? scaled_position.x : scaled_position.x - total_scale_factor * (width * 0.5 - (height - font_size));
        renderer.draw_text(window, Utilities::Vector(text_x_offset, scaled_position.y), text, font, total_scale_factor * font_size, is_text_centered, (*palette)[0]);
    }
}