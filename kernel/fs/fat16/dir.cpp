#include <fat16.h>

void Fat16::initDir(uint16_t firstClusterId, uint16_t rootDirClusterId, uint8_t rootDirAttributes) {
    fat16Element rootFolder;
    rootFolder.attributes = 0x10;
    memset(rootFolder.filename, 0x0, 8);
    memccpy(rootFolder.filename, "..", 0, 8);
    memset(rootFolder.filenameExtension, 0x0, 3);
    rootFolder.firstBlockId = rootDirClusterId;
    rootFolder.attributes = rootDirAttributes;
    // writing to the disk
    uint8_t *fdata = encodeElement(&rootFolder);
    saveElement(sectorAddressOfDataCluster(firstClusterId), fdata);
}

bool Fat16::createDir(const char  *tPath, const char *tFolderName) {
    fat16Element saveToFolder = getDir(tPath);
    std::cout << saveToFolder.filename << "\n";
    
    // creating fat16 folder
    fat16Element newFolder;
    newFolder.attributes = 0x10;
    memset(newFolder.filename, 0x0, 8);
    memccpy(newFolder.filename, tFolderName, 0, 8);
    memset(newFolder.filenameExtension, 0x0, 3);

    // finding sector for the folder
    newFolder.firstBlockId = findFreeCluster();
    takeCluster(newFolder.firstBlockId);
    initDir(newFolder.firstBlockId, saveToFolder.firstBlockId, saveToFolder.attributes);
    
    // writing to the disk
    std::cout << "Saving in " << sectorAddressOfElement(&saveToFolder) << "\nEND\n";
    uint8_t *fdata = encodeElement(&newFolder);
    return saveElement(sectorAddressOfElement(&saveToFolder), fdata);
}


fat16Element Fat16::getDir(const char *tPath) {
    uint16_t pathSize = strlen(tPath);
    assert(pathSize > 0 && tPath[0] == '/');

    // creating elements pointed to root dir
    uint16_t currentSector = 0;
    fat16Element tmpElement;
    tmpElement.attributes = 0x11;
    tmpElement.firstBlockId = 0;
    
    char currentDirName[FAT16_MAX_FILENAME];
    char currentDirExtension[FAT16_MAX_FILE_EXTENSION];
    memset(currentDirName, 0x0, FAT16_MAX_FILENAME);
    memset(currentDirExtension, 0x0, FAT16_MAX_FILE_EXTENSION);
    
    uint8_t nextChar = 0;
    uint8_t *dataOfSector = 0;

    for (int ind = 1; ind < pathSize; ind++) {
        if (tPath[ind] == '/') {
            int16_t cacheResultSector = mDirCache.get(currentSector, currentDirName);
            if (cacheResultSector != -1 && ind != pathSize - 1) {
                std::cout << "Cache used\n";
                currentSector = cacheResultSector;
            } else {
                disk->seek(sectorAddressOfDataCluster(currentSector));
                dataOfSector = disk->readSector();
                tmpElement = getElement(dataOfSector, currentDirName, currentDirExtension);
                assert(tmpElement.attributes == 0x10 || tmpElement.attributes == 0x11);
                uint16_t nextSector = tmpElement.firstBlockId;

                // updating cache
                mDirCache.update(currentSector, currentDirName, nextSector);
                
                // update state
                currentSector = nextSector;
                memset(currentDirName, 0x0, 8);
                nextChar = 0;
                free(dataOfSector);
            }
        } else {
            currentDirName[nextChar++] = tPath[ind];
        }
    }
    return tmpElement;
}

vfsDir Fat16::getVfsDir(const char *tPath) {
    fat16Element* elements = getFilesInDir(tPath);
    vfsElement *resultElements = new vfsElement[16];
    vfsElement tmpElement;
    uint8_t nextElementId = 0;
    for (uint8_t i = 0; i < 16; i++) {
        if (elements[i].filename[0] != 0 
            && (uint8_t)elements[i].filename[0] != FAT16_DELETED_SIGN) {
            convertToVfs((elements + i), &tmpElement);
            resultElements[nextElementId++] = tmpElement;
        }
    }
    vfsDir resultDir;
    resultDir.elements = resultElements;
    resultDir.countOfElements = nextElementId;
    return resultDir;
}


fat16Element* Fat16::getFilesInDir(const char *tPath) {
    fat16Element tmpElement = getDir(tPath);
    disk->seek(sectorAddressOfElement(&tmpElement));
    uint8_t *data = disk->readSector();
    fat16Element *result = new fat16Element[16];
    uint8_t addId = 0;
    for (uint16_t offset = 0; offset < bytesPerSector; offset += 32) {
        uint8_t *element = (uint8_t*)malloc(32);
        memcpy(element, data+offset, 32);
        tmpElement = decodeElement(element);
        result[addId++] = tmpElement;
    }
    return result;
}

bool Fat16::hasDir(const char *tPath) {
    uint16_t tPathSize = strlen(tPath);
    assert(tPathSize > 0 && tPath[0] == '/');

    disk->seek(rootDirStart);
    uint8_t *curretSector = disk->readSector();

    fat16Element tmpElement;
    char currentFolderName[8];
    memset(currentFolderName, 0x0, 8);
    char currentFolderExtension[FAT16_MAX_FILE_EXTENSION];
    memset(currentFolderExtension, 0x0, FAT16_MAX_FILE_EXTENSION);
    uint8_t nxtChar = 0;
    
    for (int ind = 1; ind < tPathSize; ind++) {
        if (tPath[ind] == '/') {
            tmpElement = getElement(curretSector, currentFolderName, currentFolderExtension);
            std::cout << currentFolderName << " " << (int)tmpElement.attributes << "\n";
            if (tmpElement.attributes != 0x10 && tmpElement.attributes != 0x11) {
                return false;
            }
            memset(currentFolderName, 0x0, 8);
            nxtChar = 0;
            disk->seek(sectorAddressOfElement(&tmpElement));
            curretSector = disk->readSector();
        } else {
            currentFolderName[nxtChar++] = tPath[ind];
        }
    }
    return true;
}