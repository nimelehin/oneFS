#include <fat16.h>

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