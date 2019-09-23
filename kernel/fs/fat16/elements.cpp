#include <fat16.h>

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

fat16Element Fat16::findElementWithName(uint8_t *tData, const char* filename, const char* filenameExtension) {
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

fat16Element* Fat16::getFilesInDir(const char *tPath) {
    fat16Element tmpElement = cd(tPath);
    disk->seek(sectorAddressOfElement(&tmpElement));
    uint8_t *data = disk->readSector();
    fat16Element *result = new fat16Element[16];
    int addId = 0;
    for (uint16_t offset = 0; offset < bytesPerSector; offset += 32) {
        uint8_t *element = (uint8_t*)malloc(32);
        memcpy(element, data+offset, 32);
        tmpElement = decodeElement(element);
        result[addId++] = tmpElement;
    }
    return result;
}