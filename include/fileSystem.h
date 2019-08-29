#ifndef FS_FILE
#define FS_FILE

#include <DiskDriver.h>

class FileSystem {
private:
    DiskDriver *hd;
    char *fsName;
public:
    FileSystem(DiskDriver *hd);
    virtual void readFile(char *path, char *filename) = 0;
    virtual void writeFile(char *path, char *filename, char *data) = 0;
    virtual void mkdir(char *path) = 0;
};

#endif //FS_FILE