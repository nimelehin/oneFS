#ifndef FAT16_FILE
#define FAT16_FILE

#include <file_system.h>

class Fat16: public FileSystem {
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t numberOfFATs;
    uint16_t sectorsPerFAT;
    uint16_t rootEntries;
    uint32_t rootDirStart;
    uint32_t dataSegStart;
public:
    Fat16(DiskDriver *disk);
    static bool testDisk(DiskDriver *disk);
    void readFile(char *path, char *filename);
    void writeFile(char *path, char *filename, char *data);
    void mkdir(char *path); 
    void readParams();
    bool isAttached();
};

#endif // FAT16_FILE