/*
    oneFS
    Folder Cache
    |   is supposed to manage folder cache to make cd cmd faster.
    |   Folder cache can find the folder's sector with data using
    |   parent's sector id and folder name. LFU algo is used.
    |   Each record consists of tParentSector(2 bytes), 
    |   tDirName(8 bytes), neededSector(2 bytes), usedCount(4 bytes);
*/

#include <fat16/dir_cache.h>


Fat16DirCache::Fat16DirCache() {
    mDirCache = (uint8_t*)malloc(FAT16_DIR_CACHE_CAPACITY * FAT16_DIR_CACHE_ENTITY_SIZE);
}

// findEntity is supposed to find entity in the cache table
// Return a row id in that table (not offset) and 0xff if there is no
uint8_t Fat16DirCache::findEntity(uint16_t tParentSector, const char* tDirName) {
    for (uint8_t entity = 0; entity < FAT16_DIR_CACHE_CAPACITY; entity++) {
        uint16_t entityStart = entity * FAT16_DIR_CACHE_ENTITY_SIZE;
        if (!cmpParentSector(mDirCache+entityStart, tParentSector)) {
            continue;
        }
        if (!cmpDirName(mDirCache+entityStart, tDirName)) {
            continue;
        }
        return entityStart;
    }
    return 0xff;
}

// get is supposed to return cluster from cache if there is.
// Return -1 if there is no such entity in cache
int16_t Fat16DirCache::get(uint16_t tParentSector, const char* tDirName) {
    if (mDirCache == nullptr) {
        std::cout << "Some problems with cache";
        return -1;
    }
    uint8_t rowId = findEntity(tParentSector, tDirName);
    if (rowId == 0xff) {
        return -1;
    }
    uint16_t entityStart = rowId * FAT16_DIR_CACHE_ENTITY_SIZE;
    return getSector(mDirCache+entityStart);
}

void Fat16DirCache::update(uint16_t tParentSector, const char* tDirName, uint16_t sector) {

}

void Fat16DirCache::invalidate(uint16_t tParentSector, const char* tDirName, uint16_t sector) {

}

Fat16DirCache::~Fat16DirCache() {
    free(mDirCache);
}

uint16_t Fat16DirCache::getSector(const uint8_t *tData) {
    return (uint16_t(tData[FAT16_MAX_FILENAME+3]) << 8) + tData[FAT16_MAX_FILENAME+2];
}

bool Fat16DirCache::cmpParentSector(const uint8_t *tData, uint16_t tParentSector) {
    return tParentSector == ((uint16_t(tData[1]) << 8) + tData[0]);
}

bool Fat16DirCache::cmpDirName(const uint8_t *tData, const char* tDirName) {
    uint8_t filenameOffset = 2;
    bool wrongFilename = false;
    for (uint8_t letter = 0; tDirName[letter] != 0 
                            && letter < FAT16_MAX_FILENAME; letter++) {
        wrongFilename |= (tData[filenameOffset+letter] != tDirName[letter]);
    }
    return !wrongFilename;
}
