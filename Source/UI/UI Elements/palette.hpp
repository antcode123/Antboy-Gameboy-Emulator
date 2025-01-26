#pragma once


#include "ui_element.hpp"


namespace UI {
    class Palette : public UIElement {
    public:
        std::shared_ptr<std::array<sf::Color, 4>> selected_palette;
        bool is_selected_as_new_palette;
        bool is_select_button_selected;

        Palette(Utilities::Vector _position, bool _is_in_view, std::function<void()> _command, std::shared_ptr<std::array<sf::Color, 4>> _selected_palette, sf::Font& _font);
        void reset() override;
        void handle_select_event() override;
        void handle_left_event() override;
        void handle_right_event() override;
        void display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) override;
    };
}
