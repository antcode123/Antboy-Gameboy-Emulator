#pragma once


#include <unordered_map>
#include "ui_element.hpp"


namespace UI {
    class BindRemapper : public UIElement {
    public:
        std::string input_type;
        std::string bind_type;
        std::unordered_map<int, std::string> key_map;
        std::unordered_map<int, std::string> controller_map;
        bool is_binding;
        double binding_animation_counter;
        double binding_animation_opacity;
        int bind_code;

        BindRemapper(Utilities::Vector _position, std::string _input_type, std::string _bind_type, int _bind_code, bool _is_in_view, sf::Font& _font);
        void handle_select_event() override;
        void remap_bind(int bind_code);
        void animate(double delta_time) override;
        void display(sf::RenderWindow& window, Utilities::Renderer& renderer, int scale_factor, std::shared_ptr<std::array<sf::Color, 4>> palette) override;
    };
}
