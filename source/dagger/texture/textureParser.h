#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "palette.h" 

struct DecodedTexture {
    int width;
    int height;
    int potWidth;
    int potHeight;
    std::vector<ColorRGBA> linearPixels; // imagen final
};

class TextureParser {
public:
    static bool loadRecord(const std::string& filepath, 
                           int recordIndex, 
                           const Palette& palette, 
                           DecodedTexture& outTexture);
};
