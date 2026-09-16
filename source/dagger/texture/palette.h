#pragma once
#include <cstdint>
#include <string>

struct ColorRGBA {
    uint8_t r, g, b, a;
};

class Palette {
public:
    ColorRGBA colors[256];

    Palette();
    bool load(const std::string& path);
    ColorRGBA getColor(uint8_t index) const;
};