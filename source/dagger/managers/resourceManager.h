#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <3ds.h>
#include <citro3d.h>
#include <unordered_map>
#include <cstdint>
#include "palette.h"

struct TextureId
{
    uint16_t archive;
    uint16_t record;
    uint8_t frame;

    bool operator==(const TextureId& other) const
    {
        return archive == other.archive &&
               record  == other.record &&
               frame   == other.frame;
    }
};


// Permite utilizar TextureId como clave de unordered_map
struct TextureIdHash
{
    std::size_t operator()(const TextureId& id) const
    {
        std::size_t hash = 0;

        hash ^= std::hash<uint16_t>{}(id.archive)
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

        hash ^= std::hash<uint16_t>{}(id.record)
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

        hash ^= std::hash<uint8_t>{}(id.frame)
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

        return hash;
    }
};


struct TextureResource
{
    TextureId id;
    int width;
    int height;
    
    C3D_Tex gpuTexture;
    bool loaded;
};


class ResourceManager
{
public:

    ResourceManager();

    void setPalette(Palette* pal);

    TextureResource* getTexture(const TextureId& id);

    // Elimina una textura concreta de memoria.
    void unloadTexture(const TextureId& id);

    // Elimina todas las texturas cargadas.
    void clearTextures();

private:

    std::unordered_map<TextureId, TextureResource, TextureIdHash> textures;
    Palette* activePalette = nullptr;
};

#endif