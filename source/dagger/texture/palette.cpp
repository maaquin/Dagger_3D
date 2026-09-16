#include "palette.h"
#include <cstdio>

Palette::Palette() {
    for (int i = 0; i < 256; ++i) {
        colors[i] = {0, 0, 0, 255};
    }
}

bool Palette::load(const std::string& path) {
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return false;

    uint8_t raw[768];
    if (fread(raw, 1, 768, f) != 768) {
        fclose(f);
        return false;
    }
    fclose(f);

    for (int i = 0; i < 256; ++i) {
        colors[i].r = raw[i * 3 + 0];
        colors[i].g = raw[i * 3 + 1];
        colors[i].b = raw[i * 3 + 2];
        // 0 = transparencia
        colors[i].a = (i == 0) ? 0 : 255;
    }

    return true;
}

ColorRGBA Palette::getColor(uint8_t index) const {
    return colors[index];
}