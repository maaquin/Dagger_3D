#pragma once

#include <cstdint>
#include <vector>
#include "mesh.h"

class Arch3dParser {
public:
    // Decodifica el buffer binario crudo y puebla los vectores de puntos y planos
    static bool parse(const std::vector<uint8_t>& rawData, 
                      std::vector<DaggerPoint>& outPoints, 
                      std::vector<DaggerPlane>& outPlanes);
};