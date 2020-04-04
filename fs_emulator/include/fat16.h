#ifndef FAT16_H
#define FAT16_H

#include <file_system.h>
#include <vfs_elements.h>
#include <fat16/fat16_element.hpp>
#include <fat16/dir_cache.h>

#include <iostream> //for test purposes

#define FAT16_MAX_FILENAME 8
#define FAT16_MAX_FILE_EXTENSION 3
#define FAT16_DELETED_SIGN (uint8_t)0xE5

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

    uint8_t *mFileAllocationTable;

    //Folder Cache System
    Fat16DirCache mDirCache;

    // Cluster Tools
    uint16_t findFreeCluster();
    bool editCluster(uint16_t tClusterId, uint16_t tNewValue);
    uint16_t getClusterValue(uint16_t tClusterId);
    bool takeCluster(uint16_t tClusterId);
    bool freeNextCluster(uint16_t tClusterId);
    bool freeSequenceOfClusters(uint16_t tClusterId);
    bool makeClusterLast(uint16_t tClusterId);
    uint16_t extendCluster(uint16_t tClusterId);
    uint16_t allocateCluster();
    uint16_t getNextOrExtendCluster(uint16_t tClusterId);
    uint16_t seekClusters(uint16_t tClusterId, uint8_t seekCount);

    // Tools to work with Fat16 Element
    uint8_t* encodeElement(fat16Element *tData);
    fat16Element decodeElement(uint8_t *tData);
    bool saveElement(uint32_t tSectorStart, uint8_t *tData);
    bool saveElement(uint8_t *tSegment, uint32_t tSectorStart, uint8_t *tData);
    bool saveElement(fat16Element *tHodler, uint8_t *tData);
    bool deleteElement(uint32_t tSectorStart, const char* tFilename, const char* tFilenameExtension);
    bool deleteElement(fat16Element *tHodler, const char* tFilename, const char* tFilenameExtension);
    int16_t getElementOffset(uint8_t *tData, const char* tFilename, const char* tFilenameExtension);
    int16_t getElementOffset(uint32_t tSectorStart, const char* tFilename, const char* tFilenameExtension);
    int16_t getElementOffset(fat16Element *tHodler, const char* tFilename, const char* tFilenameExtension);
    fat16Element getElement(uint8_t *tData, const char* tFilename, const char* tFilenameExtension);
    fat16Element getElement(uint32_t tSectorStart, const char* tFilename, const char* tFilenameExtension);
    fat16Element getElement(fat16Element *tHodler, const char* tFilename, const char* tFilenameExtension);
    fat16Element getElement(uint8_t *tData, int16_t tElementOffset);
    fat16Element getElement(uint32_t tSectorStart, int16_t tElementOffset);
    fat16Element getElement(fat16Element *tHodler, int16_t tElementOffset);
    void setFilename(fat16Element *tElement, const char *tFilename);
    void setFileExtension(fat16Element *tElement, const char *tFileExtension);
    void setAttribute(fat16Element *tElement, uint8_t tAttr);
    void setFirstCluster(fat16Element *tElement, uint16_t tCluster);
    void setDataSize(fat16Element *tElement, uint16_t tDataSize);

    // Dir tools
    fat16Element getDir(const char *tPath);
    fat16Element* getFilesInDir(const char *tPath);
    fat16Element* getFilesInDir(fat16Element tDir);
    void initDir(uint16_t firstClusterId, uint16_t rootDirClusterId, uint8_t rootDirAttributes);

    // Fat Init
    void readParams();
    void loadFAT();
    void saveFAT();

    // Tools
    void convertToVfs(fat16Element *tFat16Element, vfsElement *tVfsElement);
    uint32_t getSectorAddress(uint16_t tFirstClusterId);
    uint32_t getSectorAddress(fat16Element *tElement);

    // Debug Tools
    void printFAT(uint16_t tStart, uint16_t tEnd);

public:
    Fat16(DiskDriver *disk);
    ~Fat16();
    void stop();
    static bool testDisk(DiskDriver *disk);
    void writeFile(const char *tPath, const char *tFilename, const char *tFilenameExtension, const char *tData, uint16_t tDataSize);
    uint8_t* readFile(const char *tPath, const char *tFilename, const char *tFilenameExtension, uint16_t tReadOffset, int16_t tLen);
    bool deleteFile(const char *tPath, const char *tFilename, const char *tFilenameExtension);
    bool deleteFile(fat16Element *tHolderFolder, fat16Element *tFile);
    bool deleteDir(const char *tPath, const char *tDirName);

    bool createDir(const char *tPath, const char *tDirName);
    vfsDir getVfsDir(const char *tPath);
    bool hasDir(const char *tPath);
    bool isAttached();
};

#endif //FAT16_H
