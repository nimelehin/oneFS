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
    for (uint16_t sector = 0; sector < sectorsPerFAT; sector++) {
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

bool Fat16::takeBlockWithId(uint16_t tBlockId) {
    uint16_t recordIdInFAT = tBlockId + 2;
    uint16_t sectorOfFATWithRecord = recordIdInFAT / (bytesPerSector / 2);
    uint16_t recordIdInSectorOfFat = 2 * (recordIdInFAT % (bytesPerSector / 2));

    std::cout << "recordIdInFAT " << recordIdInFAT << "\n";
    std::cout << "sectorOfFATWithRecord " << sectorOfFATWithRecord << "\n";
    std::cout << "recordIdInSectorOfFat " << recordIdInSectorOfFat << "\n";

    disk->seek(startOfFATs + sectorOfFATWithRecord);
    uint8_t *data = disk->readSector();
    uint16_t result = data[recordIdInSectorOfFat + 1] * 0x100 + data[recordIdInSectorOfFat];
    assert(result == 0);
    data[recordIdInSectorOfFat] = 0xff, data[recordIdInSectorOfFat + 1] = 0xff;
    disk->seek(startOfFATs + sectorOfFATWithRecord);
    disk->writeSector(data);
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

bool Fat16::saveElement(uint16_t tSegmentStart, uint8_t *tData) {
    disk->seek(tSegmentStart);
    uint8_t *writeTo = disk->readSector();
    int16_t offset = -1;
    for (int i = 0; i < bytesPerSector; i += 32) {
        if (writeTo[i] == 0xe5 || writeTo[i] == 0x00) {
            offset = i;
            break;
        }
    }
    if (offset == -1) {
        return false;
    }
    memcpy(writeTo+offset, tData, 32);
    disk->seek(rootDirStart);
    disk->writeSector(writeTo);
    return true;
}

void Fat16::mkdir(char *tPath, char *tFolderName) {
    fat16Element newFolder;
    newFolder.attributes = 0x10;
    for (int i = 0; i < 8; i++) {
        newFolder.filename[i] = 0x20;
    }
    for (int i = 0; i < 8; i++) {
        if (tFolderName[i] == 0) break;
        newFolder.filename[i] = tFolderName[i];
        std::cout << newFolder.filename[i] << " ";
    }
    newFolder.firstBlockId = findFreeBlock();
    std::cout << newFolder.firstBlockId << "\n";
    takeBlockWithId(newFolder.firstBlockId);
    
    // writing to the disk
    uint8_t *fdata = encodeElement(&newFolder);
    std::cout << saveElement(rootDirStart, fdata) << "-1--1";
}

void Fat16::dummyFileCreation() {
    std::cout << "Adding random file to root dir: ";
    mkdir("", "Hello3");
    mkdir("", "Hello4");
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

void Fat16::readFile(char *t_path, char *t_filename) {}
void Fat16::writeFile(char *t_path, char *t_filename, char *t_data) {}
bool Fat16::isAttached() {
    return true;
}