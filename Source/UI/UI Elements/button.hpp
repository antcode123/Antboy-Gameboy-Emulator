#pragma once


#include "ui_element.hpp"


namespace UI {
    class Button : public UIElement {
    public:

        Button(Utilities::Vector _position, float _width, float _height, std::function<void()> _command, bool _is_in_view, bool _is_text_centered, std::string _text, sf::Font& _font);
        void handle_select_event() override;
        void display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) override;
    };
}
