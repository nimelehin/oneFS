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
    fat16Element holderFolder = cd(tPath);
    disk->seek(sectorAddressOfElement(&holderFolder));
    uint8_t *holderFolderData = disk->readSector();
    fat16Element writableFile = findElementWithName(holderFolderData, tFilename, tFilenameExtension);
    std::cout << (int)writableFile.attributes << "\n";
    // if file does not exist lets create it
    if (writableFile.attributes == 0xff) {
        setFilename(&writableFile, tFilename);
        setFileExtension(&writableFile, tFilenameExtension);
        setAttribute(&writableFile, 0x01);
        setFirstCluster(&writableFile, allocateCluster());
    }
    writableFile.dataSize = tDataSize;

    std::cout << writableFile.firstBlockId << "\n";

    uint16_t saveToCluster = writableFile.firstBlockId;
    uint16_t dataBytesPerCluster = bytesPerCluster - 2;
    uint8_t *clusterData = (uint8_t*)malloc(dataBytesPerCluster + 2);

    for (uint16_t firstByteToCluster = 0; firstByteToCluster < tDataSize; 
                                                firstByteToCluster+=dataBytesPerCluster) {
        memcpy(clusterData, tData+firstByteToCluster, dataBytesPerCluster);
        bool lastCluster = firstByteToCluster + dataBytesPerCluster > tDataSize;
        uint16_t nxtCluster = lastCluster ? getNextCluster(saveToCluster) : 0xffff;
        disk->seek(sectorAddressOfDataCluster(saveToCluster));
        disk->writeSector(clusterData);
        saveToCluster = nxtCluster;
    }

    std::cout << "Saving File in " << sectorAddressOfElement(&holderFolder) << "\nEND\n";
    uint8_t *fdata = encodeElement(&writableFile);
    saveElement(sectorAddressOfElement(&holderFolder), fdata);
    free(clusterData);
    free(holderFolderData);
}

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
        delete[] clusterData;
    } while (nextCluster != 0xffff);
    delete[] elementsInDir;
    return resultData;  
}