#include <iostream>
#include <filesystem>
#include <fstream>
#include "gameboy.hpp"
#include <iostream>
#include "UI/state_manager.hpp"


int main(int argc, char* argv[]) {
    std::string exe_path = std::filesystem::canonical(std::filesystem::path(argv[0])).parent_path().string();
    Gameboy gameboy(exe_path);
    UI::StateManager app(gameboy);
    app.run();
};