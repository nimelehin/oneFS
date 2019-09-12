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

uint16_t Fat16::findFreeCluster() {
    for (uint32_t sector = 0; sector < sectorsPerFAT; sector++) {
        disk->seek(startOfFATs + sector * bytesPerSector);
        uint8_t *data = disk->readSector();
        for (uint16_t i = 0; i < 512; i+=2) {
            uint16_t result = data[i + 1] * 0x100 + data[i];
            if (result == 0) {
                return (sector * bytesPerSector + i) / 2 - 2;
            }
        }
    }
    return 0;
}

bool Fat16::editClusterWithId(uint16_t tBlockId, uint16_t tNewValue) {
    uint16_t recordIdInFAT = tBlockId + 2;
    uint16_t sectorOfFATWithRecord = recordIdInFAT / (bytesPerSector / 2);
    uint16_t recordIdInSectorOfFat = 2 * (recordIdInFAT % (bytesPerSector / 2));
    disk->seek(startOfFATs + sectorOfFATWithRecord);
    uint8_t *data = disk->readSector();
    uint16_t result = data[recordIdInSectorOfFat + 1] * 0x100 + data[recordIdInSectorOfFat];
    assert(result == 0);
    data[recordIdInSectorOfFat] = tNewValue % 0x100, data[recordIdInSectorOfFat + 1] = tNewValue / 0x100;
    disk->seek(startOfFATs + sectorOfFATWithRecord);
    disk->writeSector(data);
    return true;
}

bool Fat16::takeClusterWithId(uint16_t tBlockId) {
    return editClusterWithId(tBlockId, 0xffff);
}

bool Fat16::freeClusterWithId(uint16_t tBlockId) {
    return editClusterWithId(tBlockId, 0x0000);
}

uint16_t Fat16::extendClusterWithId(uint16_t tBlockId) {
    uint16_t newBlockId = findFreeCluster();
    editClusterWithId(tBlockId, newBlockId);
    return newBlockId;
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
    resultData[0x1b] = tData->firstBlockId / 0x100;
    resultData[0x1a] = tData->firstBlockId % 0x100;
    return resultData;
}

fat16Element Fat16::decodeElement(uint8_t *tData) {
    fat16Element resultElement;
    for (uint8_t i = 0; i < 8; i++) {
        resultElement.filename[i] = tData[i];
    }
    for (uint8_t i = 0; i < 3; i++) {
        resultElement.filenameExtension[i] = tData[0x08 + i];
    }
    resultElement.attributes = tData[0x0b];
    resultElement.firstBlockId = tData[0x1b] * 0x100 + tData[0x1a];
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
    disk->seek(tSegmentStart);
    disk->writeSector(writeTo);
    return true;
}

fat16Element Fat16::findElementWithName(uint8_t *tData, char* filename, char* filenameExtension) {
    fat16Element tmpElement;
    uint8_t tmpData[32];
    for (int i = 0; i < bytesPerSector; i += 32) {
        memcpy(tmpData, tData + i, 32);
        tmpElement = decodeElement(tmpData);
        for (int j = 0; j < 7; j++) {
            if (tmpElement.filename[j] != filename[j]) {
                continue;
            }
        }
        return tmpElement;
    }
    // not found sign
    tmpElement.attributes = 0xff;
    return tmpElement;
}

bool Fat16::mkdir(char *tPath, char *tFolderName) {
    
    fat16Element saveToFolder = cd(tPath);
    std::cout << (int)saveToFolder.attributes << "PRO\n";
    std::cout << saveToFolder.filename << "PRO2\n";

    // creating fat16 folder
    fat16Element newFolder;
    newFolder.attributes = 0x10;
    for (int i = 0; i < 8; i++) {
        newFolder.filename[i] = 0x20;
    }
    for (int i = 0; i < 8; i++) {
        if (tFolderName[i] == 0) break;
        newFolder.filename[i] = tFolderName[i];
    }

    // finding sector for the folder
    newFolder.firstBlockId = findFreeCluster();
    takeClusterWithId(newFolder.firstBlockId);
    
    // writing to the disk
    std::cout << "Saving in " << sectorAddressOfDataBlock(&saveToFolder) << "\nEND\n";
    uint8_t *fdata = encodeElement(&newFolder);
    return saveElement(sectorAddressOfDataBlock(&saveToFolder), fdata);
}

void Fat16::dummyFileCreation() {
    mkdir("/", "a");
    //createFile("/", "hello", "txt")
    //auto tmp = cd("/Hello3/");
    std::cout << "\n\n";
    mkdir("/a/", "b");
    //mkdir("/", "c");
}

fat16Element Fat16::cd(char *tPath) {
    uint16_t tPathSize = strlen(tPath);
    assert(tPathSize > 0 && tPath[0] == '/');

    disk->seek(rootDirStart);
    uint8_t *data = disk->readSector();
    
    fat16Element tmpElement;
    tmpElement.attributes = 0x11; // root folder sign
    char currentFolderName[8];
    for (int i = 0; i < 8; i++) {
        currentFolderName[i] = 0x20;
    }
    uint8_t nxtChar = 0;

    for (int ind = 1; ind < tPathSize; ind++) {
        if (tPath[ind] == '/') {
            tmpElement = findElementWithName(data, currentFolderName);
            assert(tmpElement.attributes != 0xff);
            for (int i = 0; i < 8; i++) {
                std::cout << tmpElement.filename[i];
                currentFolderName[i] = 0x20;
            }
            nxtChar = 0;
            disk->seek(sectorAddressOfDataBlock(&tmpElement));
            data = disk->readSector();
        } else {
            currentFolderName[nxtChar++] = tPath[ind];
        }
    }
    std::cout << "CD " << tmpElement.filename << "\n";
    std::cout << "CD " << (int)tmpElement.attributes << "\n";
    std::cout << "CD " << (int)tmpElement.firstBlockId << "\n";
    return tmpElement;
}

DirDescriptor* Fat16::ls(char *tPath, uint16_t tPathSize) {
    fat16Element tmpElement = cd(tPath);
    disk->seek(sectorAddressOfDataBlock(&tmpElement));
    uint8_t *data = disk->readSector();
    for (uint16_t offset = 0; offset < bytesPerSector; offset += 32) {
        uint8_t *element = (uint8_t*)malloc(32);
        memcpy(element, data+offset, 32);
        tmpElement = decodeElement(element); 
        if (tmpElement.attributes == 0x10) {
            std::cout << "Folder ";
            for (int i = 0; i < 8; i++) {
                std::cout << tmpElement.filename[i];
            }
            std::cout << "\n";
        }
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

uint32_t Fat16::sectorAddressOfDataBlock(fat16Element *tElement) {
    return (tElement->attributes == 0x11 ? rootDirStart : 
            bytesPerSector * tElement->firstBlockId + rootDirStart + rootEntries * 32);
}