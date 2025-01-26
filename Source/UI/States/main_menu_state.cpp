#include <SFML/Graphics.hpp>
#include "main_menu_state.hpp"


namespace UI {

    // Class responsible for the main menu
    MainMenuState::MainMenuState(Gameboy& _gameboy) :
        State(_gameboy, "MAIN MENU") {
        reset();
    }


    void MainMenuState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 0), ui_element_width, ui_element_height, [&](){enter_new_state(INSERT_ROM);}, true, true, "INSERT ROM", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 1), ui_element_width, ui_element_height, [&](){enter_new_state(SETTINGS);}, true, true, "SETTINGS", gameboy.font));
        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * 2), ui_element_width, ui_element_height, [&](){exit_app();}, true, true, "EXIT", gameboy.font));
    }


    void MainMenuState::configure_ui_element_parameters() {
        length_between_ui_elements =  14;
        ui_element_offset = gameboy.lcd.position + Utilities::Vector(gameboy.lcd.width * 0.5, 84);
        ui_element_width = 55;
        ui_element_height = 10;
    }


    void MainMenuState::display_heading() {
        gameboy.renderer.draw_text(gameboy.window, (gameboy.lcd.position + Utilities::Vector(gameboy.lcd.width * 0.5, 40)) * gameboy.lcd.scale_factor, "ANTBOY", gameboy.font, gameboy.lcd.scale_factor * 29, true, (*gameboy.selected_palette)[0]);
        gameboy.renderer.draw_text(gameboy.window, (gameboy.lcd.position + Utilities::Vector(gameboy.lcd.width * 0.5, 65)) * gameboy.lcd.scale_factor, "RELIVE THE GAMEBOY'S CLASSICS", gameboy.font, gameboy.lcd.scale_factor * 3.6, true, (*gameboy.selected_palette)[0]);


    }
}
