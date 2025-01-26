#include <SFML/Graphics.hpp>
#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>
#include "gameboy.hpp"
#include "Utilities/misc.hpp"


// Houses all Gameboy components/configurations, handling the emulation and loading/storing settings
Gameboy::Gameboy(std::string _exe_path) :
    exe_path(_exe_path),
    fps(60),
    previous_time(0),
    full_screen_mode(sf::VideoMode::getFullscreenModes()[0]),
    windowed_mode(sf::VideoMode(0, 0)),
    cpu(mmu),
    ppu(mmu, lcd, cpu),
    timer(mmu, cpu),
    joypad(cpu),
    mmu(cpu, ppu, cartridge, joypad, timer, exe_path) {
    font.loadFromFile(exe_path + "\\Assets\\pixel_mix_regular_font.ttf");
    icon.loadFromFile(exe_path + "\\Assets\\antboy_icon.png");
    configure_full_screen_parameters();
    load_settings();
}


Gameboy::~Gameboy() {
    save_settings();
}


void Gameboy::reset() {
    cpu.reset();
    mmu.reset();
    ppu.reset();
    lcd.reset();
    timer.reset();
    cartridge.reset();
    joypad.reset();

    if (is_bootstrap_enabled) {
        mmu.is_bootstrap_enabled = true;
        cpu.program_counter = 0;
        return;
    }

    mmu.is_bootstrap_enabled = false;
    cpu.program_counter = 0x100;
}


void Gameboy::insert_rom(std::string rom_path) {
    reset();
    cartridge.insert(rom_path);
}


void Gameboy::emulate() {

    // Calculates the ticks threshold for the current frame
    // This makes sure the components run at the desired emulation speed regardless of the FPS
    // If the FPS drops below 5, reset the ticks threshold of a 60 FPS iteration
    // This avoids processing too much data per frame, which could lead to an unresponsive window
    int ticks_per_second = (cpu.clock_speed * emulation_speed) / 100;
    int ticks_per_frame = fps > 5 ? ticks_per_second / fps : ticks_per_second / target_fps;

    // Components are clocked by the ticks of the CPU's last instruction
    // This iterates until the CPU's ticks threshold is reached for the current frame
    while (cpu.ticks < ticks_per_frame) {
        int last_instruction_ticks = cpu.run();
        cpu.ticks += last_instruction_ticks;
        ppu.run(last_instruction_ticks);
        timer.run(last_instruction_ticks);
        cpu.handle_interrupts();
    }

    cpu.ticks -= ticks_per_frame;
}


void Gameboy::update_fps() {
    current_time = clock.getElapsedTime().asSeconds();
    delta_time = current_time - previous_time;
    double expected_delta_time = 1 / target_fps;

    // Sleeps the program when the program runs faster than the target framerate
    // This limits the FPS to the target FPS
    if (delta_time < expected_delta_time) {
        double wait_time = expected_delta_time - delta_time;
        sf::sleep(sf::seconds(wait_time));
        previous_time = clock.getElapsedTime().asSeconds();
        delta_time = expected_delta_time;
    }

    else previous_time = current_time;
    fps = 1 / delta_time;
}


void Gameboy::configure_full_screen_parameters() {
    lcd.full_screen_scale_factor = full_screen_mode.height / lcd.height;
    lcd.gap_width = (full_screen_mode.width - lcd.full_screen_scale_factor * lcd.width) / lcd.full_screen_scale_factor;
    lcd.gap_height = (full_screen_mode.height - lcd.full_screen_scale_factor * lcd.height) / lcd.full_screen_scale_factor;
    lcd.black_bar_width = lcd.gap_width * 0.5 ;
    lcd.black_bar_height = lcd.height;
}


void Gameboy::resize_window() {
    if (lcd.scale_factor == lcd.full_screen_scale_factor) {
        window.create(full_screen_mode, "Antboy", sf::Style::Fullscreen);
        lcd.position = Utilities::Vector(lcd.black_bar_width, lcd.gap_height * 0.5);
        configure_window_parameters();
        return;
    }

    window.create(windowed_mode, "Antboy", sf::Style::Titlebar | sf::Style::Close);
    lcd.position = Utilities::Vector(0, 0);
    int window_width = lcd.width * lcd.scale_factor;
    int window_height = lcd.height * lcd.scale_factor;
    window.setSize(sf::Vector2u(window_width, window_height));
    window.setView(sf::View(sf::FloatRect(0, 0, window_width, window_height)));
    int window_x = full_screen_mode.width / 2 - window_width / 2;
    int window_y = full_screen_mode.height / 2 - window_height / 2;
    window.setPosition(sf::Vector2i(window_x, window_y));
    configure_window_parameters();
}


