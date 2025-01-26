#include <cstdint>
#include <cstring>
#include "ppu.hpp"
#include "mmu.hpp"
#include "lcd.hpp"
#include "cpu.hpp"
#include "../Utilities/misc.hpp"


namespace Hardware {

    // Processes tiles from VRAM and OAM and renders scanlines which are transfered to the LCD
    // This implementation of the PPU doesn't use pixel FIFO so isn't cycle accurate.
    // However it can display graphics accurately for most ROMs.
    PPU::PPU(MMU& _mmu, LCD& _lcd, CPU& _cpu) :
        mmu(_mmu),
        lcd(_lcd),
        cpu(_cpu),
        video_ram(std::make_unique<U8[]>(8192)),
        oam(std::make_unique<U8[]>(160)),
        h_blank_interval(204),
        v_blank_interval(4560),
        oam_search_interval(80),
        pixel_transfer_interval(172),
        current_scanline_buffer(std::make_unique<U8[]>(lcd.width)) {
    }


    void PPU::reset() {
        set_lcd_control(0x91);
        ticks = 0;
        scanline_y = 0;
        scroll_x = 0;
        scroll_y = 0;
        mode = H_BLANK;
        std::memset(video_ram.get(), 0, 8192);
        std::memset(oam.get(), 0, 160);
    }


    U8 PPU::read(U16 address) {
        switch (address) {
            case 0xFF40: return get_lcd_control();
            case 0xFF41: return get_lcd_status();
            case 0xFF42: return scroll_y;
            case 0xFF43: return scroll_x;
            case 0xFF44: return scanline_y;
            case 0xFF45: return scanline_y_comparison;
            case 0xFF47: return background_palette;
            case 0xFF48: return object_palette_0;
            case 0xFF49: return object_palette_1;
            case 0xFF4A: return window_y;
            case 0xFF4B: return window_x;
            default: return 0xFF;
        }
    }


    void PPU::write(U16 address, U8 u8) {
        switch (address) {
            case 0xFF40: set_lcd_control(u8); break;
            case 0xFF41: set_lcd_status(u8); break;
            case 0xFF42: scroll_y = u8; break;
            case 0xFF43: scroll_x = u8; break;
            case 0xFF45: scanline_y_comparison = u8; break;
            case 0xFF47: background_palette = u8; break;
            case 0xFF48: object_palette_0 = u8; break;
            case 0xFF49: object_palette_1 = u8; break;
            case 0xFF4A: window_y = u8; break;
            case 0xFF4B: window_x = u8; break;
        }
    }


    void PPU::set_lcd_control(U8 u8) {
        is_lcd_enabled = Utilities::get_bit_u8(u8, 7);
        is_window_tile_map_1_selected = Utilities::get_bit_u8(u8, 6);
        is_window_enabled = Utilities::get_bit_u8(u8, 5);
        is_unsigned_background_tileset_selected = Utilities::get_bit_u8(u8, 4);
        is_background_tile_map_1_selected = Utilities::get_bit_u8(u8, 3);
        are_objects_8x16 = Utilities::get_bit_u8(u8, 2);
        are_objects_enabled = Utilities::get_bit_u8(u8, 1);
        is_background_enabled = Utilities::get_bit_u8(u8, 0);

        // Resets the PPU and LCD when the lcd is disabled
        if (!is_lcd_enabled) {
            ticks = 0;
            scanline_y = 0;
            mode = H_BLANK;
            lcd.reset();
        }
    }


    U8 PPU::get_lcd_control() {
        U8 lcd_control = 0;
        Utilities::set_bit_u8(lcd_control, 7, is_lcd_enabled);
        Utilities::set_bit_u8(lcd_control, 6, is_window_tile_map_1_selected);
        Utilities::set_bit_u8(lcd_control, 5, is_window_enabled);
        Utilities::set_bit_u8(lcd_control, 4, is_unsigned_background_tileset_selected);
        Utilities::set_bit_u8(lcd_control, 3, is_background_tile_map_1_selected);
        Utilities::set_bit_u8(lcd_control, 2, are_objects_8x16);
        Utilities::set_bit_u8(lcd_control, 1, are_objects_enabled);
        Utilities::set_bit_u8(lcd_control, 0, is_background_enabled);
        return lcd_control;
    }


