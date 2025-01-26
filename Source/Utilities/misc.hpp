#pragma once


#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>
#include "vector.hpp"


typedef unsigned char U8;
typedef unsigned short U16;


namespace Utilities {
    void set_bit_u8(U8& u8, int bit, bool state);
    bool get_bit_u8(U8 u8, int bit);
    void set_bit_u16(U16& u16, int bit, bool state);
    bool get_bit_u16(U16 u16, int bit);
    std::string capitalise(std::string string);
    std::string get_file_name_from_path(std::string path);
    bool does_file_contain_extension(std::string file_name, std::string extension);
    int get_random_integer(int start, int end);
    sf::Color get_random_color();
    double lerp_1D(double start, double end, double t);
    Vector lerp_2D(Vector start, Vector end, double t);
    sf::Color lerp_rgb(sf::Color start, sf::Color end, double t);
}
