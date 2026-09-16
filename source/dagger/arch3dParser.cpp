#include "Arch3dParser.h"
#include <cstring>
#include <cstdio>

// Cabecera estricta de 64 bytes de ARCH3D (Según Dave Humphrey)
#pragma pack(push, 1)
struct Dagger3DHeader
{
    char version[4];     // "v2.7" etc.
    uint32_t pointCount; // Total de vértices (Bytes 4-7)
    uint32_t planeCount; // Total de caras (Bytes 8-11)
    uint32_t unknown1;
    uint8_t nullValue1[8];
    uint32_t data1Offset;
    uint32_t data2Offset;
    uint32_t numData2Records;
    uint16_t unknown3;
    uint16_t unknown4;
    uint32_t nullValue3;
    uint32_t nullValue4;
    uint32_t pointOffset; // Dónde inician los vértices (Byte 48)
    uint32_t normalOffset;
    uint32_t unknown6;
    uint32_t planeOffset; // Dónde inician las caras (Byte 60)
};
#pragma pack(pop)

bool Arch3dParser::parse(const std::vector<uint8_t> &rawData,
                         std::vector<DaggerPoint> &outPoints,
                         std::vector<DaggerPlane> &outPlanes)
{
    outPoints.clear();
    outPlanes.clear();

    // Validar tamaño mínimo de la cabecera
    if (rawData.size() < 64)
    {
        printf("Error: Registro demasiado pequeño.\n");
        return false;
    }

    const uint8_t *ptr = rawData.data();
    Dagger3DHeader header;
    memcpy(&header, ptr, sizeof(Dagger3DHeader));

    // 1. Extraer Puntos (X, Y, Z - 12 bytes cada uno)
    if (header.pointCount > 0 && header.pointOffset < rawData.size())
    {
        const uint8_t *ptPtr = ptr + header.pointOffset;
        for (uint32_t i = 0; i < header.pointCount; ++i)
        {
            if (ptPtr + 12 - ptr > rawData.size())
                break;

            DaggerPoint pt;
            memcpy(&pt.x, ptPtr + 0, sizeof(int32_t));
            memcpy(&pt.y, ptPtr + 4, sizeof(int32_t));
            memcpy(&pt.z, ptPtr + 8, sizeof(int32_t));
            outPoints.push_back(pt);

            ptPtr += 12; // Siguiente punto
        }
    }

    // 2. Extraer Planos/Caras (Estructura revelada por la doc)
    if (header.planeCount > 0 && header.planeOffset < rawData.size())
    {
        const uint8_t *plPtr = ptr + header.planeOffset;
        for (uint32_t i = 0; i < header.planeCount; ++i)
        {
            if (plPtr + 8 - ptr > rawData.size())
                break;

            // El primer byte de la cara nos dice cuántos vértices la forman
            uint8_t pointCountOnFace = plPtr[0];

            // Saltamos la mini-cabecera de la cara (8 bytes de metadatos/textura)
            plPtr += 8;

            int currentU = 0;
            int currentV = 0;

            DaggerPlane plane;
            for (uint8_t v = 0; v < pointCountOnFace; ++v)
            {
                if (plPtr + 8 - ptr > rawData.size())
                    break;

                uint32_t pointOffset;
                int16_t texU, texV; // UVs en Daggerfall son enteros cortos con signo
                memcpy(&pointOffset, plPtr, sizeof(uint32_t));
                memcpy(&texU, plPtr + 4, sizeof(int16_t));
                memcpy(&texV, plPtr + 6, sizeof(int16_t));

                // Matemática relativa extraña de Daggerfall según la documentación
                if (v == 0)
                {
                    currentU = texU;
                    currentV = texV;
                }
                else if (v == 1 || v == 2)
                {
                    currentU += texU;
                    currentV += texV;
                }
                else if (v == 3)
                {
                    currentU = texU; // El 4to punto vuelve a ser absoluto
                    currentV = texV;
                }

                uint32_t vertexIndex = pointOffset / 12;
                plane.indices.push_back(vertexIndex);

                // Normalizar a flotantes (0.0 a 1.0).
                // Daggerfall usa un factor de escala de (16 * ancho_textura). Para 64px es 1024.
                plane.u.push_back(currentU / 1024.0f);
                plane.v.push_back(currentV / 1024.0f);

                plPtr += 8;
            }
            outPlanes.push_back(plane);
        }
    }

    printf("Modelo Listo: %zu Verts, %zu Caras\n", outPoints.size(), outPlanes.size());
    return !outPoints.empty() && !outPlanes.empty();
}