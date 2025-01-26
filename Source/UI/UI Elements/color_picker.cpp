#include <cmath>
#include "color_picker.hpp"
#include "../../Utilities/misc.hpp"


namespace UI {
    ColorPicker::ColorPicker(Utilities::Vector _position, sf::Font& _font) :
        UIElement(_position, 20, 10, nullptr, true, true, "", _font),
        horizontal_resolution(5),
        vertical_resolution(6),
        is_enabled(true),
        cursor(Utilities::Vector(0, 0)) {
        generate_color_grid();
    }


    void ColorPicker::generate_color_grid() {
        colors.clear();
        colors.push_back(sf::Color(255, 0, 0));
        colors.push_back(sf::Color(252, 144, 3));
        colors.push_back(sf::Color(255, 255, 0));
        colors.push_back(sf::Color(0, 255, 0));
        colors.push_back(sf::Color(0, 0, 255));
        colors.push_back(sf::Color(255, 0, 255));
        grid_width = horizontal_resolution * (colors.size() - 1) + 1;
        grid_height = vertical_resolution * 2 + 1;
        color_grid.resize(grid_height, std::vector<sf::Color>(grid_width, sf::Color::Black));

        for (int i = 0; i < grid_height; i++) {
            for (int j = 0; j < grid_width; j++) {

                // Lerps between two adjacent colors
                int initial_color_pointer = j / horizontal_resolution;
                int target_color_pointer = (initial_color_pointer + 1) == colors.size() ? initial_color_pointer : initial_color_pointer + 1;
                sf::Color initial_color = colors[initial_color_pointer];
                sf::Color target_color = colors[target_color_pointer];
                double t = (double)(j % horizontal_resolution) / horizontal_resolution;
                sf::Color lerped_color = Utilities::lerp_rgb(initial_color, target_color, t);

                // Adjusts the brightness of the lerped color by lerping between white and black
                t = (double)(i % vertical_resolution) / vertical_resolution;
                if (i < vertical_resolution) lerped_color = Utilities::lerp_rgb(sf::Color::White, lerped_color, t);
                else if (i < vertical_resolution * 2) lerped_color = Utilities::lerp_rgb(lerped_color, sf::Color::Black, t);
                else lerped_color = sf::Color::Black;
                color_grid[i][j] = lerped_color;
            }
        }
    }


    void ColorPicker::handle_left_event() {cursor.x = (int)(cursor.x + grid_width - 1) % grid_width;} // Moves cursor to the left and wraps once it reaches the left edge


    void ColorPicker::handle_right_event() {cursor.x = (int)(cursor.x + 1) % grid_width;} // Moves cursor to the right and wraps once it reaches the right edge


    void ColorPicker::handle_up_event() {cursor.y = (int)(cursor.y + grid_height - 1) % grid_height;} // Moves cursor up and wraps once it reaches the top edge


    void ColorPicker::handle_down_event() {cursor.y = (int)(cursor.y + 1) % grid_height;} // Moves cursor down and wraps once it reaches the bottom edge


    void ColorPicker::display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) {
        sf::Color outline_color = is_selected ? (*palette)[2] : (*palette)[1];
        float segment_size = 4;
        int scaled_width = scale_factor * grid_width * segment_size;
        int scaled_height = scale_factor * grid_height * segment_size;
        Utilities::Vector scaled_position = position * (int)scale_factor;

        // Draws color grid
        renderer.draw_rectangle(window, scaled_position, scaled_width * 1.01, scaled_height * 1.025, true, outline_color);
        renderer.draw_color_grid(window, scaled_position, color_grid, scale_factor * segment_size, true);

        // Draws cursor
        if (is_enabled) {
            Utilities::Vector cursor_offset = scaled_position - Utilities::Vector(scaled_width * 0.5, scaled_height * 0.5);
            Utilities::Vector cursor_position = cursor_offset + cursor * scale_factor * segment_size;
            float cursor_size = scale_factor * segment_size;
            renderer.draw_rectangle(window, cursor_position - Utilities::Vector(0.1, 0.1) * cursor_size, cursor_size * 1.2, cursor_size * 1.2, false, (*palette)[1]);
            renderer.draw_rectangle(window, cursor_position + Utilities::Vector(0.025, 0.025) * cursor_size, cursor_size * 0.95, cursor_size * 0.95, false, color_grid[cursor.y][cursor.x]);
        }
    }
}