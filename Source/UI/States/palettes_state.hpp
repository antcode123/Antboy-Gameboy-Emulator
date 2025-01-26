#pragma once


#include "state.hpp"


namespace UI {
    class PalettesState : public State {
    public:

        PalettesState(Gameboy& _gameboy);
        void configure_ui_elements() override;
        void perform_logic() override;
        void update_selected_palette();
        void customise_palette_segment();
        void add_palette();
    };
}