#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include "renderer.hpp"
#include "vector.hpp"
#include <iostream>


namespace Utilities {
    void Renderer::draw_line(sf::RenderWindow& window, Vector point_a, Vector point_b, int thickness, sf::Color color) {
        Vector displacement = point_b - point_a;
        Vector midpoint = (point_a + point_b) / 2;
        double length = displacement.get_magnitude();
        double angle_of_rotation = std::atan(displacement.get_slope()) * 180 / M_PI;
        rectangle.setSize(sf::Vector2f(length, thickness));
        rectangle.setOrigin(length / 2, 1);
        rectangle.setPosition(midpoint.x, midpoint.y);
        rectangle.setRotation(0);
        rectangle.rotate(angle_of_rotation);
        rectangle.setFillColor(color);
        window.draw(rectangle);
    }


    void Renderer::draw_rectangle(sf::RenderWindow& window, Vector position, int width, int height, bool is_centered, sf::Color color) {
        rectangle.setPosition(sf::Vector2f(position.x, position.y));
        rectangle.setRotation(0);
        rectangle.setSize(sf::Vector2f(width, height));
        rectangle.setFillColor(color);
        if (is_centered) rectangle.setOrigin(sf::Vector2f(width * 0.5, height * 0.5));
        else rectangle.setOrigin(sf::Vector2f(0, 0));
        window.draw(rectangle);
    }


    void Renderer::draw_rounded_rectangle(sf::RenderWindow& window, Vector position, int width, int height, int border_radius, bool is_centered, sf::Color color) {
        if (is_centered) {
            position.x -= width * 0.5;
            position.y -= height * 0.5;
        }

        Utilities::Vector rectangle_position_1(position.x + border_radius, position.y);
        Utilities::Vector rectangle_position_2(position.x, position.y + border_radius);
        int rectangle_width_1 = width - 2 * border_radius;
        int rectangle_width_2 = width;
        int rectangle_height_1 = height;
        int rectangle_height_2 = height - 2 * border_radius;
        Utilities::Vector circle_position_1(position.x + border_radius, position.y + border_radius);
        Utilities::Vector circle_position_2(position.x + width - border_radius, position.y + border_radius);
        Utilities::Vector circle_position_3(position.x + border_radius, position.y + height - border_radius);
        Utilities::Vector circle_position_4(position.x + width - border_radius, position.y + height - border_radius);
        draw_rectangle(window, rectangle_position_1, rectangle_width_1, rectangle_height_1, false, color);
        draw_rectangle(window, rectangle_position_2, rectangle_width_2, rectangle_height_2, false, color);
        draw_circle(window, circle_position_1, border_radius, color);
        draw_circle(window, circle_position_2, border_radius, color);
        draw_circle(window, circle_position_3, border_radius, color);
        draw_circle(window, circle_position_4, border_radius, color);
    }


    void Renderer::draw_color_grid(sf::RenderWindow& window, Vector position, std::vector<std::vector<sf::Color>>& grid, int scale, bool is_centered) {
        if (is_centered) position = position - Utilities::Vector(grid[0].size() * scale * 0.5, grid.size() * scale * 0.5);

        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[i].size(); j++) {draw_rectangle(window, position + Vector(j, i) * scale, scale, scale, false, grid[i][j]);}
        }
    }


    void Renderer::draw_circle(sf::RenderWindow& window, Vector center, int radius, sf::Color color) {
        circle.setOrigin(sf::Vector2f(radius, radius));
        circle.setPosition(sf::Vector2f(center.x, center.y));
        circle.setRadius(radius);
        circle.setFillColor(color);
        circle.setPointCount(0.25 * 6.28318 * radius);
        window.draw(circle);
    }


    void Renderer::draw_arc(sf::RenderWindow& window, Vector center, int radius, double start_angle, double end_angle, sf::Color color) {
        double tau = 2 * M_PI;
        int number_of_points_in_full_circle = 60;
        int number_of_points = number_of_points_in_full_circle * (end_angle - start_angle) / tau;
        double delta_theta = tau / number_of_points_in_full_circle;
        Vector rotation_vector = Vector(radius, 0);
        std::vector<Vector> points;

        for (int i = 0; i < number_of_points; i++) {
            Vector arc_point = center + rotation_vector.rotate(start_angle + i * delta_theta);
            points.push_back(arc_point);
        }

        for (int i = 0; i < number_of_points - 1; i++) {draw_line(window, points[i], points[i + 1], 2, color);}
    }


    void Renderer::draw_regular_polygon(sf::RenderWindow& window, Vector center, int number_of_sides, int radius, sf::Color color) {
        regular_polygon.setOrigin(sf::Vector2f(radius, radius));
        regular_polygon.setPointCount(number_of_sides);
        regular_polygon.setPosition(sf::Vector2f(center.x, center.y));
        regular_polygon.setRadius(radius);
        regular_polygon.setFillColor(color);
        window.draw(regular_polygon);
    }


    void Renderer::draw_convex_polygon(sf::RenderWindow& window, std::vector<Vector> points, sf::Color color) {
        int number_of_points = points.size();
        convex_polygon.setPointCount(number_of_points);
        for (int i = 0; i < number_of_points; i++) {convex_polygon.setPoint(i, sf::Vector2f(points[i].x, points[i].y));}
        convex_polygon.setFillColor(color);
        window.draw(convex_polygon);
    }


    void Renderer::draw_text(sf::RenderWindow& window, Vector position, std::string text_string, sf::Font& font, int font_size, bool is_centered, sf::Color color) {
        text.setFont(font);
        text.setCharacterSize(font_size);
        text.setString(text_string);
        text.setPosition(sf::Vector2f(position.x, position.y));
        text.setFillColor(color);
        sf::FloatRect text_bounds = text.getLocalBounds();
        double x_offset = is_centered ? text_bounds.width * 0.5 : 0;
        text.setOrigin(sf::Vector2f(x_offset, text_bounds.height * 0.6));
        window.draw(text);
    }

    void Renderer::draw_sprite(sf::RenderWindow& window, Vector position, sf::Texture& texture, double scale, bool is_centered) {
        sprite.setTexture(texture);
        sprite.setScale(sf::Vector2f(scale, scale));
        sprite.setPosition(sf::Vector2f(position.x, position.y));
        sf::FloatRect text_bounds = sprite.getLocalBounds();
        double x_offset = is_centered ? text_bounds.width / 2 : 0;
        double y_offset = is_centered ? text_bounds.height / 2 : 0;
        sprite.setOrigin(sf::Vector2f(x_offset, y_offset));
        window.draw(sprite);
    }
}
