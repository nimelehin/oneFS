#include <fat16.h>
#include <iostream>

Fat16::Fat16(DiskDriver *t_disk): FileSystem(t_disk) {
    readParams();
}

bool Fat16::testDisk(DiskDriver *t_disk) {
    t_disk->seek(0);
    unsigned char* res = t_disk->readSector();
    char fat16Signature[] = {'F', 'A', 'T', '1', '6', 0x20, 0x20, 0x20};
    bool sigCorrect = true;
    for (uint16_t i = 0x36; i < 0x36 + 8; i++) {
        sigCorrect &= (fat16Signature[i - 0x36] == res[i]);
    }
    return sigCorrect;
}

uint16_t Fat16::findFreeBlock() {
    for (uint16_t sector = 0; sector < 1; sector++) {
        disk->seek(startOfFATs + sector * bytesPerSector);
        uint8_t *data = disk->readSector();
        for (uint8_t i = 0; i < 512; i+=2) {
            uint16_t result = data[i + 1] * 0x100 + data[i];
            if (result == 0) {
                return (sector * bytesPerSector + i) / 2 - 2;
            }
        }
    }
    return 0;
}

void Fat16::readFile(char *t_path, char *t_filename) {}
void Fat16::writeFile(char *t_path, char *t_filename, char *t_data) {}
void Fat16::mkdir(char *t_path) {}
bool Fat16::isAttached() {
    return true;
}

uint8_t* Fat16::encodeElement(fat16Element *tData) {
    uint8_t *resultData = (uint8_t*)malloc(32);
    for (uint8_t i = 0; i < 8; i++) {
        resultData[i] = tData->filename[i];
    }
    for (uint8_t i = 0; i < 3; i++) {
        resultData[0x08 + i] = tData->filenameExtension[i];
    }
    resultData[0x0b] = tData->attributes;
    return resultData;
}

fat16Element* Fat16::decodeElement(uint8_t *tData) {
    fat16Element *resultElement = new fat16Element;
    for (uint8_t i = 0; i < 8; i++) {
        resultElement->filename[i] = tData[i];
    }
    for (uint8_t i = 0; i < 3; i++) {
        resultElement->filenameExtension[i] = tData[0x08 + i];
    }
    resultElement->attributes = tData[0x0b];
    return resultElement;
}



void Fat16::dummyFileCreation() {
    std::cout << "Adding random file to root dir: ";
    disk->seek(rootDirStart);
    uint8_t *data = disk->readSector();

    fat16Element r;
    r.attributes = 0x10;
    r.filename[0] = 'H';
    r.filename[1] = 'e';
    r.filename[2] = 'l';
    r.filename[3] = '0';
    r.filenameExtension[0] = 'd';
    r.filenameExtension[1] = 'e';
    r.filenameExtension[2] = 'l';
    r.sectorWithData = findFreeBlock();
    std::cout << r.sectorWithData << "\n";

    uint8_t *fdata = encodeElement(&r);
    for (int i = 0; i < 32; i++){
        std::cout << fdata[i];
    }
    std::cout << "\n\n";

    memcpy(data, fdata, 32);
    disk->seek(rootDirStart);
    disk->writeSector(data);
}

DirDescriptor* Fat16::ls(char *tPath, uint16_t tPathSize) {
    assert(tPathSize > 0 && tPath[0] == '/');
    disk->seek(rootDirStart);
    uint8_t *data = disk->readSector();
    for (uint16_t i = 0; i < rootEntries; i++) {
        uint32_t offset = 32 * i;
        uint8_t *element = (uint8_t*)malloc(32);
        memcpy(element, data+offset, 32);
        fat16Element* result = decodeElement(element); 
        if (result->attributes == 0x10) {
            std::cout << "Folder ";
            for (int i = 0; i < 8; i++) {
                std::cout << result->filename[i];
            }
            std::cout << "\n";
        }
        delete result;
    }
}

void Fat16::readParams() {
    disk->seek(0);
    unsigned char* data = disk->readSector();
    bytesPerSector = data[0x0c] * 0x100 + data[0x0b];
    sectorsPerCluster = data[0x0d];
    reservedSectors = data[0x0f] * 0x100 + data[0x0e];
    startOfFATs = reservedSectors * bytesPerSector;
    numberOfFATs = data[0x10];
    sectorsPerFAT = data[0x17] * 0x100 + data[0x16];
    rootEntries = data[0x12] * 0x100 + data[0x11];
    rootDirStart = bytesPerSector * reservedSectors + numberOfFATs * sectorsPerFAT * bytesPerSector; 
    dataSegStart = rootDirStart + rootEntries * 32;
}