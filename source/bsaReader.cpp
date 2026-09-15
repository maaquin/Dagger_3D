#include "BsaReader.h"
#include <cstring>

BsaReader::BsaReader() : fileHandle(nullptr), recordCount(0), directoryType(0) {}

BsaReader::~BsaReader() {
    close();
}

void BsaReader::close() {
    if (fileHandle) {
        fclose(fileHandle);
        fileHandle = nullptr;
    }
    directory.clear();
    recordCount = 0;
    directoryType = 0;
}

bool BsaReader::open(const std::string& filepath) {
    close();

    fileHandle = fopen(filepath.c_str(), "rb");
    if (!fileHandle) {
        return false;
    }

    // 1. Leer cabecera de 4 bytes
    uint16_t header[2];
    if (fread(header, sizeof(uint16_t), 2, fileHandle) != 2) {
        close();
        return false;
    }

    recordCount = header[0];
    directoryType = header[1];

    if (recordCount == 0) {
        close();
        return false;
    }

    // 2. Ir al final del archivo para saber el tamaño total
    fseek(fileHandle, 0, SEEK_END);
    long totalFileSize = ftell(fileHandle);

    // 3. Calcular dónde empieza el directorio
    // 0x0100 = 18 bytes por entrada (14 de nombre + 4 de tamaño)
    // 0x0200 = 8 bytes por entrada (4 de ID + 4 de tamaño)
    size_t entrySize = (directoryType == 0x0200) ? 8 : 18;
    size_t dirTotalBytes = recordCount * entrySize;
    long dirStartOffset = totalFileSize - (long)dirTotalBytes;

    if (dirStartOffset < 4) {
        // Archivo corrupto o mal formado
        close();
        return false;
    }

    // Saltamos al inicio del directorio
    fseek(fileHandle, dirStartOffset, SEEK_SET);

    // 4. Leer todas las entradas del directorio
    directory.resize(recordCount);

    for (uint16_t i = 0; i < recordCount; ++i) {
        if (directoryType == 0x0200) {
            // Variante ARCH3D: [ID 4 bytes] [Size 4 bytes]
            uint32_t idAndSize[2];
            fread(idAndSize, sizeof(uint32_t), 2, fileHandle);
            directory[i].id = idAndSize[0];
            directory[i].size = idAndSize[1];
            directory[i].name[0] = '\0';
        } else {
            // Variante Nombres: [Name 14 bytes] [Size 4 bytes]
            char rawName[14];
            uint32_t recSize;
            fread(rawName, 14, 1, fileHandle);
            fread(&recSize, sizeof(uint32_t), 1, fileHandle);
            memcpy(directory[i].name, rawName, 14);
            directory[i].name[14] = '\0';
            directory[i].size = recSize;
            directory[i].id = i;
        }
    }

    // 5. Precalcular los offsets acumulativos
    // El primer registro empieza inmediatamente tras la cabecera (byte 4)
    uint32_t currentOffset = 4;
    for (uint16_t i = 0; i < recordCount; ++i) {
        directory[i].offset = currentOffset;
        currentOffset += directory[i].size;
    }

    return true;
}

int BsaReader::findRecordIndexById(uint32_t id) const {
    for (size_t i = 0; i < directory.size(); ++i) {
        if (directory[i].id == id) {
            return (int)i;
        }
    }
    return -1;
}

bool BsaReader::extractRecord(int index, std::vector<uint8_t>& outData) {
    if (!fileHandle || index < 0 || index >= (int)directory.size()) {
        return false;
    }

    const BsaRecordInfo& record = directory[index];
    outData.resize(record.size);

    // Saltar al offset del registro y leer exactamente sus bytes
    fseek(fileHandle, record.offset, SEEK_SET);
    size_t bytesRead = fread(outData.data(), 1, record.size, fileHandle);

    return (bytesRead == record.size);
}