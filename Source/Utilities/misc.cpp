#include <cstdint>
#include <cctype>
#include <string>
#include "misc.hpp"



namespace Utilities {
    void set_bit_u8(U8& u8, int bit, bool state) {u8 = state ? u8 | (1 << bit) : u8 & ~(1 << bit);}


    bool get_bit_u8(U8 u8, int bit) {return (bool)(u8 & (1 << bit));}


    void set_bit_u16(U16& u16, int bit, bool state) {u16 = state ? u16 | (1 << bit) : u16 & ~(1 << bit);}


    bool get_bit_u16(U16 u16, int bit) {return (bool)(u16 & (1 << bit));}


    std::string capitalise(std::string string) {
        std::string capitalised_string = string;
        for (int i = 0; i < string.size(); i++) capitalised_string[i] = std::toupper(string[i]);
        return capitalised_string;
    }


    std::string get_file_name_from_path(std::string path) {return path.substr(path.find_last_of("\\") + 1);}


    bool does_file_contain_extension(std::string file_name, std::string extension) {
        return file_name.substr(file_name.length() - extension.length()) == extension;
    }


    int get_random_integer(int start, int end) {return start + rand() % (end - start + 1);}


    sf::Color get_random_color() {
        int r = get_random_integer(0, 255);
        int g = get_random_integer(0, 255);
        int b = get_random_integer(0, 255);
        return sf::Color(r, g, b);
    }


    double lerp_1D(double start, double end, double t) {return start + (end - start) * t;}


    Vector lerp_2D(Vector start, Vector end, double t) {return start + (end - start) * t;}


    sf::Color lerp_rgb(sf::Color start, sf::Color end, double t) {
        int lerped_r = start.r + (end.r - start.r) * t;
        int lerped_g = start.g + (end.g - start.g) * t;
        int lerped_b = start.b + (end.b - start.b) * t;
        return sf::Color(lerped_r, lerped_g, lerped_b);
    }
}