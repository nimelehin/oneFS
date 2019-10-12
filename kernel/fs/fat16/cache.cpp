/*
    oneFS
    Folder Cache
    |   is supposed to manage folder cache to make cd cmd faster.
    |   Folder cache can find the folder's sector with data using
    |   parent's sector id and folder name. LFU algo is used.
    |   Each record consists of tParentSector(2 bytes), 
    |   tDirName(8 bytes), neededSector(2 bytes), usedCount(4 bytes);
*/

#include <fat16.h>


void Fat16::initDirCache() {
    mDirCache = (uint8_t*)malloc(FAT16_DIR_CACHE_CAPACITY * FAT16_DIR_CACHE_ENTITY_SIZE);
}

int16_t Fat16::getDirCache(uint16_t tParentSector, const char* tDirName) {
    return 0;
}

void Fat16::updateDirCache(uint16_t tParentSector, const char* tDirName, uint16_t sector) {

}

void Fat16::invalidateDirCache(uint16_t tParentSector, const char* tDirName, uint16_t sector) {

}

void Fat16::freeDirCache() {
    free(mDirCache);
}