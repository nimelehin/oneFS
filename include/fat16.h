#ifndef FAT16_H
#define FAT16_H

#include <file_system.h>
#include <dir_descriptor.h>
#include <fat16/fat16_element.hpp>

#include <iostream> //for test purposes

class Fat16: public FileSystem {
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint32_t startOfFATs;
    uint8_t numberOfFATs;
    uint16_t sectorsPerFAT;
    uint16_t rootEntries;
    uint32_t rootDirStart;
    uint32_t dataSegStart;

    uint32_t sectorAddressOfDataBlock(fat16Element *tElement);

    uint16_t findFreeCluster();
    bool editClusterWithId(uint16_t tBlockId, uint16_t tNewValue);
    bool takeClusterWithId(uint16_t tBlockId);
    bool freeClusterWithId(uint16_t tBlockId);
    uint16_t extendClusterWithId(uint16_t tBlockId); // returns allocated blockId

    uint8_t* encodeElement(fat16Element *tData);
    fat16Element decodeElement(uint8_t *tData);
    bool saveElement(uint16_t tBlockId, uint8_t *tData);
    fat16Element findElementWithName(uint8_t *tData, char* filename, char* filenameExtension=nullptr);

    fat16Element cd(char *tPath);


public:
    Fat16(DiskDriver *disk);
    static bool testDisk(DiskDriver *disk);
    void readFile(char *path, char *filename);
    void writeFile(char *path, char *filename, char *data);
    bool mkdir(char *tPath, char *tFolderName);
    void readParams();
    bool isAttached();
    DirDescriptor* ls(char *tPath, uint16_t tPathSize);
    
    void dummyFileCreation();
};

#endif //FAT16_H