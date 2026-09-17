#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include <3ds.h>
#include <citro3d.h>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include "mesh.h"
#include "bsaReader.h"

struct MeshId
{
    uint32_t record;

    bool operator==(const MeshId& other) const
    {
        return record == other.record;
    }
};

struct MeshIdHash
{
    std::size_t operator()(const MeshId& k) const
    {
        return std::hash<uint32_t>()(k.record);
    }
};

struct MeshResource
{
    MeshId id;
    void* vbo_data = nullptr;
    void* ibo_data = nullptr;
    size_t vertexCount = 0;
    size_t indexCount = 0;
    std::vector<SubMesh> subMeshes;
    bool loaded = false;
};

class MeshManager
{
public:
    MeshManager();
    ~MeshManager();

    void setBsaReader(BsaReader* bsa);

    MeshResource* getMesh(const MeshId &id, float scale);
    void unloadMesh(const MeshId &id);
    void clearMeshes();

private:
    std::unordered_map<MeshId, MeshResource, MeshIdHash> meshes;
    BsaReader* activeBsa = nullptr;
};

#endif