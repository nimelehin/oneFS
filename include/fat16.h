#ifndef FAT16_H
#define FAT16_H

#include <file_system.h>
#include <vfs_elements.h>
#include <fat16/fat16_element.hpp>

#include <iostream> //for test purposes

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

    uint32_t sectorAddressOfDataCluster(uint16_t tFirstClusterId);
    uint32_t sectorAddressOfElement(fat16Element *tElement);

    uint16_t findFreeCluster();
    bool editClusterWithId(uint16_t tBlockId, uint16_t tNewValue);
    bool takeClusterWithId(uint16_t tBlockId);
    bool freeClusterWithId(uint16_t tBlockId);
    uint16_t extendClusterWithId(uint16_t tBlockId); // returns allocated blockId

    uint8_t* encodeElement(fat16Element *tData);
    fat16Element decodeElement(uint8_t *tData);
    bool saveElement(uint16_t tBlockId, uint8_t *tData);
    fat16Element findElementWithName(uint8_t *tData, const char* filename, const char* filenameExtension=nullptr);

    fat16Element cd(const char *tPath);
    fat16Element* getFilesInDir(const char *tPath);

    void convertToVfs(fat16Element *tFat16Element, vfsElement *tVfsElement);

    void readParams();
    
public:
    Fat16(DiskDriver *disk);
    static bool testDisk(DiskDriver *disk);
    void readFile(const char *tPath, const char *tFilename, const char *tFilenameExtension);
    void writeFile(const char *tPath, const char *tFilename, const char *tFilenameExtension, const char *tData, uint16_t tDataSize);
    bool createDir(const char *tPath, const char *tFolderName);
    vfsDir getDir(const char *tPath);
    bool isAttached();
    
    void dummyFileCreation();
};

#endif //FAT16_H