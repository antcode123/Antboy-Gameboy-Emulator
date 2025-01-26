#pragma once


#include "ui_element.hpp"


namespace UI {
    class ArrowSelector : public UIElement {
    public:
        std::vector<std::string> options;
        std::string current_selection;
        int cursor;

        ArrowSelector(Utilities::Vector _position, bool _is_in_view, std::string _text, std::vector<std::string> _options, std::string _current_selection, sf::Font& _font);
        void handle_left_event() override;
        void handle_right_event() override;
        void handle_select_event() override;
        void update_cursor();
        void display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) override;
    };
}
