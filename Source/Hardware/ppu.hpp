#pragma once


#include <cstdint>
#include <memory>


typedef unsigned char U8;
typedef unsigned short U16;


namespace Hardware {
    class MMU;
    class LCD;
    class CPU;


    class PPU {
    public:
        enum Mode {H_BLANK, V_BLANK, OAM_SEARCH, PIXEL_TRANSFER};
        MMU& mmu;
        LCD& lcd;
        CPU& cpu;
        int ticks;
        int mode;
        int h_blank_interval;
        int v_blank_interval;
        int oam_search_interval;
        int pixel_transfer_interval;
        bool is_lcd_enabled;
        bool is_window_tile_map_1_selected;
        bool is_window_enabled;
        bool is_unsigned_background_tileset_selected;
        bool is_background_tile_map_1_selected;
        bool are_objects_8x16;
        bool is_background_enabled;
        bool are_objects_enabled;
        bool is_scanline_comparison_enabled;
        bool is_oam_search_stat_interrupt_enabled;
        bool is_v_blank_stat_interrupt_enabled;
        bool is_h_blank_stat_interrupt_enabled;
        bool is_scanline_comparison_equal;
        U8 scroll_x;
        U8 scroll_y;
        U8 scanline_y;
        U8 scanline_y_comparison;
        U8 window_x;
        U8 window_y;
        U8 background_palette;
        U8 object_palette_0;
        U8 object_palette_1;
        std::unique_ptr<U8[]> video_ram;
        std::unique_ptr<U8[]> oam;
        std::unique_ptr<U8[]> current_scanline_buffer;


        PPU(MMU& _mmu, LCD& _lcd, CPU& _cpu);
        void reset();
        U8 read(U16 address);
        void write(U16 address, U8 u8);
        void set_lcd_control(U8 u8);
        U8 get_lcd_control();
        void set_lcd_status(U8 u8);
        U8 get_lcd_status();
        void run(int last_instruction_ticks);
        void run_hblank();
        void run_vblank();
        void run_oam_search();
        void run_pixel_transfer();
        void check_lcd_y_comparison();
        void render_scanline();
        void render_background();
        void render_window();
        void render_objects();
        uint16_t get_tile_location(U8 tile_index);
        U8 get_color_from_id(U8 color_id, U8 palette);
    };
}
