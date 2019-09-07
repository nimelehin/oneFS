#ifndef FAT16_FILE
#define FAT16_FILE

#include <file_system.h>
#include <dir_descriptor.h>
#include <fat16/fat16_element.hpp>

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

    uint16_t findFreeBlock();

public:
    Fat16(DiskDriver *disk);
    static bool testDisk(DiskDriver *disk);
    void readFile(char *path, char *filename);
    void writeFile(char *path, char *filename, char *data);
    void mkdir(char *path); 
    void readParams();
    bool isAttached();
    DirDescriptor* ls(char *tPath, uint16_t tPathSize);
    uint8_t* encodeElement(fat16Element *tData);
    fat16Element* decodeElement(uint8_t *tData);
    void dummyFileCreation();
};

#endif // FAT16_FILE