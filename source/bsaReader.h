#pragma once

#include <3ds.h>
#include <stdio.h>
#include <vector>
#include <string>

// Información calculada de cada registro individual dentro del BSA
struct BsaRecordInfo {
    uint32_t id;         // ID numérico (en ARCH3D, BLOCKS, MAPS)
    char name[16];       // Nombre de archivo si es de texto (por si acaso)
    uint32_t offset;     // Posición exacta en bytes dentro del archivo SD
    uint32_t size;       // Tamaño en bytes del registro
};

class BsaReader {
private:
    FILE* fileHandle;
    uint16_t recordCount;
    uint16_t directoryType; // 0x0100 (con nombres) o 0x0200 (con IDs numéricos)
    std::vector<BsaRecordInfo> directory;

public:
    BsaReader();
    ~BsaReader();

    // Abre el archivo desde la SD y procesa el directorio al final
    bool open(const std::string& filepath);

    // Cierra el archivo y libera la tabla
    void close();

    // Devuelve cuántos registros contiene
    uint16_t getRecordCount() const { return recordCount; }

    // Busca el índice en la tabla por ID numérico (devuelve -1 si no existe)
    int findRecordIndexById(uint32_t id) const;

    // Extrae los bytes en crudo de un registro específico en un buffer en RAM
    bool extractRecord(int index, std::vector<uint8_t>& outData);
};