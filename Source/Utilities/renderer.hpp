#pragma once


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include <vector>


namespace Utilities {
    class Renderer {
    public:
        void draw_line(sf::RenderWindow& window, Vector point_a, Vector point_b, int thickness, sf::Color color);
        void draw_rectangle(sf::RenderWindow& window, Vector position, int width, int height, bool is_centered, sf::Color color);
        void draw_rounded_rectangle(sf::RenderWindow& window, Vector position, int width, int height, int border_radius, bool is_centered, sf::Color color);
        void draw_color_grid(sf::RenderWindow& window, Vector position, std::vector<std::vector<sf::Color>>& grid, int scale, bool is_centered);
        void draw_circle(sf::RenderWindow& window, Vector center, int radius, sf::Color color);
        void draw_arc(sf::RenderWindow& window, Vector center, int radius, double start_angle, double end_angle, sf::Color color);
        void draw_regular_polygon(sf::RenderWindow& window, Vector center, int number_of_sides, int radius, sf::Color color);
        void draw_convex_polygon(sf::RenderWindow& window, std::vector<Vector> points, sf::Color color);
        void draw_text(sf::RenderWindow& window, Vector position, std::string text, sf::Font& font, int font_size, bool is_centered, sf::Color color);
        void draw_sprite(sf::RenderWindow& window, Vector position, sf::Texture& texture, double scale, bool is_centered);

    private:
        sf::RectangleShape rectangle;
        sf::CircleShape circle;
        sf::CircleShape regular_polygon;
        sf::ConvexShape convex_polygon;
        sf::Sprite sprite;
        sf::Text text;
    };
}
