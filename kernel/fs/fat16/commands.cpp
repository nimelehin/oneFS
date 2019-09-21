#include <fat16.h>

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
            disk->seek(sectorAddressOfElement(&tmpElement));
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

vfsDir Fat16::getDir(char *tPath) {
    fat16Element* elements = getFilesInDir(tPath);
    vfsElement *resultElements = new vfsElement[16];
    vfsElement tmpElement;
    uint8_t nextElementId = 0;
    for (uint8_t i = 0; i < 16; i++) {
        convertToVfs((elements + i), &tmpElement);
        resultElements[nextElementId++] = tmpElement;
    }
    vfsDir resultDir;
    resultDir.elements = resultElements;
    resultDir.countOfElements = nextElementId;
}

bool Fat16::createDir(char *tPath, char *tFolderName) {
    fat16Element saveToFolder = cd(tPath);
    
    // creating fat16 folder
    fat16Element newFolder;
    newFolder.attributes = 0x10;
    memset(newFolder.filename, 0x20, 8);
    memccpy(newFolder.filename, tFolderName, 0, 8);

    // finding sector for the folder
    newFolder.firstBlockId = findFreeCluster();
    takeClusterWithId(newFolder.firstBlockId);
    
    // writing to the disk
    std::cout << "Saving in " << sectorAddressOfElement(&saveToFolder) << "\nEND\n";
    uint8_t *fdata = encodeElement(&newFolder);
    return saveElement(sectorAddressOfElement(&saveToFolder), fdata);
}

void Fat16::writeFile(char *tPath, char *tFilename, char *tFilenameExtension, char *tData, uint16_t tDataSize) {
    fat16Element saveToFolder = cd(tPath);
    
    // creating fat16 file
    fat16Element newFile;
    newFile.attributes = 0x01;
    memset(newFile.filename, 0x20, 8);
    memccpy(newFile.filename, tFilename, 0, 8);
    memset(newFile.filenameExtension, 0x20, 3);
    memccpy(newFile.filenameExtension, tFilenameExtension, 0, 3);

    // finding sector for the folder
    newFile.firstBlockId = findFreeCluster();
    takeClusterWithId(newFile.firstBlockId);

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
            clusterData[bytesPerSector * sectorsPerCluster - 2] = currentCluster % 0x100;
            clusterData[bytesPerSector * sectorsPerCluster - 1] = currentCluster / 0x100;
            disk->seek(sectorAddressOfDataCluster(currentCluster));
            disk->writeSector(clusterData);
            currentCluster = newCluster;
        }
    }

    std::cout << "Saving File in " << sectorAddressOfElement(&saveToFolder) << "\nEND\n";
    uint8_t *fdata = encodeElement(&newFile);
    saveElement(sectorAddressOfElement(&saveToFolder), fdata);
}

void Fat16::readFile(char *tPath, char *tFilename) {
    fat16Element* elements = getFilesInDir(tPath);
    fat16Element file;
    char filename[8];
    memset(filename, 0x20, 8);
    memccpy(filename, tFilename, 0, 8);
    bool find = true; 
    for (int i = 0; i < 16; i++){
        find = true;
        for (int letterId = 0; letterId < 8; letterId++){
            if ((elements+i)->filename[letterId] != filename[letterId]) {
                find = false;
                break;
            }
        }
        if (find) {
            file = *(elements+i);
            break;
        }
    }
    if (!find) {
        std::cout << "NO such file\n";
        return;
    }


    uint16_t currentCluster = file.firstBlockId;
    disk->seek(sectorAddressOfDataCluster(currentCluster));
    uint8_t *data = disk->readSector();
    while (1) {
        for (int i = 0; i < 510; i++) {
            std::cout << data[i];
        }
        uint16_t nextBlock = data[511] * 0x100 + data[510];
        if (nextBlock == 0xffff){
            break;
        } else {
            disk->seek(sectorAddressOfDataCluster(nextBlock));
            uint8_t *data = disk->readSector();
        }
    }
}