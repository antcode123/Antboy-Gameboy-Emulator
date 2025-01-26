#include <algorithm>
#include <filesystem>
#include "insert_rom_state.hpp"


namespace UI {
    InsertRomState::InsertRomState(Gameboy& _gameboy) :
        State(_gameboy, "INSERT ROM") {
        reset();
    }


    void InsertRomState::configure_ui_elements() {
        configure_ui_element_parameters();
        ui_elements.clear();
        std::sort(gameboy.roms.begin(), gameboy.roms.end());

        for (int i = 0; i < gameboy.roms.size(); i++) {
            std::string rom_path = gameboy.roms[i];
            std::string rom_name = Utilities::capitalise(Utilities::get_file_name_from_path(rom_path));
            ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * i), ui_element_width, ui_element_height, [&](){insert_rom();}, ui_elements.size() < 9, true, rom_name, gameboy.font));
        }

        ui_elements.push_back(std::make_unique<Button>(ui_element_offset + Utilities::Vector(0, length_between_ui_elements * ui_elements.size()), ui_element_width, ui_element_height, [&](){return_to_previous_state();}, ui_elements.size() < 9, true, "<- BACK", gameboy.font));
        update_selected_ui_element();
    }


    void InsertRomState::insert_rom() {
        gameboy.insert_rom(gameboy.roms[cursor]);
        enter_new_state(EMULATION);
    }


    void InsertRomState::perform_logic() {
        animate_ui_elements();
        update_scrollbar();
        update_roms();
    }


    void InsertRomState::update_roms() {
        bool are_roms_modified = false;
        std::vector<std::string> updated_roms;

        // Checks for newly inserted ROMs
        for (auto& entry : std::filesystem::directory_iterator(gameboy.exe_path + "\\Assets\\ROMs")) {
            std::string rom_path = entry.path().string();
            if (!is_a_gb_file(rom_path)) continue;
            bool is_rom_in_roms = std::find(gameboy.roms.begin(), gameboy.roms.end(), rom_path) != gameboy.roms.end();
            if (!is_rom_in_roms) are_roms_modified = true;
            updated_roms.push_back(rom_path);
        }

        // Checks for deleted ROMs
        for (std::string& rom_path : gameboy.roms) {
            bool is_rom_in_updated_roms = std::find(updated_roms.begin(), updated_roms.end(), rom_path) != updated_roms.end();

            if (!is_rom_in_updated_roms) {
                are_roms_modified = true;
                break;
            }
        }

        if (are_roms_modified) {
            gameboy.roms = updated_roms;
            configure_ui_elements();
            configure_scrollbar();
            update_cursor_after_ui_element_reconfiguration();
        }
    }


    bool InsertRomState::is_a_gb_file(std::string rom_path) {return Utilities::does_file_contain_extension(rom_path, ".gb");}
}
