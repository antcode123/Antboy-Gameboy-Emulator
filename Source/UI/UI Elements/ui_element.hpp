#pragma once


#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <memory>
#include "../../Utilities/renderer.hpp"
#include "../../Utilities/vector.hpp"
#include "../../Utilities/misc.hpp"


namespace UI {
    class UIElement {
    public:
        Utilities::Vector position;
        double animation_scale_factor;
        float width;
        float height;
        float font_size;
        bool is_in_view;
        bool is_selected;
        bool is_pressed;
        bool is_text_centered;
        std::string text;
        sf::Font& font;
        std::function<void()> command;

        UIElement(Utilities::Vector _position, float _width, float _height, std::function<void()> _command, bool _is_in_view, bool _is_text_centered, std::string _text, sf::Font& _font);
        virtual void reset();
        virtual void animate(double delta_time);
        virtual void handle_select_event();
        virtual void handle_left_event();
        virtual void handle_right_event();
        virtual void display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette);
        void scroll();
    };
}