#ifndef FS_FILE
#define FS_FILE

#include <DiskDriver.h>

class FileSystem {
protected:
    DiskDriver *disk;
    char *fsName;
public:
    FileSystem(DiskDriver *hd);
    virtual void readFile(char *path, char *filename) = 0;
    virtual void writeFile(char *path, char *filename, char *data) = 0;
    virtual void mkdir(char *path) = 0;
    virtual bool isAttached() = 0;
};

#endif //FS_FILE