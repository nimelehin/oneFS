#include <fat16.h>

fat16Element Fat16::cd(const char *tPath) {
    uint16_t tPathSize = strlen(tPath);
    assert(tPathSize > 0 && tPath[0] == '/');

    disk->seek(rootDirStart);
    uint8_t *curretSector = disk->readSector();

    fat16Element tmpElement;
    tmpElement.attributes = 0x11; // root folder sign
    tmpElement.firstBlockId = 0;
    char currentFolderName[8];
    memset(currentFolderName, 0x0, 8);
    uint8_t nxtChar = 0;

    for (int ind = 1; ind < tPathSize; ind++) {
        if (tPath[ind] == '/') {
            tmpElement = findElementWithName(curretSector, currentFolderName);
            assert(tmpElement.attributes == 0x10 || tmpElement.attributes == 0x11);
            memset(currentFolderName, 0x0, 8);
            nxtChar = 0;
            disk->seek(sectorAddressOfElement(&tmpElement));
            curretSector = disk->readSector();
        } else {
            currentFolderName[nxtChar++] = tPath[ind];
        }
    }
    return tmpElement;
}

void Fat16::writeFile(const char *tPath, const char *tFilename, const char *tFilenameExtension, const char *tData, uint16_t tDataSize) {
    fat16Element saveToFolder = cd(tPath);
    
    // creating fat16 file
    fat16Element newFile;
    newFile.attributes = 0x01;
    memset(newFile.filename, 0x0, 8);
    memccpy(newFile.filename, tFilename, 0, 8);
    memset(newFile.filenameExtension, 0x0, 3);
    memccpy(newFile.filenameExtension, tFilenameExtension, 0, 3);
    newFile.dataSize = tDataSize;

    for (int i = 0; i < 8; i++) {
        std::cout << (int)newFile.filename[i] << "\n";
    }
    for (int i = 0; i < 3; i++) {
        std::cout << (int)newFile.filenameExtension[i] << "\n";
    }

    // finding sector for the folder
    newFile.firstBlockId = findFreeCluster();
    takeClusterWithId(newFile.firstBlockId);
    std::cout << newFile.firstBlockId << "\n";

    uint16_t currentCluster = newFile.firstBlockId;
    uint16_t dataBytesPerSector = bytesPerSector * sectorsPerCluster - 2;
    uint8_t *clusterData = (uint8_t*)malloc(dataBytesPerSector + 2);
    for (uint16_t currentByte = 0; currentByte < tDataSize; currentByte++) {
        clusterData[currentByte % dataBytesPerSector] = tData[currentByte];
        if (currentByte == tDataSize - 1) {
            clusterData[bytesPerSector * sectorsPerCluster - 2] = 0xff;
            clusterData[bytesPerSector * sectorsPerCluster - 1] = 0xff;
            disk->seek(sectorAddressOfDataCluster(currentCluster));
            disk->writeSector(clusterData);
        } else if (currentByte % dataBytesPerSector == dataBytesPerSector - 1) {
            uint16_t newCluster = extendClusterWithId(currentCluster);
            clusterData[bytesPerSector * sectorsPerCluster - 2] = newCluster % 0x100;
            clusterData[bytesPerSector * sectorsPerCluster - 1] = newCluster / 0x100;
            disk->seek(sectorAddressOfDataCluster(currentCluster));
            disk->writeSector(clusterData);
            currentCluster = newCluster;
        }
    }

    std::cout << "Saving File in " << sectorAddressOfElement(&saveToFolder) << "\nEND\n";
    uint8_t *fdata = encodeElement(&newFile);
    saveElement(sectorAddressOfElement(&saveToFolder), fdata);
}

// TODO use dataSize
uint8_t* Fat16::readFile(const char *tPath, const char *tFilename, const char *tFilenameExtension) {
    fat16Element* elementsInDir = getFilesInDir(tPath);
    char filename[8];
    memset(filename, 0x0, 8);
    memccpy(filename, tFilename, 0, 8);
    char filenameExtension[3];
    memset(filenameExtension, 0x0, 3);
    memccpy(filenameExtension, tFilenameExtension, 0, 3);

    // searching for file in Dir
    bool found = false;
    uint8_t elementId = 0;
    for (; !found && elementId < 16; elementId++){
        found = strncmp(elementsInDir[elementId].filename, filename, 8) == 0;
        found &= strncmp(elementsInDir[elementId].filenameExtension, tFilenameExtension, 3) == 0;
    }
    if (!found) {
        std::cout << "NO such file\n";
        return nullptr;
    }
    elementId--; // Come back to found element
    fat16Element file = elementsInDir[elementId];

    // extracting data from the file
    assert(file.attributes < 0x10);
    uint8_t *clusterData;
    uint16_t nextCluster = file.firstBlockId;
    uint8_t *resultData = (uint8_t*)malloc(file.dataSize + 1);
    std::cout << file.dataSize << "\n";
    resultData[file.dataSize] = 0;
    uint16_t nxtDataByte = 0;
    do {
        disk->seek(sectorAddressOfDataCluster(nextCluster));
        clusterData = disk->readSector();
        for (int nxtClusterByte = 0; clusterData[nxtClusterByte] != 0 && nxtClusterByte < bytesPerCluster-2; nxtClusterByte++) {
            resultData[nxtDataByte++] = clusterData[nxtClusterByte];
        }
        nextCluster = (clusterData[bytesPerCluster-1] << 8) + clusterData[bytesPerCluster-2]; 
        delete clusterData;
    } while (nextCluster != 0xffff);
    delete elementsInDir;
    return resultData;  
}