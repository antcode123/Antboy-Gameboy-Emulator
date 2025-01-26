#pragma once


#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include "../../gameboy.hpp"
#include "../UI Elements/ui_element.hpp"
#include "../UI Elements/button.hpp"
#include "../UI Elements/arrow_selector.hpp"
#include "../../Utilities/renderer.hpp"
#include "../../Utilities/vector.hpp"
#include "../../Utilities/misc.hpp"


namespace UI {
    class State {
    public:
        enum StateType {
            MAIN_MENU, INSERT_ROM, EMULATION, PAUSED, SETTINGS, GENERAL_SETTINGS, DISPLAY_SETTINGS,
            PALETTES, MODIFY_PALETTE, COLOR_PICKER, CONTROLS, INPUT_SELECT, REMAP_BINDS
        };

        Gameboy& gameboy;
        bool is_screen_fading;
        int transition_state;
        bool has_entered_new_state;
        bool is_running;
        std::string name;
        std::vector<std::unique_ptr<UIElement>> ui_elements;
        sf::Color screen_fade_color;
        double screen_fade_opacity;
        int menu_type;
        int cursor;
        int length_between_ui_elements;
        int ui_element_width;
        int ui_element_font_size;
        int ui_element_height;
        int scrollbar_height;
        int scrollbar_width;
        float scrollbar_size;
        Utilities::Vector scrollbar_start;
        Utilities::Vector scrollbar_end;
        Utilities::Vector scrollbar_position;
        Utilities::Vector ui_element_offset;

        State(Gameboy& _gameboy, std::string _name);
        virtual void reset();
        virtual void configure_ui_elements();
        virtual void configure_ui_element_parameters();
        void configure_scrollbar();
        void handle_events();
        void exit_app();
        void jump_to_last_button();
        virtual void handle_keyboard_events();
        virtual void handle_controller_events();
        virtual void perform_logic();
        void animate_ui_elements();
        virtual void render();
        virtual void display_heading();
        void display_fps();
        void display_ui_elements();
        void display_scrollbar();
        virtual void display_miscellaneous();
        virtual void update_cursor(int scroll_direction);
        void update_cursor_after_ui_element_reconfiguration();
        void scroll_ui_elements(int direction);
        void update_scrollbar();
        void update_selected_ui_element();
        void enter_new_state(int state_type);
        void fade_in();
        void fade_out();
        void apply_screen_fade();
        void return_to_previous_state();
    };
}