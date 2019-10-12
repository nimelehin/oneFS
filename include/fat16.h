#ifndef FAT16_H
#define FAT16_H

#include <file_system.h>
#include <vfs_elements.h>
#include <fat16/fat16_element.hpp>

#include <iostream> //for test purposes

#define FAT16_MAX_FILENAME 8
#define FAT16_MAX_FILE_EXTENSION 3
#define FAT16_DELETED_SIGN (uint8_t)0xE5
#define FAT16_DIR_CACHE_CAPACITY 48
#define FAT16_DIR_CACHE_ENTITY_SIZE 16

class Fat16: public FileSystem {
    uint16_t bytesPerSector;
    uint16_t bytesPerCluster;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint32_t startOfFATs;
    uint8_t numberOfFATs;
    uint16_t sectorsPerFAT;
    uint16_t rootEntries;
    uint32_t rootDirStart;
    uint32_t dataSegStart;

    //Folder Cache System
    uint8_t *mDirCache;
    void initDirCache();
    int16_t getDirCache(uint16_t tParentSector, const char* tDirName);
    void updateDirCache(uint16_t tParentSector, const char* tDirName, uint16_t sector);
    void invalidateDirCache(uint16_t tParentSector, const char* tDirName, uint16_t sector);
    void freeDirCache();

    uint32_t sectorAddressOfDataCluster(uint16_t tFirstClusterId);
    uint32_t sectorAddressOfElement(fat16Element *tElement);

    uint16_t findFreeCluster();
    bool editCluster(uint16_t tClusterId, uint16_t tNewValue);
    uint16_t getClusterValue(uint16_t tClusterId);
    bool takeCluster(uint16_t tClusterId);
    bool freeNextCluster(uint16_t tClusterId);
    bool freeSequenceOfClusters(uint16_t tClusterId);
    bool makeClusterLast(uint16_t tClusterId);
    uint16_t extendCluster(uint16_t tClusterId);
    uint16_t allocateCluster();
    uint16_t getNextCluster(uint16_t tClusterId);

    // Tools to work with Fat16 Element
    uint8_t* encodeElement(fat16Element *tData);
    fat16Element decodeElement(uint8_t *tData);
    bool saveElement(uint16_t tSectorStart, uint8_t *tData);
    bool saveElement(uint8_t *tSegment, uint16_t tSectorStart, uint8_t *tData);
    bool saveElement(fat16Element *tHodler, uint8_t *tData);
    bool deleteElement(uint16_t tSectorStart, const char* tFilename, const char* tFilenameExtension);
    bool deleteElement(fat16Element *tHodler, const char* tFilename, const char* tFilenameExtension);
    int16_t getElementOffset(uint8_t *tData, const char* tFilename, const char* tFilenameExtension);
    int16_t getElementOffset(uint16_t tSectorStart, const char* tFilename, const char* tFilenameExtension);
    int16_t getElementOffset(fat16Element *tHodler, const char* tFilename, const char* tFilenameExtension);
    fat16Element getElement(uint8_t *tData, const char* tFilename, const char* tFilenameExtension);
    fat16Element getElement(uint16_t tSectorStart, const char* tFilename, const char* tFilenameExtension);
    fat16Element getElement(fat16Element *tHodler, const char* tFilename, const char* tFilenameExtension);
    fat16Element getElement(uint8_t *tData, int16_t tElementOffset);
    fat16Element getElement(uint16_t tSectorStart, int16_t tElementOffset);
    fat16Element getElement(fat16Element *tHodler, int16_t tElementOffset);

    void setFilename(fat16Element *tElement, const char *tFilename);
    void setFileExtension(fat16Element *tElement, const char *tFileExtension);
    void setAttribute(fat16Element *tElement, uint8_t tAttr);
    void setFirstCluster(fat16Element *tElement, uint16_t tCluster);
    void setDataSize(fat16Element *tElement, uint16_t tDataSize);

    fat16Element cd(const char *tPath);
    fat16Element* getFilesInDir(const char *tPath);

    // Tools
    void convertToVfs(fat16Element *tFat16Element, vfsElement *tVfsElement);
    
    void readParams();
    
    void initDir(uint16_t firstClusterId, uint16_t rootDirClusterId, uint8_t rootDirAttributes);
    
public:
    Fat16(DiskDriver *disk);
    static bool testDisk(DiskDriver *disk);
    void writeFile(const char *tPath, const char *tFilename, const char *tFilenameExtension, const char *tData, uint16_t tDataSize);
    uint8_t* readFile(const char *tPath, const char *tFilename, const char *tFilenameExtension);
    bool deleteFile(const char *tPath, const char *tFilename, const char *tFilenameExtension);

    bool createDir(const char *tPath, const char *tDirName);
    vfsDir getDir(const char *tPath);
    bool existPath(const char *tPath);
    bool isAttached();
};

#endif //FAT16_H