void Gameboy::configure_window_parameters() {
    while (window.pollEvent(event)) {} // Flushes any events which were transfered over from the previous window
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    window.setKeyRepeatEnabled(false);
    window.setMouseCursorVisible(false);
}


void Gameboy::load_settings() {
    try {
        std::ifstream settings_file(exe_path + "\\Assets\\settings.json");
        nlohmann::json settings_json;
        settings_file >> settings_json;
        emulation_speed = settings_json["EMULATION_SPEED"];
        is_bootstrap_enabled = settings_json["IS_BOOTSTRAP_ENABLED"];
        target_fps = settings_json["TARGET_FPS"];
        is_display_fps_enabled = settings_json["IS_DISPLAY_FPS_ENABLED"];
        lcd.scale_factor = settings_json["SCALE_FACTOR"];
        lcd.is_retro_mode_enabled = settings_json["IS_RETRO_MODE_ENABLED"];
        lcd.frame_blend_strength = settings_json["FRAME_BLEND_STRENGTH"];
        palettes.resize(settings_json["NUMBER_OF_PALETTES"]);

        for (int i = 0; i < palettes.size(); i++) {
            palettes[i] = std::make_shared<std::array<sf::Color, 4>>();

            for (int j = 0; j < 4; j++) {
                (*palettes[i])[j].r = settings_json["PALETTES"][std::to_string(i)][std::to_string(j)]["R"];
                (*palettes[i])[j].g = settings_json["PALETTES"][std::to_string(i)][std::to_string(j)]["G"];
                (*palettes[i])[j].b = settings_json["PALETTES"][std::to_string(i)][std::to_string(j)]["B"];
            }
        }

        selected_palette_pointer = settings_json["SELECTED_PALETTE_POINTER"];
        selected_palette = palettes[selected_palette_pointer];
        key_binds["GAME"]["SELECT"] = settings_json["GAME"]["KEYBOARD"]["SELECT"];
        key_binds["GAME"]["START"] = settings_json["GAME"]["KEYBOARD"]["START"];
        key_binds["GAME"]["A"] = settings_json["GAME"]["KEYBOARD"]["A"];
        key_binds["GAME"]["B"] = settings_json["GAME"]["KEYBOARD"]["B"];
        key_binds["GAME"]["PAUSE"] = settings_json["GAME"]["KEYBOARD"]["PAUSE"];
        key_binds["GAME"]["UP"] = settings_json["GAME"]["KEYBOARD"]["UP"];
        key_binds["GAME"]["DOWN"] = settings_json["GAME"]["KEYBOARD"]["DOWN"];
        key_binds["GAME"]["LEFT"] = settings_json["GAME"]["KEYBOARD"]["LEFT"];
        key_binds["GAME"]["RIGHT"] = settings_json["GAME"]["KEYBOARD"]["RIGHT"];
        key_binds["SYSTEM"]["SELECT"] = settings_json["SYSTEM"]["KEYBOARD"]["SELECT"];
        key_binds["SYSTEM"]["BACK"] = settings_json["SYSTEM"]["KEYBOARD"]["BACK"];
        key_binds["SYSTEM"]["UP"] = settings_json["SYSTEM"]["KEYBOARD"]["UP"];
        key_binds["SYSTEM"]["DOWN"] = settings_json["SYSTEM"]["KEYBOARD"]["DOWN"];
        key_binds["SYSTEM"]["LEFT"] = settings_json["SYSTEM"]["KEYBOARD"]["LEFT"];
        key_binds["SYSTEM"]["RIGHT"] = settings_json["SYSTEM"]["KEYBOARD"]["RIGHT"];
        controller_binds["GAME"]["SELECT"] = settings_json["GAME"]["CONTROLLER"]["SELECT"];
        controller_binds["GAME"]["BACK"] = settings_json["GAME"]["CONTROLLER"]["BACK"];
        controller_binds["GAME"]["START"] = settings_json["GAME"]["CONTROLLER"]["START"];
        controller_binds["GAME"]["A"] = settings_json["GAME"]["CONTROLLER"]["A"];
        controller_binds["GAME"]["B"] = settings_json["GAME"]["CONTROLLER"]["B"];
        controller_binds["GAME"]["PAUSE"] = settings_json["GAME"]["CONTROLLER"]["PAUSE"];
        controller_binds["SYSTEM"]["SELECT"] = settings_json["SYSTEM"]["CONTROLLER"]["SELECT"];
        controller_binds["SYSTEM"]["BACK"] = settings_json["SYSTEM"]["CONTROLLER"]["BACK"];
        resize_window();
    }

    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        restore_default_settings();
    }
}


