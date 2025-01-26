#include "ui_element.hpp"


namespace UI {
    UIElement::UIElement(Utilities::Vector _position, float _width, float _height, std::function<void()> _command, bool _is_in_view, bool _is_text_centered, std::string _text, sf::Font& _font) :
        animation_scale_factor(1),
        position(_position),
        width(_width),
        height(_height),
        is_in_view(_is_in_view),
        is_text_centered(_is_text_centered),
        text(_text),
        font(_font),
        font_size(height * 0.4),
        command(_command),
        is_selected(false),
        is_pressed(false) {
    }


    void UIElement::reset() {}


    void UIElement::handle_select_event() {}


    void UIElement::handle_left_event() {}


    void UIElement::handle_right_event() {}


    void UIElement::animate(double delta_time) {
        double animation_rate = delta_time * 0.75;
        animation_scale_factor = is_selected ? std::min(animation_scale_factor + animation_rate, 1.08) : std::max(animation_scale_factor - animation_rate, 1.0);
    }


    void UIElement::display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) {}


    void UIElement::scroll() {}
}