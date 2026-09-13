#pragma once
#include <3ds.h>
#include <citro3d.h>

// más adelante uv para texturas, normales para iluminación.
struct Vertex {
    float x, y, z;
    float r, g, b, a;
};

class Mesh {
public:
    void* vbo_data;
    int vertexCount;

    void initCube();
    void cleanup();
};