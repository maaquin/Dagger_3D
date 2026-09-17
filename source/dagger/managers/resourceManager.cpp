#include "resourceManager.h"
#include <cstdio>
#include "textureParser.h"
#include "textureUtils.h"
#include "palette.h"

ResourceManager::ResourceManager()
{
}

void ResourceManager::setPalette(Palette *pal)
{
    activePalette = pal;
}

TextureResource *ResourceManager::getTexture(const TextureId &id)
{
    auto it = textures.find(id);
    if (it != textures.end())
    {
        return &it->second;
    }
    if (!activePalette)
        return nullptr;

    // 1. Construir la ruta dinámicamente según el ID
    char texPath[128];
    snprintf(texPath, sizeof(texPath), "sdmc:/3ds/DAGGER/ARENA2/TEXTURE.%03d", id.archive);

    DecodedTexture tex;
    // 2. Extraer y cargar usando el index (id.record)
    if (TextureParser::loadRecord(texPath, id.record, *activePalette, tex))
    {
        TextureResource newRes;
        newRes.id = id;
        newRes.width = tex.width;
        newRes.height = tex.height;
        newRes.loaded = true;

        // Swizzling
        std::vector<uint32_t> tiledPixels(tex.potWidth * tex.potHeight);
        TextureUtils::tileImage8x8(tex.linearPixels.data(), tiledPixels.data(),
                                   tex.width, tex.height, tex.potWidth, tex.potHeight);

        // Subir a GPU
        C3D_TexInit(&newRes.gpuTexture, (u16)tex.potWidth, (u16)tex.potHeight, GPU_RGBA8);
        C3D_TexSetFilter(&newRes.gpuTexture, GPU_NEAREST, GPU_NEAREST);
        C3D_TexUpload(&newRes.gpuTexture, tiledPixels.data());
        C3D_TexSetWrap(&newRes.gpuTexture, GPU_REPEAT, GPU_REPEAT);

        // Guardar en el mapa
        textures[id] = newRes;

        return &textures[id];
    }
    else
    {
        printf("ERROR: No se pudo cargar %s index %d\n", texPath, id.record);
    }

    return nullptr;
}

void ResourceManager::unloadTexture(const TextureId &id)
{
    auto it = textures.find(id);
    if (it != textures.end())
    {
        // liberar memoria antes de borrar la textura
        C3D_TexDelete(&it->second.gpuTexture);
        textures.erase(it);
    }
}

void ResourceManager::clearTextures()
{
    for (auto &pair : textures)
    {
        C3D_TexDelete(&pair.second.gpuTexture);
    }
    textures.clear();
}