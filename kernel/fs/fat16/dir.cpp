#include <fat16.h>

// initDir is supposed to init dir. Creating ".." element
void Fat16::initDir(uint16_t firstClusterId, uint16_t rootDirClusterId, uint8_t rootDirAttributes) {
    fat16Element rootFolder;
    rootFolder.attributes = FAT16_ELEMENT_FOLDER;
    memset(rootFolder.filename, 0x0, 8);
    memccpy(rootFolder.filename, "..", 0, 8);
    memset(rootFolder.filenameExtension, 0x0, 3);
    rootFolder.firstBlockId = rootDirClusterId;
    rootFolder.attributes = rootDirAttributes;
    // writing to the disk
    uint8_t *fdata = encodeElement(&rootFolder);
    saveElement(getSectorAddress(firstClusterId), fdata);
}

// REQUIRED BY VFS
// createDir is supposed to create a dir in tPath with name tDirName
bool Fat16::createDir(const char *tPath, const char *tFolderName) {
    fat16Element saveToFolder = getDir(tPath);

    // creating fat16 folder
    fat16Element newFolder;
    newFolder.attributes = FAT16_ELEMENT_FOLDER;
    memset(newFolder.filename, 0x0, 8);
    memccpy(newFolder.filename, tFolderName, 0, 8);
    memset(newFolder.filenameExtension, 0x0, 3);

    // finding sector for the folder
    newFolder.firstBlockId = findFreeCluster();
    takeCluster(newFolder.firstBlockId);
    initDir(newFolder.firstBlockId, saveToFolder.firstBlockId, saveToFolder.attributes);

    // writing to the disk
    std::cout << "Creating folder " << newFolder.filename << "\n";
    std::cout << "Saving in " << getSectorAddress(&saveToFolder) << "\n";
    std::cout << "Saving in " << getSectorAddress(&saveToFolder) << "\n";
    std::cout << "Block is " << newFolder.firstBlockId << " or " << getSectorAddress(&newFolder) << "\n";
    uint8_t *fdata = encodeElement(&newFolder);
    bool opStatus = saveElement(getSectorAddress(&saveToFolder), fdata);
    free(fdata);
    return opStatus;
}

// getDir is supposed to return fat16Element of folder in the path
fat16Element Fat16::getDir(const char *tPath) {
    uint16_t pathSize = strlen(tPath);
    assert(pathSize > 0 && tPath[0] == '/');

    // creating elements pointed to root dir
    fat16Element tmpElement;
    tmpElement.attributes = FAT16_ELEMENT_ROOT_FOLDER;
    tmpElement.firstBlockId = 0;

    uint8_t nextChar = 0;
    char currentDirName[FAT16_MAX_FILENAME];
    char currentDirExtension[FAT16_MAX_FILE_EXTENSION];
    memset(currentDirName, 0x0, FAT16_MAX_FILENAME);
    memset(currentDirExtension, 0x0, FAT16_MAX_FILE_EXTENSION);

    uint16_t currentSector = 0;
    uint8_t *dataOfSector = 0;

    for (uint16_t ind = 1; ind < pathSize; ind++) {
        if (tPath[ind] == '/') {
            int16_t cacheResultSector = mDirCache.get(currentSector, currentDirName);
            if (cacheResultSector != -1 && ind != pathSize - 1) {
                currentSector = cacheResultSector;
            } else {
                disk->seek(getSectorAddress(currentSector));
                dataOfSector = disk->readSector();
                tmpElement = getElement(dataOfSector, currentDirName, currentDirExtension);

                if (tmpElement.attributes != FAT16_ELEMENT_ROOT_FOLDER
                    && tmpElement.attributes != FAT16_ELEMENT_FOLDER) {
                    // means folder doens't exist in the path
                    tmpElement.attributes = FAT16_ELEMENT_NULL;
                    return tmpElement;
                }

                uint16_t nextSector = tmpElement.firstBlockId;
                mDirCache.update(currentSector, currentDirName, nextSector);
                currentSector = nextSector;
                free(dataOfSector);
            }
            memset(currentDirName, 0x0, 8);
            nextChar = 0;
        } else {
            currentDirName[nextChar++] = tPath[ind];
        }
    }
    return tmpElement;
}

// REQUIRED BY VFS
// getVfsDir is supposed to convert FAT16 dir to vfsDir standart
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
    free(elements);
    return resultDir;
}

// getFilesInDir is supposed to return array of fat16Element which
// are in the dir
fat16Element* Fat16::getFilesInDir(fat16Element tmpElement) {
    disk->seek(getSectorAddress(&tmpElement));
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

// getFilesInDir is supposed to return array of fat16Element which
// are in the dir
fat16Element* Fat16::getFilesInDir(const char *tPath) {
    fat16Element tmpElement = getDir(tPath);
    return getFilesInDir(tmpElement);
}

// REQUIRED BY VFS
// hasDir is supposed to check if path exists
bool Fat16::hasDir(const char *tPath) {
    fat16Element tmpElement = getDir(tPath);
    return tmpElement.attributes != FAT16_ELEMENT_NULL;
}
