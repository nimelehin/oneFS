#ifndef FAT16_FILE
#define FAT16_FILE

#include <FileSystem.h>

class Fat16: public FileSystem {

public:
    Fat16(DiskDriver *hd);
    static bool testDisk(DiskDriver *disk);
    void readFile(char *path, char *filename);
    void writeFile(char *path, char *filename, char *data);
    void mkdir(char *path); 
    bool isAttached();
};

#endif // FAT16_FILE