    void PPU::set_lcd_status(U8 u8) {
        is_scanline_comparison_enabled = Utilities::get_bit_u8(u8, 6);
        is_oam_search_stat_interrupt_enabled = Utilities::get_bit_u8(u8, 5);
        is_v_blank_stat_interrupt_enabled = Utilities::get_bit_u8(u8, 4);
        is_h_blank_stat_interrupt_enabled = Utilities::get_bit_u8(u8, 3);
    }


    U8 PPU::get_lcd_status() {
        U8 lcd_status = 0;
        Utilities::set_bit_u8(lcd_status, 6, is_scanline_comparison_enabled);
        Utilities::set_bit_u8(lcd_status, 5, is_oam_search_stat_interrupt_enabled);
        Utilities::set_bit_u8(lcd_status, 4, is_v_blank_stat_interrupt_enabled);
        Utilities::set_bit_u8(lcd_status, 3, is_h_blank_stat_interrupt_enabled);
        Utilities::set_bit_u8(lcd_status, 2, is_scanline_comparison_equal);
        lcd_status |= (U8)mode;
        return lcd_status;
    }


    void PPU::run(int last_instruction_ticks) {
        ticks += last_instruction_ticks;
        if (!is_lcd_enabled) return; // Halts the PPU if the LCD is disabled

        // Switches through modes 0-4
        switch (mode) {
            case H_BLANK: run_hblank(); break;
            case V_BLANK: run_vblank(); break;
            case OAM_SEARCH: run_oam_search(); break;
            case PIXEL_TRANSFER: run_pixel_transfer(); break;
        }
    }


    void PPU::run_hblank() {
        if (ticks < h_blank_interval) return;

        // PPU finishing H-blank and switching to either OAM search mode or V-blank mode
        ticks -= h_blank_interval;
        scanline_y += 1;
        check_lcd_y_comparison();

        // PPU at the end of a frame and switching to V-blank mode
        if (scanline_y == 144) {
            cpu.set_interrupt(0, true);
            mode = V_BLANK;
            if (is_v_blank_stat_interrupt_enabled) cpu.set_interrupt(1, true);
            return;
        }

        // PPU switching to OAM search mode - start of the next scanline
        mode = OAM_SEARCH;
        if (is_oam_search_stat_interrupt_enabled) cpu.set_interrupt(1, true);
    }


    void PPU::run_vblank() {
        if (ticks < v_blank_interval / 10) return;

        // PPU finishing V-blank and switching to the next V-blank scanline
        ticks -= v_blank_interval / 10;
        scanline_y += 1;
        check_lcd_y_comparison();
        if (scanline_y < 153) return;

        // PPU finishing V-blank and switching OAM search mode - start of next frame
        mode = OAM_SEARCH;
        scanline_y = 0;
        lcd.update_frame_buffers();
        if (is_v_blank_stat_interrupt_enabled) cpu.set_interrupt(1, true);
    }


    void PPU::run_oam_search() {
        // Nothing needs to actually happen in OAM search mode (other than timing) to have accurate graphics for most ROMS
        // Something which I could implement here is object orderin so that they are rendered on top of one another in the correct order
        // However this isn't important to implement

        if (ticks < oam_search_interval) return;

        // PPU finishing OAM search mode and switching to pixel transfer mode
        ticks -= oam_search_interval;
        mode = PIXEL_TRANSFER;
    }



    void PPU::run_pixel_transfer() {

        if (ticks < pixel_transfer_interval) return;

        // PPU finished pixel transfer mode, renders the current scanline and switches to H-blank mode
        render_scanline();
        ticks -= pixel_transfer_interval;
        mode = H_BLANK;
        if (is_h_blank_stat_interrupt_enabled) cpu.set_interrupt(1, true);
    }


