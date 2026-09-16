#pragma once
#include <3ds.h>
#include <citro3d.h>
#include <vector>
#include <stdint.h>
#include "resourceManager.h"

struct Vertex {
    float x, y, z;
    float r, g, b, a;
    float u, v;
};

// Estructura intermedia de punto nativo de ARCH3D
struct DaggerPoint {
    int32_t x, y, z;
};

// Estructura de cara nativa de ARCH3D
struct DaggerUV {
    float u, v;
};

struct DaggerPlane {
    std::vector<uint32_t> indices; // Índices a DaggerPoint
    std::vector<float> u;
    std::vector<float> v;
    
    uint16_t textureArchive; 
    uint16_t subImageIndex;
};

struct SubMesh {
    TextureId textureId;
    size_t indexOffset;
    size_t indexCount;
};

class Mesh {
public:
    void* vbo_data;
    void* ibo_data;
    int vertexCount;
    int indexCount;
    std::vector<SubMesh> subMeshes;

    Mesh() : vbo_data(nullptr), ibo_data(nullptr), vertexCount(0), indexCount(0) {}

    void initFromDaggerData(const std::vector<DaggerPoint>& points, 
                            const std::vector<DaggerPlane>& planes,
                            float scaleFactor = 64.0f);
    void initCube();
    void cleanup();
};