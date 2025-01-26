#include "palette.hpp"


namespace UI {
    Palette::Palette(Utilities::Vector _position, bool _is_in_view, std::function<void()> _command, std::shared_ptr<std::array<sf::Color, 4>> _selected_palette, sf::Font& _font) :
        UIElement(_position, 134, 8, _command, _is_in_view, true, "", _font),
        selected_palette(_selected_palette),
        is_selected_as_new_palette(false),
        is_select_button_selected(true) {

    }


    void Palette::reset() {is_select_button_selected = true;}


    void Palette::handle_select_event() {
        if (is_select_button_selected) is_selected_as_new_palette = true;
        else command();
    }


    void Palette::handle_left_event() {if (!is_select_button_selected) is_select_button_selected = true;}


    void Palette::handle_right_event() {if (is_select_button_selected) is_select_button_selected = false;}


    void Palette::display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) {
        if (!is_in_view) return;
        sf::Color select_button_color = is_select_button_selected ? (*palette)[2] : (*palette)[1];
        sf::Color modify_button_color = is_select_button_selected ? (*palette)[1] : (*palette)[2];
        double total_scale_factor = scale_factor * animation_scale_factor;
        Utilities::Vector scaled_position(position * scale_factor);
        Utilities::Vector select_button_position = scaled_position - Utilities::Vector(total_scale_factor * width * 0.25, 0);
        Utilities::Vector modify_button_position = scaled_position + Utilities::Vector(total_scale_factor * width * 0.25, 0);
        int segment_width = total_scale_factor * 22;
        int segment_height = total_scale_factor * height;

        // Draws the select and modify buttons of the palette
        renderer.draw_rounded_rectangle(window, select_button_position, total_scale_factor * width * 0.5, segment_height, segment_height * 0.3, true, select_button_color);
        renderer.draw_rounded_rectangle(window, modify_button_position, total_scale_factor * width * 0.5, segment_height, segment_height * 0.3, true, modify_button_color);
        renderer.draw_text(window, Utilities::Vector(scaled_position.x - total_scale_factor * (width * 0.5 - 11), scaled_position.y), "SELECT", font, total_scale_factor * font_size, true, (*palette)[0]);
        renderer.draw_text(window, Utilities::Vector(scaled_position.x + total_scale_factor * (width * 0.5 - 12), scaled_position.y), "MODIFY", font, total_scale_factor * font_size, true, (*palette)[0]);


        // // Draws the 4 segments which make up a palette
        Utilities::Vector segment_offset = scaled_position - Utilities::Vector(segment_width * 1.5, 0);
        for (int i = 0; i < 4; i++) renderer.draw_rectangle(window, segment_offset + Utilities::Vector(segment_width * i, 0), segment_width, segment_height, true, (*selected_palette)[i]);

        // Draws the palette outline
        segment_offset = segment_offset - Utilities::Vector(segment_width * 0.5, segment_height * 0.5);
        sf::Color outline_color = is_selected ? (*palette)[2] : (*palette)[1];
        int outline_thickness = 0.4 * total_scale_factor;

        // Top and bottom outlines
        renderer.draw_rectangle(window, segment_offset, segment_width * 4, outline_thickness, false, outline_color);
        renderer.draw_rectangle(window, segment_offset + Utilities::Vector(0, segment_height - outline_thickness), segment_width * 4, outline_thickness, false, outline_color);

        // Left and right segment outlines
        for (int i = 0; i < 5; i++) renderer.draw_rectangle(window, segment_offset + Utilities::Vector(segment_width * i, segment_height * 0.5), outline_thickness * 1.4, segment_height, true, outline_color);
    }
}