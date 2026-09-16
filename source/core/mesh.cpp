#include "mesh.h"
#include <string.h>

#include <cstdio>

void Mesh::initFromDaggerData(const std::vector<DaggerPoint> &points,
                              const std::vector<DaggerPlane> &planes,
                              float scaleFactor)
{
    cleanup();

    std::vector<Vertex> rawVertices;
    std::vector<uint16_t> indices;

    if (planes.empty())
        return;

    // arreglo de sub meshs
    SubMesh currentSubMesh;
    currentSubMesh.textureId = {planes[0].textureArchive, planes[0].subImageIndex, 0};
    currentSubMesh.indexOffset = 0;
    currentSubMesh.indexCount = 0;

    // recorrer cada plano / cara
    for (const auto &plane : planes)
    {
        size_t n = plane.indices.size();
        if (n < 3)
            continue;

        // revisar si se comparte textura con otra cara
        TextureId planeTexId = {plane.textureArchive, plane.subImageIndex, 0};

        printf("textura plano id: %d\n", planeTexId);
        printf("textura submesh id: %d\n", currentSubMesh.textureId);

        if (!(planeTexId == currentSubMesh.textureId))
        {
            // Guardamos el grupo anterior
            if (currentSubMesh.indexCount > 0)
            {
                printf("Grupo guardado: %d\n", currentSubMesh.textureId);
                subMeshes.push_back(currentSubMesh);
            }
            // Empezamos un nuevo grupo
            currentSubMesh.textureId = planeTexId;
            currentSubMesh.indexOffset = indices.size();
            currentSubMesh.indexCount = 0;
            printf("grupo iniciado: %d\n", planeTexId);
        }

        uint16_t startIndex = rawVertices.size();

        // 1. Crear los vértices de esta cara UNA SOLA VEZ con sus UVs
        for (size_t v = 0; v < n; ++v)
        {
            const DaggerPoint &pt = points[plane.indices[v]];
            Vertex vert;

            vert.x = static_cast<float>(pt.x) / scaleFactor;
            vert.y = -static_cast<float>(pt.y) / scaleFactor;
            vert.z = -static_cast<float>(pt.z) / scaleFactor;

            vert.r = 1.0f;
            vert.g = 1.0f;
            vert.b = 1.0f;
            vert.a = 1.0f;

            vert.u = plane.u[v];
            vert.v = 1.0f - plane.v[v];

            rawVertices.push_back(vert);
        }

        // 2. Armar los triángulos usando solo índices (invirtiendo para Citro3D)
        for (size_t i = 1; i < n - 1; ++i)
        {
            indices.push_back(startIndex);         // i0 (Pivote)
            indices.push_back(startIndex + i + 1); // i2
            indices.push_back(startIndex + i);     // i1

            currentSubMesh.indexCount += 3;
        }
    }

    if (currentSubMesh.indexCount > 0)
    {
        subMeshes.push_back(currentSubMesh);
    }

    vertexCount = rawVertices.size();
    indexCount = indices.size();

    if (vertexCount == 0 || indexCount == 0)
        return;

    // 1. Asignar memoria para Vértices (VBO)
    size_t vboSize = vertexCount * sizeof(Vertex);
    vbo_data = linearAlloc(vboSize);
    if (vbo_data)
    {
        memcpy(vbo_data, rawVertices.data(), vboSize);
    }

    // 2. Asignar memoria para Índices (IBO)
    size_t iboSize = indexCount * sizeof(uint16_t);
    ibo_data = linearAlloc(iboSize);
    if (ibo_data)
    {
        memcpy(ibo_data, indices.data(), iboSize);
    }
}

void Mesh::cleanup()
{
    if (vbo_data)
    {
        linearFree(vbo_data);
        vbo_data = nullptr;
    }
    if (ibo_data)
    {
        linearFree(ibo_data);
        ibo_data = nullptr;
    }
    vertexCount = 0;
    indexCount = 0; // Añadido para limpiar la cuenta de índices
}