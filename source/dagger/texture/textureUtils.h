#pragma once
#include <cstdint>
#include <vector>
#include <cstdio>
#include <cstring>
#include "palette.h"

class TextureUtils {
public:
    static uint32_t nextPowerOfTwo(uint32_t v) {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return (v < 8) ? 8 : v;
    }

    static void tileImage8x8(const ColorRGBA* srcLinear, uint32_t* dstTiled, 
                             uint32_t srcWidth, uint32_t srcHeight, 
                             uint32_t potWidth, uint32_t potHeight) 
    {
        for (uint32_t y = 0; y < potHeight; ++y) {
            for (uint32_t x = 0; x < potWidth; ++x) {
                ColorRGBA pixel = {0, 0, 0, 0};
                if (x < srcWidth && y < srcHeight) {
                    pixel = srcLinear[y * srcWidth + x];
                }

                uint32_t subX = x & 7;
                uint32_t subY = y & 7;

                uint32_t morton = 
                    ((subX & 1) >> 0) | ((subY & 1) << 1) |
                    ((subX & 2) << 1) | ((subY & 2) << 2) |
                    ((subX & 4) << 2) | ((subY & 4) << 3);

                uint32_t tileIndex = (y / 8) * (potWidth / 8) + (x / 8);
                uint32_t destIndex = (tileIndex * 64) + morton;

                dstTiled[destIndex] = (pixel.a << 24) | (pixel.b << 16) | (pixel.g << 8) | pixel.r;
            }
        }
    }
};