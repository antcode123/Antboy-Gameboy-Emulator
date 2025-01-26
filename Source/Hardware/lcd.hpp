#pragma once


#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <array>
#include "../Utilities/vector.hpp"


typedef unsigned char U8;


namespace Hardware {
    class LCD {
    public:
        enum FrameBlendStrength {
            WEAK, MEDIUM, STRONG
        };

        int scale_factor;
        int full_screen_scale_factor;
        int gap_width;
        int gap_height;
        int black_bar_width;
        int black_bar_height;
        Utilities::Vector position;
        int width;
        int height;
        int frame_blend_strength;
        bool is_retro_mode_enabled;
        int total_frame_buffers;
        std::vector<std::array<U8, 23040>> frame_buffers;
        sf::VertexArray frame_buffer_vertices;

        LCD();
        void reset();
        void transfer_pixel(int x, int y, U8 pixel);
        void update_frame_buffers();
        void display(sf::RenderWindow& window, std::shared_ptr<std::array<sf::Color, 4>> palette);
        void scale_pixel(int pixel_index, Utilities::Vector pixel_position, sf::Color pixel_color);
        sf::Color blend_pixel(int location, std::shared_ptr<std::array<sf::Color, 4>> palette);
    };
}