    void PPU::check_lcd_y_comparison() {
        if (scanline_y == scanline_y_comparison) {
            is_scanline_comparison_equal = true;
            if (is_scanline_comparison_enabled) cpu.set_interrupt(1, true);
        }

        else is_scanline_comparison_equal = false;
    }


    U16 PPU::get_tile_location(U8 tile_index) {

        // Unsigned tile addressing from tileset 2
        if (is_unsigned_background_tileset_selected) return 0x8000 + tile_index * 16;

        // Adding 128 to the unsigned 8-bit tile index and then adding the scaled result to pointer 0x8800
        // This essential works the same way as adding the signed equivalent of the scaled tile index to the pointer 0x9000
        tile_index += 128;
        return 0x8800 + tile_index * 16;

    }


    U8 PPU::get_color_from_id(U8 color_id, U8 palette) {
        switch (color_id) {
            case 0: return palette & 0b00000011;
            case 1: return (palette & 0b00001100) >> 2;
            case 2: return (palette & 0b00110000) >> 4;
            case 3: return (palette & 0b11000000) >> 6;
            default: return 0;
        }
    }


    void PPU::render_scanline() {
        if (is_background_enabled) render_background();
        if (is_window_enabled) render_window();
        if (are_objects_enabled) render_objects();
    }


    void PPU::render_background() {
        U16 tile_map_offset = is_background_tile_map_1_selected ? 0x9C00 : 0x9800; // The tile map offset to use for this scanline
        U8 background_y = scanline_y + scroll_y; // The y position of the current scanline pixel within the background map
        U8 tile_row = background_y >> 3; // The row of the tile, that the current pixel is in, within the background map


        // Rendering each pixel in the scanline
        for (U8 scanline_x = 0; scanline_x < 160; scanline_x++) {
            U8 background_x = scanline_x + scroll_x; // The x position of the current scanline pixel within the background map
            U8 tile_col = background_x >> 3; // The column of the tile, that the current pixel is in, within the background map
            U8 tile_index = mmu.read_u8(tile_map_offset + tile_row * 32 + tile_col); // A pointer to the location of the tile in the selected tileset
            U16 tile_line = mmu.read_u16(get_tile_location(tile_index) + (background_y % 8) * 2); // The row of pixels within the tile which the current pixel is locate in
            U8 tile_line_high_byte = tile_line >> 8; // The high byte of the row of pixels - the byte containing the left bit of the pixels in the tile
            U8 tile_line_low_byte = tile_line & 0xFF; // The low byte of the row of pixels - the byte containing the right bit of the pixels in the tile
            U8 pixel_bit = 7 - (background_x % 8); // The bit position the current pixel is located within the pixel row
            U8 pixel_bit_mask = 1 << pixel_bit; // The mask which extracts the left and right bits of the current pixel from the pixel row

            // Getting the left and right bits of the pixels within the pixel row
            U8 pixel_bit_high = ((tile_line_high_byte & pixel_bit_mask) >> pixel_bit) << 1;
            U8 pixel_bit_low = (tile_line_low_byte & pixel_bit_mask) >> pixel_bit;

            // The current pixel variable actual stores the pixel id which is used by the background palette to obtain a pixel color value
            // Here the left and right pixel bits are combined to form the color id
            // This is used to get the color of the actual pixel to be pushed to the LCD
            U8 color_id = pixel_bit_high | pixel_bit_low;
            U8 color = get_color_from_id(color_id, background_palette);
            lcd.transfer_pixel(scanline_x, scanline_y, color);
            current_scanline_buffer[scanline_x] = color_id;
        }

    }


