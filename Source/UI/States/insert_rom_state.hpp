#pragma once


#include "state.hpp"


namespace UI {
    class InsertRomState : public State {
    public:
        InsertRomState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void insert_rom();
        void perform_logic() override;
        void update_roms();
        bool is_a_gb_file(std::string rom_path);
    };
}
