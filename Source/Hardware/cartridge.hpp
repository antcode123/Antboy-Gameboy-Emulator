#pragma once


#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "mbc.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Hardware {
    class Cartridge {
    public:
        std::unique_ptr<U8[]> rom;
        std::unique_ptr<U8[]> ram;
        int rom_size;
        int ram_size;
        bool does_contain_battery;
        bool is_saved;
        std::shared_ptr<MBC> mbc;
        std::string file_path;

        Cartridge();
        ~Cartridge();
        void reset();
        U8 read_rom(U16 address);
        U8 read_ram(U16 address);
        void write_ram(U16 address, U8 u8);
        void insert(std::string _file_path);
        void configure();
        void save_ram();
        void load_ram();
    };
}