    void PPU::render_window() {
        if (scanline_y < window_y) return; // Ignoring scanlines which don't intersect the window
        window_x -= 7; // The x position of the window within the LCD
        U16 tile_map_offset = is_window_tile_map_1_selected ? 0x9C00 : 0x9800;
        U8 background_y = (scanline_y - window_y);
        U8 tile_row = background_y >> 3;

        for (int i = 0; i < 160 - window_x; i++) {
            U8 tile_col = i >> 3;
            U8 tile_index = mmu.read_u8(tile_map_offset + tile_row * 32 + tile_col);
            U16 tile_line = mmu.read_u16(get_tile_location(tile_index) + (background_y % 8) * 2);
            U8 tile_line_high_byte = tile_line >> 8;
            U8 tile_line_low_byte = tile_line & 0xFF;
            U8 pixel_bit = 7 - (i % 8);
            U8 pixel_bit_mask = 1 << pixel_bit;
            U8 pixel_bit_high = ((tile_line_high_byte & pixel_bit_mask) >> pixel_bit) << 1;
            U8 pixel_bit_low = (tile_line_low_byte & pixel_bit_mask) >> pixel_bit;
            U8 color_id = pixel_bit_high | pixel_bit_low;
            U8 color = get_color_from_id(color_id, background_palette);
            lcd.transfer_pixel(window_x + i, scanline_y, color);
        }

        window_x += 7;
    }


    void PPU::render_objects() {
        for (int i = 0; i < 40; i++) {

            // The attributes of the current object
            U8 object_y = oam[i * 4];
            U8 object_x = oam[i * 4 + 1];
            U8 object_index = oam[i * 4 + 2];
            U8 object_attributes = oam[i * 4 + 3];
            U8 object_height = are_objects_8x16 ? 16 : 8;

            if (!(scanline_y + 16 >= object_y && scanline_y + 16 < object_y + object_height)) continue; // Ignores objects which don't intersect the current scanline
            object_y -= 16; // 16 is subtracted as the object y actually stores the objects y position + 16
            U8 tile_pixel_y = scanline_y - object_y; // The y position of the scanline of pixels within the object's tile
            if (Utilities::get_bit_u8(object_attributes, 6)) tile_pixel_y = object_height - tile_pixel_y - 1; // Checks if the object is flipped vertically
            U16 tile_line = mmu.read_u16(0x8000 + object_index * 16 + tile_pixel_y * 2);
            U8 tile_line_high_byte = tile_line >> 8;
            U8 tile_line_low_byte = tile_line & 0xFF;

            // Rendering each pixel of the object within the scanline
            for (int tile_pixel_x = 0; tile_pixel_x < 8; tile_pixel_x++) {
                U8 scanline_x = object_x + tile_pixel_x;
                if (!(scanline_x >= 8 && scanline_x < lcd.width + 8)) continue; // Ignores pixels outside the lcd bounds
                scanline_x -= 8; // 8 is subtracted as the object x actually stores the objects x position + 8
                U8 pixel_bit = Utilities::get_bit_u8(object_attributes, 5) ? tile_pixel_x : 7 - tile_pixel_x;
                U8 pixel_bit_mask = 1 << pixel_bit;
                U8 pixel_bit_high = ((tile_line_high_byte & pixel_bit_mask) >> pixel_bit) << 1;
                U8 pixel_bit_low = (tile_line_low_byte & pixel_bit_mask) >> pixel_bit;
                U8 color_id = pixel_bit_high | pixel_bit_low;
                U8 palette = Utilities::get_bit_u8(object_attributes, 4) ? object_palette_1 : object_palette_0;
                U8 color = get_color_from_id(color_id, palette);
                if (color_id == 0) continue; // Ignores transparent pixels
                if (!Utilities::get_bit_u8(object_attributes, 7)) lcd.transfer_pixel(scanline_x, scanline_y, color); // Checks if the sprite has priority over the background
                else if (current_scanline_buffer[scanline_x] == 0) lcd.transfer_pixel(scanline_x, scanline_y, color); // Draws the pixel regardless of priority if the background pixel is transparent
            }
        }
    }
}
