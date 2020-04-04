#include <fat16.h>

void Fat16::writeFile(const char *tPath, const char *tFilename, const char *tFilenameExtension, const char *tData, uint16_t tDataSize) {
    fat16Element holderFolder = getDir(tPath);
    disk->seek(getSectorAddress(&holderFolder));
    uint8_t *holderFolderData = disk->readSector();
    fat16Element writableFile = getElement(holderFolderData, tFilename, tFilenameExtension);
    bool isFileNew = false;
    // if file does not exist lets create it
    if (writableFile.attributes == 0xff) {
        setFilename(&writableFile, tFilename);
        setFileExtension(&writableFile, tFilenameExtension);
        setAttribute(&writableFile, 0x01);
        setFirstCluster(&writableFile, allocateCluster());
        isFileNew = true;
    }
    setDataSize(&writableFile, tDataSize);

    uint16_t saveToCluster = writableFile.firstBlockId;
    uint16_t lastEditedCluster = 0;
    uint16_t dataBytesPerCluster = bytesPerCluster - 2;
    uint8_t *clusterData = (uint8_t*)malloc(dataBytesPerCluster + 2);

    for (uint16_t firstByteToCluster = 0; firstByteToCluster < tDataSize;
                                                firstByteToCluster+=dataBytesPerCluster) {
        memcpy(clusterData, tData+firstByteToCluster, dataBytesPerCluster);
        bool lastCluster = firstByteToCluster + dataBytesPerCluster > tDataSize;
        uint16_t nxtCluster = lastCluster ? 0xffff : getNextOrExtendCluster(saveToCluster);
        clusterData[dataBytesPerCluster] = nxtCluster % 0x100; 
        clusterData[dataBytesPerCluster+1] = (nxtCluster >> 8) % 0x100;
        disk->seek(getSectorAddress(saveToCluster));
        disk->writeSector(clusterData);
        lastEditedCluster = saveToCluster;
        saveToCluster = nxtCluster;
    }

    if (isFileNew) {
        uint8_t *fdata = encodeElement(&writableFile);
        saveElement(getSectorAddress(&holderFolder), fdata);
    } else {
        makeClusterLast(lastEditedCluster);
    }
    free(clusterData);
    free(holderFolderData);
}

uint8_t* Fat16::readFile(const char *tPath, const char *tFilename, const char *tFilenameExtension, uint16_t tReadOffset, int16_t tLen) {
    
    fat16Element* elementsInDir = getFilesInDir(tPath);
    char filename[8];
    memset(filename, 0x20, 8);
    memccpy(filename, tFilename, 0x20, 8);
    char filenameExtension[3];
    memset(filenameExtension, 0x20, 3);
    memccpy(filenameExtension, tFilenameExtension, 0x20, 3);

    for (int i = 0; i < FAT16_MAX_FILENAME; i++) {
        if (filename[i] == 0) {
            filename[i] = 32;
        }
    }
    for (int i = 0; i < FAT16_MAX_FILE_EXTENSION; i++) {
        if (filenameExtension[i] == 0) {
            filenameExtension[i] = 32;
        }
    }

    // searching for file in Dir
    bool found = false;
    uint8_t elementId = 0;
    for (; !found && elementId < 16; elementId++){
        found = strncmp(elementsInDir[elementId].filename, filename, FAT16_MAX_FILENAME) == 0;
        found &= strncmp(elementsInDir[elementId].filenameExtension, filenameExtension, FAT16_MAX_FILE_EXTENSION) == 0;
    }
    if (!found) {
        std::cout << "NO such file\n";
        return nullptr;
    }
    
    elementId--; // Come back to found element
    fat16Element file = elementsInDir[elementId];

    if (tLen == -1) {
        tLen = file.dataSize - tReadOffset;
    }
    if (tReadOffset + tLen > file.dataSize) {
        return nullptr;
    }

    // extracting data from the file
    assert(file.attributes < 0x10);
    uint8_t *clusterData;
    uint16_t seekBlocks = tReadOffset / (bytesPerCluster-2);
    uint16_t offsetInCluster = tReadOffset % (bytesPerCluster-2);
    uint16_t nextCluster = seekClusters(file.firstBlockId, seekBlocks);
    uint8_t *resultData = (uint8_t*)malloc(tLen + 1);
    resultData[tLen] = 0;
    uint16_t nxtDataByte = 0;
    do {
        disk->seek(getSectorAddress(nextCluster));
        clusterData = disk->readSector();
        for (uint16_t nxtClusterByte = offsetInCluster; clusterData[nxtClusterByte] != 0 
                && nxtDataByte < tLen && nxtClusterByte < bytesPerCluster-2; nxtClusterByte++) {
            resultData[nxtDataByte++] = clusterData[nxtClusterByte];
        }
        nextCluster = (uint16_t(clusterData[bytesPerCluster-1] << 8) + 
                       uint16_t(clusterData[bytesPerCluster-2])); 
        offsetInCluster = 0;
        free(clusterData);
    } while (nextCluster != 0xffff);
    free(elementsInDir);
    return resultData;
}

bool Fat16::deleteDir(const char *tPath, const char *tDirName) {
    std::cout << "Del Dir " << tPath << " " << tDirName << "\n";
    char thisDirPath[256];
    uint16_t holderPathLen = strlen(tPath);
    uint16_t dirNameLen = 0;
    for (dirNameLen; tDirName[dirNameLen] != 0 && dirNameLen < FAT16_MAX_FILENAME; dirNameLen++) {}

    memccpy(thisDirPath, tPath, 0, holderPathLen);
    memccpy(thisDirPath+holderPathLen, tDirName, 0, FAT16_MAX_FILENAME);
    thisDirPath[holderPathLen+dirNameLen] = '/';
    thisDirPath[holderPathLen+dirNameLen+1] = 0;

    fat16Element holderDir = getDir(tPath);
    fat16Element thisDir = getDir(thisDirPath);
    fat16Element* elements = getFilesInDir(thisDirPath);

    for (uint8_t i = 0; i < 16; i++) {
        if (elements[i].filename[0] != 0
            && (elements[i].filename[0] != '.' && elements[i].filename[1] != '.')
            && (uint8_t)elements[i].filename[0] != FAT16_DELETED_SIGN) {
            if (elements[i].attributes == 0x10) {
                deleteDir(thisDirPath, elements[i].filename);
                mDirCache.invalidate(thisDir.firstBlockId, elements[i].filename);
            } else if (elements[i].attributes < 0x10) {
                deleteFile(&thisDir, &elements[i]);
            }
        }
    }
    free(elements);
    return deleteElement(&holderDir, tDirName, "");
}

bool Fat16::deleteFile(fat16Element *tHolderFolder, fat16Element *tFile) {
    std::cout << "Del File " << " " << tFile->filename << "\n";
    return deleteElement(tHolderFolder, tFile->filename, tFile->filenameExtension);
}

bool Fat16::deleteFile(const char *tPath, const char *tFilename, const char *tFilenameExtension) {
    std::cout << "Del File " << tPath << " " << tFilename << "\n";
    fat16Element holderFolder = getDir(tPath);
    return deleteElement(&holderFolder, tFilename, tFilenameExtension);
}
