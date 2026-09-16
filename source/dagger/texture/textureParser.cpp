#include "textureParser.h"
#include "textureUtils.h"
#include <cstdio>
#include <vector>

bool TextureParser::loadRecord(const std::string &filepath,
                               int recordIndex,
                               const Palette &palette,
                               DecodedTexture &outTexture)
{
    FILE *file = fopen(filepath.c_str(), "rb");
    if (!file) return false;

    // 1. Validar registros (Cabecera de 26 bytes)
    uint16_t recordCount = 0;
    fread(&recordCount, 2, 1, file);
    if (recordIndex < 0 || recordIndex >= recordCount) {
        fclose(file); return false;
    }

    // 2. Extraer RecordPosition 
    // TextureFileHeader (26) + (Index * 20). RecordPosition está en el byte 2 de esa cabecera.
    fseek(file, 26 + (recordIndex * 20) + 2, SEEK_SET);
    uint32_t recordPosition = 0;
    fread(&recordPosition, 4, 1, file);

    // 3. Extraer variables del TextureRecord (A partir de RecordPosition)
    fseek(file, recordPosition + 4, SEEK_SET); // Saltamos OffsetX y OffsetY
    uint16_t width = 0, height = 0, compression = 0;
    fread(&width, 2, 1, file);
    fread(&height, 2, 1, file);
    fread(&compression, 2, 1, file);

    fseek(file, recordPosition + 14, SEEK_SET); // Vamos directo a DataOffset
    uint32_t dataOffset = 0;
    fread(&dataOffset, 4, 1, file);
    
    fseek(file, recordPosition + 20, SEEK_SET); // Vamos directo a FrameCount
    uint16_t frameCount = 0;
    fread(&frameCount, 2, 1, file);

    printf("\n-- Index %d --\n", recordIndex);
    printf("Size: %ux%u | Comp: %u | Frames: %u\n", width, height, compression, frameCount);

    if (width == 0 || height == 0 || width > 1024 || height > 1024) {
        fclose(file); return false;
    }

    // 4. Saltar a los píxeles (RecordPosition + DataOffset)
    fseek(file, recordPosition + dataOffset, SEEK_SET);
    size_t pixelCount = (size_t)width * height;
    std::vector<uint8_t> rawIndices(pixelCount);

    // 5. El Algoritmo Mágico de UESP (Lectura entrelazada de 256 bytes)
    if (frameCount == 1)
    {
        int p = 0;
        for (int y = 0; y < height; ++y)
        {
            fread(&rawIndices[p], 1, width, file);
            p += width;
            fseek(file, 256 - width, SEEK_CUR); // Saltamos la basura del lienzo
        }
    }
    else
    {
        printf("Aun no soportamos multiples frames.\n");
        fclose(file);
        return false;
    }
    fclose(file);

    // 6. Preparar textura para la GPU
    outTexture.width = width;
    outTexture.height = height;
    outTexture.potWidth = TextureUtils::nextPowerOfTwo(width);
    outTexture.potHeight = TextureUtils::nextPowerOfTwo(height);

    outTexture.linearPixels.resize(pixelCount);
    for (size_t i = 0; i < pixelCount; ++i) {
        outTexture.linearPixels[i] = palette.getColor(rawIndices[i]);
    }

    return true;
}