#include <fat16.h>

uint8_t* Fat16::encodeElement(fat16Element *tData) {
    uint8_t *resultData = (uint8_t*)malloc(32);
    memset(resultData, 0x0, 8+3);
    memccpy(resultData, tData->filename, 0, 8);
    memccpy((resultData+0x08), tData->filenameExtension, 0, 3);
    resultData[0x0b] = tData->attributes;
    resultData[0x1a] = tData->firstBlockId % 0x100;
    resultData[0x1a+1] = tData->firstBlockId / 0x100;

    //writing dataSize
    resultData[0x1c] = tData->dataSize % 0x100;
    resultData[0x1c+1] = (tData->dataSize >>  8) % 0x100;
    resultData[0x1c+2] = (tData->dataSize >> 16) % 0x100;
    resultData[0x1c+3] = (tData->dataSize >> 24) % 0x100;
    return resultData;
}

fat16Element Fat16::decodeElement(uint8_t *tData) {
    fat16Element resultElement;
    memset(resultElement.filename, 0x0, 8);
    memset(resultElement.filenameExtension, 0x0, 3);
    memccpy(resultElement.filename, tData, 0, 8);
    memccpy(resultElement.filenameExtension, (tData+0x08), 0, 3);
    resultElement.attributes = tData[0x0b];
    resultElement.firstBlockId = tData[0x1b] * 0x100 + tData[0x1a];
    resultElement.dataSize = (tData[0x1c] +
                             (tData[0x1c+1] <<  8) +
                             (tData[0x1c+2] << 16) +
                             (tData[0x1c+3] << 24));
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
    std::cout << "Creating with offset " << (int)offset << "\n";
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
        bool wrongFilename = false;
        for (int j = 0; j < 8; j++) {
            if (tmpElement.filename[j] == 0){
                break;
            }
            if (tmpElement.filename[j] != filename[j]) {
                wrongFilename = true;
            }
        }
        if (!wrongFilename)
            return tmpElement;
    }
    // not found sign
    tmpElement.attributes = 0xff;
    return tmpElement;
}