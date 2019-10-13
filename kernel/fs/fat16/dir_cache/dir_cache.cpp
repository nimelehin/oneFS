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

#define DIR_CACHE_COUNTER_OFFSET 12
#define DIR_CACHE_PSECTOR_OFFSET 0
#define DIR_CACHE_RSECTOR_OFFSET 10
#define DIR_CACHE_DIRNAME_OFFSET 2


Fat16DirCache::Fat16DirCache() {
    mDirCache = (uint8_t*)malloc(FAT16_DIR_CACHE_CAPACITY * FAT16_DIR_CACHE_ENTITY_SIZE);
}

Fat16DirCache::~Fat16DirCache() {
    free(mDirCache);
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
    increaseCounter(rowId);
    uint16_t entityStart = rowId * FAT16_DIR_CACHE_ENTITY_SIZE;
    return getSector(mDirCache+entityStart);
}

// update is supposed to update cache taking a new call in mind.
// Should be called to calculate right cache's set.
void Fat16DirCache::update(uint16_t tParentSector, const char* tDirName, uint16_t sector) {
    if (mDirCache == nullptr) {
        std::cout << "Some problems with cache";
        return;
    }
    uint8_t rowId = findEntity(tParentSector, tDirName);
    
    
    if (rowId == 0xff) {
        // has't the element
        uint8_t worstEntityRowId = findWorstEntity();
        uint16_t worstEntityOffset = rowId * FAT16_DIR_CACHE_ENTITY_SIZE;
        setParentSector(mDirCache+worstEntityOffset, tParentSector);
        setName(mDirCache+worstEntityOffset, tDirName);
        setResultSector(mDirCache+worstEntityOffset, sector);
        setCounter(mDirCache+worstEntityOffset, 0);
    } else {
        // has the element
        increaseCounter(rowId);
    }
}

void Fat16DirCache::invalidate(uint16_t tParentSector, const char* tDirName, uint16_t sector) {
    
}

// findWorstEntity is supposed to find entity in the cache table
// with the smallest couter. Row id is returned
uint8_t Fat16DirCache::findWorstEntity() {
    uint32_t leastFoundCounter = getCounter(mDirCache);
    uint8_t worstEntityRowId = 0;
    for (uint8_t entityRowId = 1; entityRowId < FAT16_DIR_CACHE_CAPACITY; entityRowId++) {
        uint16_t entityStart = entityRowId * FAT16_DIR_CACHE_ENTITY_SIZE;
        uint32_t currentCounter = getCounter(mDirCache+entityStart);
        if (currentCounter < leastFoundCounter) {
            leastFoundCounter = currentCounter;
            worstEntityRowId = entityRowId;
        }
    }
    return worstEntityRowId;
}

// increaseCounter is supposed to increase counter of an element.
void Fat16DirCache::increaseCounter(uint8_t rowId) {
    uint16_t entityStart = rowId * FAT16_DIR_CACHE_ENTITY_SIZE;
    uint32_t counter = getCounter(mDirCache+entityStart);
    setCounter(mDirCache+entityStart, counter+1);
}

// setParentSector is supposed to set parent sector of an entity.
void Fat16DirCache::setParentSector(uint8_t *tData, uint16_t tNewValue) {
    tData[DIR_CACHE_PSECTOR_OFFSET+1] = (tNewValue >>  8) % 0x100;
    tData[DIR_CACHE_PSECTOR_OFFSET+0] = (tNewValue >>  0) % 0x100;
}

// setSector is supposed to set result sector of an entity.
void Fat16DirCache::setResultSector(uint8_t *tData, uint16_t tNewValue) {
    tData[DIR_CACHE_RSECTOR_OFFSET+1] = (tNewValue >>  8) % 0x100;
    tData[DIR_CACHE_RSECTOR_OFFSET+0] = (tNewValue >>  0) % 0x100;
}

// setSector is supposed to set result sector of an entity.
void Fat16DirCache::setName(uint8_t *tData, const char *tDirName) {
    memccpy((void*)tData[DIR_CACHE_DIRNAME_OFFSET], tDirName, 0, FAT16_MAX_FILENAME);
}

// setCounter is supposed to set counter of an element.
void Fat16DirCache::setCounter(uint8_t *tData, uint32_t tNewValue) {
    tData[DIR_CACHE_COUNTER_OFFSET+3] = (tNewValue >> 24) % 0x100;
    tData[DIR_CACHE_COUNTER_OFFSET+2] = (tNewValue >> 16) % 0x100;
    tData[DIR_CACHE_COUNTER_OFFSET+1] = (tNewValue >>  8) % 0x100;
    tData[DIR_CACHE_COUNTER_OFFSET+0] = (tNewValue >>  0) % 0x100;
}

// getCounter is supposed to return counter of an element.
uint32_t Fat16DirCache::getCounter(const uint8_t *tData) {
    uint32_t res = uint32_t(tData[DIR_CACHE_COUNTER_OFFSET+3]) << 24 +
                   uint32_t(tData[DIR_CACHE_COUNTER_OFFSET+2]) << 16 +
                   uint32_t(tData[DIR_CACHE_COUNTER_OFFSET+1]) << 8  +
                   uint32_t(tData[DIR_CACHE_COUNTER_OFFSET+0]);
    return res;
}

uint16_t Fat16DirCache::getSector(const uint8_t *tData) {
    return (uint16_t(tData[DIR_CACHE_RSECTOR_OFFSET+1]) << 8) + tData[DIR_CACHE_RSECTOR_OFFSET];
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
