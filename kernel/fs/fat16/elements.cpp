#include <fat16.h>

// encodeElement is supposed to encode fat16Element into byte array
uint8_t* Fat16::encodeElement(fat16Element *tData) {
    uint8_t *resultData = (uint8_t*)malloc(32);
    memset(resultData, 0x0, FAT16_MAX_FILENAME + FAT16_MAX_FILE_EXTENSION);
    memccpy(resultData, tData->filename, 0, FAT16_MAX_FILENAME);
    memccpy((resultData+0x08), tData->filenameExtension, 0, FAT16_MAX_FILE_EXTENSION);
    resultData[0x0b] = tData->attributes;
    resultData[0x1a] = tData->firstBlockId % 0x100;
    resultData[0x1a+1] = tData->firstBlockId / 0x100;

    // encoding dataSize
    resultData[0x1c] = tData->dataSize % 0x100;
    resultData[0x1c+1] = (tData->dataSize >>  8) % 0x100;
    resultData[0x1c+2] = (tData->dataSize >> 16) % 0x100;
    resultData[0x1c+3] = (tData->dataSize >> 24) % 0x100;
    return resultData;
}

// encodeElement is supposed to decode byte array into fat16Element
fat16Element Fat16::decodeElement(uint8_t *tData) {
    fat16Element resultElement;
    memset(resultElement.filename, 0x0, FAT16_MAX_FILENAME);
    memset(resultElement.filenameExtension, 0x0, FAT16_MAX_FILE_EXTENSION);
    memccpy(resultElement.filename, tData, 0, FAT16_MAX_FILENAME);
    memccpy(resultElement.filenameExtension, (tData+0x08), 0, FAT16_MAX_FILE_EXTENSION);
    resultElement.attributes = tData[0x0b];
    resultElement.firstBlockId = tData[0x1b] * 0x100 + tData[0x1a];
    resultElement.dataSize = (tData[0x1c] +
                             (tData[0x1c+1] <<  8) +
                             (tData[0x1c+2] << 16) +
                             (tData[0x1c+3] << 24));
    return resultElement;
}

// saveElement is supposed to save encoded fat16Elements in a sector.
// The func'll return False if there is an Element with the same name
// and return True if there isn't.
bool Fat16::saveElement(uint8_t *tSegment, uint16_t tSectorStart, uint8_t *tData) {
    int16_t saveWithOffset = -1;
    bool hasElementWithTheSameName = false;
    for (uint16_t elementOffset = 0; elementOffset < bytesPerCluster; elementOffset += 32) {
        // It means there is no files after element with 1st letter=0
        if (tSegment[elementOffset] == 0x00 && saveWithOffset == -1) {
            saveWithOffset = elementOffset;
        }
        // It means that file was delled. Let's replace it
        if (tSegment[elementOffset] == 0xe5 && saveWithOffset == -1) {
            saveWithOffset = elementOffset;
        }
    }
    if (saveWithOffset == -1 || hasElementWithTheSameName) {
        return false;
    }
    memcpy(tSegment+saveWithOffset, tData, 32);
    disk->seek(tSectorStart);
    disk->writeSector(tSegment);
    return true;
}

bool Fat16::saveElement(uint16_t tSectorStart, uint8_t *tData) {
    disk->seek(tSectorStart); 
    uint8_t *segment = disk->readSector(); // reading segment
    return saveElement(segment, tSectorStart, tData);
}

bool Fat16::saveElement(fat16Element *tHodler, uint8_t *tData) {
    disk->seek(tHodler->firstBlockId); 
    uint8_t *segment = disk->readSector(); // reading segment
    return saveElement(segment, tHodler->firstBlockId, tData);
}

// findElementWithName is supposed to find an element with name
// in sector byte array.
fat16Element Fat16::findElementWithName(uint8_t *tData, const char* tFilename, 
                                        const char* tFilenameExtension) {
    fat16Element tmpElement;
    uint16_t filenameOffset, extensionOffset;
    bool wrongFilename = false;
    for (uint16_t elementOffset = 0; elementOffset < bytesPerCluster; elementOffset += 32) {
        wrongFilename = false;
        filenameOffset = elementOffset;
        extensionOffset = elementOffset + FAT16_MAX_FILENAME;
        for (uint8_t letter = 0; tFilename[letter] != 0 
                                && letter < FAT16_MAX_FILENAME; letter++) {
            wrongFilename |= (tData[filenameOffset+letter] != tFilename[letter]);
        }
        for (uint8_t letter = 0; tFilenameExtension[letter] != 0 
                                && letter < FAT16_MAX_FILE_EXTENSION; letter++) {
            wrongFilename |= (tData[extensionOffset+letter] != tFilenameExtension[letter]);
        }
        if (!wrongFilename) {
            return decodeElement(tData+elementOffset);
        }
    }
    // not found sign
    tmpElement.attributes = 0xff;
    return tmpElement;
}

fat16Element Fat16::findElementWithName(uint16_t tSectorStart, const char* tFilename, 
                                        const char* tFilenameExtension) {
    disk->seek(tSectorStart); 
    uint8_t *segment = disk->readSector(); // reading segment
    return findElementWithName(segment, tFilename, tFilenameExtension);
}

fat16Element Fat16::findElementWithName(fat16Element *tHodler, const char* tFilename, 
                                        const char* tFilenameExtension) {
    disk->seek(tHodler->firstBlockId); 
    uint8_t *segment = disk->readSector(); // reading segment
    return findElementWithName(segment, tFilename, tFilenameExtension);
}

// convertToVfs is supposed to convert fat16Element into vfsElement
void Fat16::convertToVfs(fat16Element *tFat16Element, vfsElement *tVfsElement) {
    memset(tVfsElement->filename, 0x0, 8);
    memset(tVfsElement->filenameExtension, 0x0, 3);
    memccpy(tVfsElement->filename, tFat16Element->filename, 0x0, 8);
    memccpy(tVfsElement->filenameExtension, tFat16Element->filenameExtension, 0x0, 3);
    tVfsElement->attributes = tFat16Element->attributes;
}

// setFilename is supposed to set Fat16Element's filename
void Fat16::setFilename(fat16Element *tElement, const char *tFilename) {
    memset(tElement->filename, 0x0, FAT16_MAX_FILENAME);
    memccpy(tElement->filename, tFilename, 0x0, FAT16_MAX_FILENAME);
}

// setFileExtension is supposed to set Fat16Element's file extension
void Fat16::setFileExtension(fat16Element *tElement, const char *tFileExtension) {
    memset(tElement->filenameExtension, 0x0, FAT16_MAX_FILE_EXTENSION);
    memccpy(tElement->filenameExtension, tFileExtension, 0x0, FAT16_MAX_FILE_EXTENSION);
}

// setAttribute is supposed to set Fat16Element's attributes
void Fat16::setAttribute(fat16Element *tElement, uint8_t tAttr) {
    tElement->attributes = tAttr;
}

// setFirstCluster is supposed to set Fat16Element's cluster
void Fat16::setFirstCluster(fat16Element *tElement, uint16_t tCluster) {
    tElement->firstBlockId = tCluster;
}