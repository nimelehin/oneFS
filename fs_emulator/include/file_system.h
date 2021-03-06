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
    virtual uint8_t* readFile(const char *tPath, const char *tFilename, const char *tFilenameExtension, uint16_t tReadOffset, int16_t tLen) = 0;
    virtual void writeFile(const char *tPath, const char *tFilename, const char *tFilenameExtension, const char *tData, uint16_t tDataSize) = 0;
    virtual bool createDir(const char *tPath, const char *tFolderName) = 0;
    virtual vfsDir getVfsDir(const char *tPath) = 0;
    virtual bool hasDir(const char *tPath) = 0;
    virtual bool isAttached() = 0;
};

#endif //File_System_H