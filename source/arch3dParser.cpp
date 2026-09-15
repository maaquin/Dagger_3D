#include "Arch3dParser.h"
#include <cstring>

// Estructura interna de cabecera de sub-objeto según formato Daggerfall ARCH3D
#pragma pack(push, 1)
struct DaggerSubRecordHeader {
    char name[16];
    uint32_t nullVal;
    uint32_t pointCount;
    uint32_t planeCount;
    uint32_t unknown1;
    uint32_t unknown2;
    uint32_t pointOffset;   // Relativo al inicio del registro
    uint32_t normalOffset;
    uint32_t unknown3;
    uint32_t planeOffset;   // Relativo al inicio del registro
};
#pragma pack(pop)

bool Arch3dParser::parse(const std::vector<uint8_t>& rawData, 
                         std::vector<DaggerPoint>& outPoints, 
                         std::vector<DaggerPlane>& outPlanes) 
{
    outPoints.clear();
    outPlanes.clear();

    if (rawData.size() < 8) {
        return false;
    }

    const uint8_t* ptr = rawData.data();

    // 1. Cabecera principal: Versión (4 bytes) + Conteo de Sub-Objetos (4 bytes)
    uint32_t objectCount = 0;
    memcpy(&objectCount, ptr + 4, sizeof(uint32_t));

    if (objectCount == 0 || objectCount > 512) {
        return false;
    }

    // Offset donde arrancan las cabeceras de sub-objetos (byte 8)
    uint32_t subHeaderOffset = 8;

    for (uint32_t obj = 0; obj < objectCount; ++obj) {
        if (subHeaderOffset + sizeof(DaggerSubRecordHeader) > rawData.size()) {
            break;
        }

        DaggerSubRecordHeader subHeader;
        memcpy(&subHeader, ptr + subHeaderOffset, sizeof(DaggerSubRecordHeader));
        subHeaderOffset += sizeof(DaggerSubRecordHeader);

        uint32_t baseVertexIndex = static_cast<uint32_t>(outPoints.size());

        // 2. Extraer Puntos (Vértices)
        if (subHeader.pointCount > 0 && subHeader.pointOffset < rawData.size()) {
            const uint8_t* ptPtr = ptr + subHeader.pointOffset;
            size_t bytesNeeded = subHeader.pointCount * sizeof(int32_t) * 3;

            if (subHeader.pointOffset + bytesNeeded <= rawData.size()) {
                for (uint32_t p = 0; p < subHeader.pointCount; ++p) {
                    DaggerPoint pt;
                    memcpy(&pt.x, ptPtr + (p * 12) + 0, sizeof(int32_t));
                    memcpy(&pt.y, ptPtr + (p * 12) + 4, sizeof(int32_t));
                    memcpy(&pt.z, ptPtr + (p * 12) + 8, sizeof(int32_t));
                    outPoints.push_back(pt);
                }
            }
        }

        // 3. Extraer Planos (Polígonos / Caras)
        if (subHeader.planeCount > 0 && subHeader.planeOffset < rawData.size()) {
            const uint8_t* plPtr = ptr + subHeader.planeOffset;

            for (uint32_t pl = 0; pl < subHeader.planeCount; ++pl) {
                if (static_cast<size_t>(plPtr - ptr) >= rawData.size()) {
                    break;
                }

                // En Daggerfall cada plano empieza con el número de vértices que lo componen (1 byte)
                uint8_t vertexCountOnPlane = *plPtr;
                plPtr += 1;

                if (vertexCountOnPlane < 3 || vertexCountOnPlane > 32) {
                    // Salto de plano corrupto o descriptor de textura en formatos extendidos
                    plPtr += 24; 
                    continue;
                }

                DaggerPlane plane;
                for (uint8_t v = 0; v < vertexCountOnPlane; ++v) {
                    // En sub-records estándar los índices son de 2 bytes (uint16_t) o 1 byte
                    // Daggerfall 3D v2.x utiliza uint16_t por índice
                    uint16_t idx = 0;
                    memcpy(&idx, plPtr, sizeof(uint16_t));
                    plPtr += sizeof(uint16_t);

                    plane.indices.push_back(baseVertexIndex + idx);
                }

                // Metadatos adicionales de la cara (normales precalculadas / textura) ~24 bytes
                plPtr += 24;

                outPlanes.push_back(plane);
            }
        }
    }

    return !outPoints.empty() && !outPlanes.empty();
}