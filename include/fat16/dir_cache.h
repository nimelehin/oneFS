#ifndef FAT16_DirCache_H
#define FAT16_DirCache_H

#include <iostream>
#include <cstdlib>
#define FAT16_DIR_CACHE_CAPACITY 48
#define FAT16_DIR_CACHE_ENTITY_SIZE 16
#define FAT16_MAX_FILENAME 8
#define FAT16_MAX_FILE_EXTENSION 3

class Fat16DirCache {
private:
    uint8_t *mDirCache;
    uint8_t findEntity(uint16_t tParentSector, const char* tDirName);
    bool cmpParentSector(const uint8_t *tData, uint16_t tParentSector);
    bool cmpDirName(const uint8_t *tData, const char* tDirName);
    uint16_t getSector(const uint8_t *tData);
public:
    Fat16DirCache();
    int16_t get(uint16_t tParentSector, const char* tDirName);
    void update(uint16_t tParentSector, const char* tDirName, uint16_t sector);
    void invalidate(uint16_t tParentSector, const char* tDirName, uint16_t sector);
    ~Fat16DirCache();
};

#endif //FAT16_DirCache_H