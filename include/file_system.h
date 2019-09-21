#ifndef File_System_H
#define File_System_H

#include <disk_driver.h>
#include <vfs_elements.h>

class FileSystem {
protected:
    DiskDriver *disk;
    char *fsName;
public:
    FileSystem(DiskDriver *hd);
    virtual void readFile(char *path, char *filename) = 0;
    virtual void writeFile(char *tPath, char *tFilename, char *tFilenameExtension, char *tData, uint16_t tDataSize) = 0;
    virtual bool createDir(char *tPath, char *tFolderName) = 0;
    virtual vfsDir getDir(char *tPath) = 0;
    virtual bool isAttached() = 0;
};

#endif //File_System_H