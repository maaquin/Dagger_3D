#include "mesh.h"
#include <string.h>

void Mesh::initFromDaggerData(const std::vector<DaggerPoint>& points, 
                            const std::vector<DaggerPlane>& planes,
                            float scaleFactor) 
{
    cleanup();

    std::vector<Vertex> rawVertices;

    // Recorrer cada plano y descomponer los N-gons en triángulos (Triangle Fan)
    for (const auto& plane : planes) {
        size_t n = plane.indices.size();
        if (n < 3) continue; // Descarta aristas o puntos sueltos

        for (size_t i = 1; i < n - 1; ++i) {
            uint32_t i0 = plane.indices[0];
            uint32_t i1 = plane.indices[i];
            uint32_t i2 = plane.indices[i + 1];

            if (i0 >= points.size() || i1 >= points.size() || i2 >= points.size()) {
                continue;
            }

            // Inversión i0 -> i2 -> i1 para corregir CCW en PICA200 / Citro3D
            uint32_t triIndices[3] = { i0, i2, i1 };

            for (int k = 0; k < 3; ++k) {
                const DaggerPoint& pt = points[triIndices[k]];
                Vertex v;
                // Conversión de sistema de coordenadas Daggerfall (Y-up invertido)
                v.x = static_cast<float>(pt.x) / scaleFactor;
                v.y = -static_cast<float>(pt.y) / scaleFactor;
                v.z = -static_cast<float>(pt.z) / scaleFactor;

                // Color plano default para sombreado temporal
                v.r = 0.75f;
                v.g = 0.75f;
                v.b = 0.75f;
                v.a = 1.0f;

                rawVertices.push_back(v);
            }
        }
    }

    vertexCount = rawVertices.size();
    if (vertexCount == 0) return;

    size_t bufferSize = vertexCount * sizeof(Vertex);
    vbo_data = linearAlloc(bufferSize);
    if (vbo_data) {
        memcpy(vbo_data, rawVertices.data(), bufferSize);
    }
}

void Mesh::initCube() {
    cleanup();
    vertexCount = 36;
    Vertex cubeVertices[36] = {
        // Frente (Rojo)
        {-1, -1,  1,  1, 0, 0, 1}, { 1, -1,  1,  1, 0, 0, 1}, { 1,  1,  1,  1, 0, 0, 1},
        {-1, -1,  1,  1, 0, 0, 1}, { 1,  1,  1,  1, 0, 0, 1}, {-1,  1,  1,  1, 0, 0, 1},
        // Atrás (Verde)
        { 1, -1, -1,  0, 1, 0, 1}, {-1, -1, -1,  0, 1, 0, 1}, {-1,  1, -1,  0, 1, 0, 1},
        { 1, -1, -1,  0, 1, 0, 1}, {-1,  1, -1,  0, 1, 0, 1}, { 1,  1, -1,  0, 1, 0, 1},
        // Izquierda (Azul)
        {-1, -1, -1,  0, 0, 1, 1}, {-1, -1,  1,  0, 0, 1, 1}, {-1,  1,  1,  0, 0, 1, 1},
        {-1, -1, -1,  0, 0, 1, 1}, {-1,  1,  1,  0, 0, 1, 1}, {-1,  1, -1,  0, 0, 1, 1},
        // Derecha (Amarillo)
        { 1, -1,  1,  1, 1, 0, 1}, { 1, -1, -1,  1, 1, 0, 1}, { 1,  1, -1,  1, 1, 0, 1},
        { 1, -1,  1,  1, 1, 0, 1}, { 1,  1, -1,  1, 1, 0, 1}, { 1,  1,  1,  1, 1, 0, 1},
        // Arriba (Magenta)
        {-1,  1,  1,  1, 0, 1, 1}, { 1,  1,  1,  1, 0, 1, 1}, { 1,  1, -1,  1, 0, 1, 1},
        {-1,  1,  1,  1, 0, 1, 1}, { 1,  1, -1,  1, 0, 1, 1}, {-1,  1, -1,  1, 0, 1, 1},
        // Abajo (Cian)
        {-1, -1, -1,  0, 1, 1, 1}, { 1, -1, -1,  0, 1, 1, 1}, { 1, -1,  1,  0, 1, 1, 1},
        {-1, -1, -1,  0, 1, 1, 1}, { 1, -1,  1,  0, 1, 1, 1}, {-1, -1,  1,  0, 1, 1, 1},
    };

    vbo_data = linearAlloc(sizeof(cubeVertices));
    if (vbo_data) {
        memcpy(vbo_data, cubeVertices, sizeof(cubeVertices));
    }
}

void Mesh::cleanup() {
    if (vbo_data) {
        linearFree(vbo_data);
        vbo_data = nullptr;
    }
    vertexCount = 0;
}