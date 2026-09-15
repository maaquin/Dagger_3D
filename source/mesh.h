#pragma once
#include <3ds.h>
#include <citro3d.h>
#include <vector>
#include <stdint.h>

struct Vertex {
    float x, y, z;
    float r, g, b, a;
};

// Estructura intermedia de punto nativo de ARCH3D
struct DaggerPoint {
    int32_t x, y, z;
};

// Estructura de cara nativa de ARCH3D
struct DaggerPlane {
    std::vector<uint32_t> indices; // Índices a la lista de DaggerPoint
};

class Mesh {
public:
    void* vbo_data;
    int vertexCount;

    Mesh() : vbo_data(nullptr), vertexCount(0) {}

    void initFromDaggerData(const std::vector<DaggerPoint>& points, 
                            const std::vector<DaggerPlane>& planes,
                            float scaleFactor = 64.0f);
    void initCube();
    void cleanup();
};