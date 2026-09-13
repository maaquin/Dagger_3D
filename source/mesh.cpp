// mesh.cpp
#include "mesh.h"
#include <string.h>

void Mesh::initCube() {
    vertexCount = 36; // 6 caras * 2 triángulos * 3 vértices
    
    // Cubo de 2x2x2 centrado en el origen
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
    memcpy(vbo_data, cubeVertices, sizeof(cubeVertices));
}

void Mesh::cleanup() {
    if (vbo_data) linearFree(vbo_data);
}