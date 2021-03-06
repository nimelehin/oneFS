#ifndef Virtual_File_System_H
#define Virtual_File_System_H

#include <disk_driver.h>
#include <disk_descriptor.h>
#include <vfs_elements.h>
#include <fat16.h>
#include <ext2.h>

#define MAX_DISKS_IN_SYSTEM 8

class VirtualFileSystem {
    DiskDescriptor* mDisks[MAX_DISKS_IN_SYSTEM];
    uint8_t mNextDiskNum;
    Fat16* pathProcess(const char *tPath); // TODO implement not only Fat16 support
public:
    VirtualFileSystem();
    ~VirtualFileSystem();

    void stopAll();

    char attach(DiskDriver *driver);
    bool isAttached(char name);

    DiskDescriptor* recognize(DiskDriver *driver);
    
    bool existPath(const char *tPath);
    bool createDir(const char *tPath, const char *tFolderName);
    bool writeFile(const char *tPath, const char *tFilename, const char *tFilenameExtension, const char *tData, uint16_t tDataSize);
    uint8_t* readFile(const char *tPath, const char *tFilename, const char *tFilenameExtension);
    bool deleteFile(const char *tPath, const char *tFilename, const char *tFilenameExtension);
    bool deleteDir(const char *tPath, const char *tFilename);
    vfsDir ls(char *tPath);
};

#endif //Virtual_File_System_H