void Gameboy::save_settings() {
    try {
        nlohmann::json settings_json;
        std::ofstream settings_file(exe_path + "\\Assets\\settings.json");
        settings_json["EMULATION_SPEED"] = emulation_speed;
        settings_json["IS_BOOTSTRAP_ENABLED"] = is_bootstrap_enabled;
        settings_json["TARGET_FPS"] = target_fps;
        settings_json["IS_DISPLAY_FPS_ENABLED"] = is_display_fps_enabled;
        settings_json["SCALE_FACTOR"] = lcd.scale_factor;
        settings_json["IS_RETRO_MODE_ENABLED"] = lcd.is_retro_mode_enabled;
        settings_json["FRAME_BLEND_STRENGTH"] = lcd.frame_blend_strength;
        settings_json["NUMBER_OF_PALETTES"] = palettes.size();

        for (int i = 0; i < palettes.size(); i++) {
            for (int j = 0; j < 4; j++) {
                settings_json["PALETTES"][std::to_string(i)][std::to_string(j)]["R"] = (*palettes[i])[j].r;
                settings_json["PALETTES"][std::to_string(i)][std::to_string(j)]["G"] = (*palettes[i])[j].g;
                settings_json["PALETTES"][std::to_string(i)][std::to_string(j)]["B"] = (*palettes[i])[j].b;
            }
        }

        settings_json["SELECTED_PALETTE_POINTER"] = selected_palette_pointer;
        settings_json["GAME"]["KEYBOARD"]["SELECT"] = key_binds["GAME"]["SELECT"];
        settings_json["GAME"]["KEYBOARD"]["START"] = key_binds["GAME"]["START"];
        settings_json["GAME"]["KEYBOARD"]["A"] = key_binds["GAME"]["A"];
        settings_json["GAME"]["KEYBOARD"]["B"] = key_binds["GAME"]["B"];
        settings_json["GAME"]["KEYBOARD"]["PAUSE"] = key_binds["GAME"]["PAUSE"];
        settings_json["GAME"]["KEYBOARD"]["UP"] = key_binds["GAME"]["UP"];
        settings_json["GAME"]["KEYBOARD"]["DOWN"] = key_binds["GAME"]["DOWN"];
        settings_json["GAME"]["KEYBOARD"]["LEFT"] = key_binds["GAME"]["LEFT"];
        settings_json["GAME"]["KEYBOARD"]["RIGHT"] = key_binds["GAME"]["RIGHT"];
        settings_json["SYSTEM"]["KEYBOARD"]["SELECT"] = key_binds["SYSTEM"]["SELECT"];
        settings_json["SYSTEM"]["KEYBOARD"]["BACK"] = key_binds["SYSTEM"]["BACK"];
        settings_json["SYSTEM"]["KEYBOARD"]["UP"] = key_binds["SYSTEM"]["UP"];
        settings_json["SYSTEM"]["KEYBOARD"]["DOWN"] = key_binds["SYSTEM"]["DOWN"];
        settings_json["SYSTEM"]["KEYBOARD"]["LEFT"] = key_binds["SYSTEM"]["LEFT"];
        settings_json["SYSTEM"]["KEYBOARD"]["RIGHT"] = key_binds["SYSTEM"]["RIGHT"];
        settings_json["GAME"]["CONTROLLER"]["SELECT"] = controller_binds["GAME"]["SELECT"];
        settings_json["GAME"]["CONTROLLER"]["START"] = controller_binds["GAME"]["START"];
        settings_json["GAME"]["CONTROLLER"]["A"] = controller_binds["GAME"]["A"];
        settings_json["GAME"]["CONTROLLER"]["B"] = controller_binds["GAME"]["B"];
        settings_json["GAME"]["CONTROLLER"]["PAUSE"] = controller_binds["GAME"]["PAUSE"];
        settings_json["SYSTEM"]["CONTROLLER"]["SELECT"] = controller_binds["SYSTEM"]["SELECT"];
        settings_json["SYSTEM"]["CONTROLLER"]["BACK"] = controller_binds["SYSTEM"]["BACK"];
        settings_file << settings_json;
    }

    catch (const std::exception& e) {std::cerr << "Error: " << e.what() << std::endl;}
}


