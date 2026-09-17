#include "meshManager.h"
#include <cstdio>
#include "mesh.h"
#include "arch3dParser.h"

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
    clearMeshes();
}

void MeshManager::setBsaReader(BsaReader* bsa)
{
    activeBsa = bsa;
}

MeshResource *MeshManager::getMesh(const MeshId &id, float scale)
{
    auto it = meshes.find(id);
    if (it != meshes.end())
    {
        return &it->second;
    }

    if (!activeBsa)
        return nullptr;

    std::vector<uint8_t> buffer;

    // extraer de arch3d
    if (activeBsa->extractRecord(id.record, buffer))
    {
        std::vector<DaggerPoint> points;
        std::vector<DaggerPlane> planes;

        // parsear geometría daggerfall
        if (Arch3dParser::parse(buffer, points, planes))
        {
            Mesh mesh;
            mesh.initFromDaggerData(points, planes, scale);

            // empaquetación de recursos
            MeshResource newMesh;
            newMesh.id = id;
            newMesh.vbo_data = mesh.vbo_data;
            newMesh.ibo_data = mesh.ibo_data;
            newMesh.vertexCount = mesh.vertexCount;
            newMesh.indexCount = mesh.indexCount;
            newMesh.subMeshes = mesh.subMeshes;
            newMesh.loaded = true;

            mesh.vbo_data = nullptr;
            mesh.ibo_data = nullptr;

            meshes[id] = newMesh;
            return &meshes[id];
        }
    }
    else
    {
        printf("ERROR: No se pudo extraer el modelo ARCH3D record %d\n", id.record);
    }

    return nullptr;
}

void MeshManager::unloadMesh(const MeshId &id)
{
    auto it = meshes.find(id);
    if (it != meshes.end())
    {
        if (it->second.vbo_data)
        {
            linearFree(it->second.vbo_data);
            it->second.vbo_data = nullptr;
        }
        if (it->second.ibo_data)
        {
            linearFree(it->second.ibo_data);
            it->second.ibo_data = nullptr;
        }
        meshes.erase(it);
    }
}

void MeshManager::clearMeshes()
{
    for (auto &pair : meshes)
    {
        if (pair.second.vbo_data)
        {
            linearFree(pair.second.vbo_data);
            pair.second.vbo_data = nullptr;
        }
        if (pair.second.ibo_data)
        {
            linearFree(pair.second.ibo_data);
            pair.second.ibo_data = nullptr;
        }
    }
    meshes.clear();
}