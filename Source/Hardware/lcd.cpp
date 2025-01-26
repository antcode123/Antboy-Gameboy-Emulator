#include "lcd.hpp"
#include <iostream>


namespace Hardware {

    // Displays the pixels onto the screen. Stores a history of frame buffers for retro ghosting effect
    LCD::LCD() :
        width(160),
        height(144),
        frame_buffers(std::vector<std::array<U8, 23040>>(0)),
        frame_buffer_vertices(sf::Quads, width * height * 4) {
    }


    void LCD::reset() {
        if (!is_retro_mode_enabled) total_frame_buffers = 2;
        else if (frame_blend_strength == WEAK) total_frame_buffers = 3;
        else if (frame_blend_strength == MEDIUM) total_frame_buffers = 5;
        else if (frame_blend_strength == STRONG) total_frame_buffers = 7;
        frame_buffers.resize(total_frame_buffers);
        for (auto& frame_buffer : frame_buffers) std::fill(frame_buffer.begin(), frame_buffer.end(), 0);
    }


    void LCD::transfer_pixel(int x, int y, U8 pixel) {frame_buffers.back()[y * width + x] = pixel;}


    void LCD::update_frame_buffers() {
        frame_buffers.erase(frame_buffers.begin()); // Pops the oldest frame from the front
        frame_buffers.emplace_back(); // Pushes a new frame onto the back
    }


    void LCD::display(sf::RenderWindow& window, std::shared_ptr<std::array<sf::Color, 4>> palette) {
        int pixel_index = 0;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int pixel_location = i * width + j;
                sf::Color pixel_color;
                if (is_retro_mode_enabled) pixel_color = blend_pixel(pixel_location, palette);
                else pixel_color = (*palette)[frame_buffers[frame_buffers.size() - 2][pixel_location]]; // Uses second to last frame buffer as last frame buffer is currently being processed by PPU
                Utilities::Vector pixel_position = (position + Utilities::Vector(j, i)) * scale_factor;
                scale_pixel(pixel_index, pixel_position, pixel_color);
                pixel_index += 4;
            }
        }

        window.draw(frame_buffer_vertices); // Using a single draw call with an sf::VertexArray for optimisation
    }


    void LCD::scale_pixel(int pixel_index, Utilities::Vector pixel_position, sf::Color pixel_color) {
        double new_scale_factor = is_retro_mode_enabled ? scale_factor - 1 : scale_factor;
        float x_offset = pixel_position.x;
        float y_offset = pixel_position.y;

        if (is_retro_mode_enabled) {
            x_offset++; // Allows for vertical gridlines
            y_offset++; // Allows for horizontal gridlines
        }

        // Draws the scaled pixel to the frame buffer's vertex array
        frame_buffer_vertices[pixel_index + 0] = sf::Vertex(sf::Vector2f(x_offset, y_offset), pixel_color);
        frame_buffer_vertices[pixel_index + 1] = sf::Vertex(sf::Vector2f(x_offset + new_scale_factor, y_offset), pixel_color);
        frame_buffer_vertices[pixel_index + 2] = sf::Vertex(sf::Vector2f(x_offset + new_scale_factor, y_offset + new_scale_factor), pixel_color);
        frame_buffer_vertices[pixel_index + 3] = sf::Vertex(sf::Vector2f(x_offset, y_offset + new_scale_factor), pixel_color);
    }


    sf::Color LCD::blend_pixel(int pixel_location, std::shared_ptr<std::array<sf::Color, 4>> palette) {
        int blended_r = 0;
        int blended_g = 0;
        int blended_b = 0;
        U8 pixel;

        for (int i = 0; i < total_frame_buffers - 1; i++) {
            pixel = frame_buffers[i][pixel_location];
            blended_r += (*palette)[pixel].r;
            blended_g += (*palette)[pixel].g;
            blended_b += (*palette)[pixel].b;
        }

        // Takes an average
        // Divides each color component by total frame buffers - 1 because total frame buffers displayed is 1 less than total
        blended_r /= (total_frame_buffers - 1);
        blended_g /= (total_frame_buffers - 1);
        blended_b /= (total_frame_buffers - 1);
        return sf::Color(blended_r, blended_g, blended_b);
    }
}
