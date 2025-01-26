#pragma once


#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <json.hpp>
#include "Hardware/cpu.hpp"
#include "Hardware/ppu.hpp"
#include "Hardware/mmu.hpp"
#include "Hardware/timer.hpp"
#include "Hardware/cartridge.hpp"
#include "Hardware/lcd.hpp"
#include "Hardware/joypad.hpp"
#include "Utilities/renderer.hpp"


typedef unsigned char U8;


class Gameboy {
public:
    std::string exe_path;
    sf::Image icon;
    sf::Font font;
    Utilities::Renderer renderer;
    sf::VideoMode full_screen_mode;
    sf::VideoMode windowed_mode;
    sf::RenderWindow window;
    sf::Event event;
    sf::Clock clock;
    int emulation_speed;
    double fps;
    double target_fps;
    double delta_time;
    double previous_time;
    double current_time;
    bool is_display_fps_enabled;
    bool is_bootstrap_enabled = true;
    std::vector<std::shared_ptr<std::array<sf::Color, 4>>> palettes;
    int selected_palette_pointer;
    std::shared_ptr<std::array<sf::Color, 4>> selected_palette;
    std::shared_ptr<std::array<sf::Color, 4>> modifying_palette;
    int modifying_palette_segment;
    std::string modifying_control_type;
    std::string modifying_input_type;
    Hardware::CPU cpu;
    Hardware::Cartridge cartridge;
    Hardware::Joypad joypad;
    Hardware::LCD lcd;
    Hardware::PPU ppu;
    Hardware::MMU mmu;
    Hardware::Timer timer;
    std::vector<std::string> roms;
    nlohmann::json key_binds;
    nlohmann::json controller_binds;


    Gameboy(std::string _exe_path);
    ~Gameboy();
    void reset();
    void update_fps();
    void configure_full_screen_parameters();
    void resize_window();
    void configure_window_parameters();
    void restore_default_settings();
    void insert_rom(std::string rom_path);
    void emulate();
    void save_settings();
    void load_settings();
};