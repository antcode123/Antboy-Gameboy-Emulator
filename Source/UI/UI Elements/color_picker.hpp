#pragma once


#include <SFML/Graphics.hpp>
#include <vector>
#include "ui_element.hpp"


namespace UI {
    class ColorPicker : public UIElement{
    public:
        std::vector<std::vector<sf::Color>> color_grid;
        std::vector<sf::Color> colors;
        int horizontal_resolution;
        int vertical_resolution;
        int grid_width;
        int grid_height;
        Utilities::Vector cursor;
        bool is_enabled;


        ColorPicker(Utilities::Vector _position, sf::Font& _font);
        void generate_color_grid();
        void handle_left_event() override;
        void handle_right_event() override;
        void handle_up_event();
        void handle_down_event();
        void display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) override;
    };
}