void Gameboy::restore_default_settings() {
    emulation_speed = 100;
    is_bootstrap_enabled = true;
    target_fps = 60;
    is_display_fps_enabled = true;
    lcd.scale_factor = lcd.full_screen_scale_factor;
    lcd.is_retro_mode_enabled = true;
    lcd.frame_blend_strength = 1;
    palettes.clear();

    palettes.push_back(std::make_shared<std::array<sf::Color, 4>>(std::array<sf::Color, 4>{
        sf::Color(198, 203, 165),
        sf::Color(140, 146, 107),
        sf::Color(74, 81, 57),
        sf::Color(24, 24, 24)
    }));

    palettes.push_back(std::make_shared<std::array<sf::Color, 4>>(std::array<sf::Color, 4>{
        sf::Color(254, 250, 224),
        sf::Color(221, 161, 94),
        sf::Color(96, 108, 56),
        sf::Color(40, 54, 24)
    }));

    palettes.push_back(std::make_shared<std::array<sf::Color, 4>>(std::array<sf::Color, 4>{
        sf::Color(218, 191, 255),
        sf::Color(144, 122, 214),
        sf::Color(79, 81, 140),
        sf::Color(44, 42, 74)
    }));

    palettes.push_back(std::make_shared<std::array<sf::Color, 4>>(std::array<sf::Color, 4>{
        sf::Color(244, 241, 222),
        sf::Color(224, 122, 95),
        sf::Color(129, 178, 154),
        sf::Color(61, 64, 91)
    }));

    palettes.push_back(std::make_shared<std::array<sf::Color, 4>>(std::array<sf::Color, 4>{
        sf::Color(202, 210, 197),
        sf::Color(132, 169, 140),
        sf::Color(82, 121, 111),
        sf::Color(53, 79, 82)
    }));

    palettes.push_back(std::make_shared<std::array<sf::Color, 4>>(std::array<sf::Color, 4>{
        sf::Color(250, 249, 249),
        sf::Color(190, 227, 219),
        sf::Color(137, 176, 174),
        sf::Color(85, 91, 110)
    }));

    selected_palette_pointer = 0;
    selected_palette = palettes[selected_palette_pointer];
    key_binds["GAME"]["SELECT"] = sf::Keyboard::Enter;
    key_binds["GAME"]["START"] = sf::Keyboard::Space;
    key_binds["GAME"]["A"] = sf::Keyboard::K;
    key_binds["GAME"]["B"] = sf::Keyboard::J;
    key_binds["GAME"]["PAUSE"] = sf::Keyboard::Escape;
    key_binds["GAME"]["LEFT"] = sf::Keyboard::A;
    key_binds["GAME"]["RIGHT"] = sf::Keyboard::D;
    key_binds["GAME"]["UP"] = sf::Keyboard::W;
    key_binds["GAME"]["DOWN"] = sf::Keyboard::S;
    key_binds["SYSTEM"]["SELECT"] = sf::Keyboard::Enter;
    key_binds["SYSTEM"]["BACK"] = sf::Keyboard::Escape;
    key_binds["SYSTEM"]["LEFT"] = sf::Keyboard::Left;
    key_binds["SYSTEM"]["RIGHT"] = sf::Keyboard::Right;
    key_binds["SYSTEM"]["UP"] = sf::Keyboard::Up;
    key_binds["SYSTEM"]["DOWN"] = sf::Keyboard::Down;
    controller_binds["GAME"]["SELECT"] = 2;
    controller_binds["GAME"]["BACK"] = 4;
    controller_binds["GAME"]["START"] = 3;
    controller_binds["GAME"]["A"] = 1;
    controller_binds["GAME"]["B"] = 0;
    controller_binds["GAME"]["PAUSE"] = 7;
    controller_binds["SYSTEM"]["SELECT"] = 0;
    controller_binds["SYSTEM"]["BACK"] = 1;
    resize_window();
}
