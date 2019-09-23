#ifndef Virtual_File_System_H
#define Virtual_File_System_H

#include <disk_driver.h>
#include <disk_descriptor.h>
#include <vfs_elements.h>
#include <fat16.h>

#define MAX_DISKS_IN_SYSTEM 8

class VirtualFileSystem {
    DiskDescriptor* mDisks[MAX_DISKS_IN_SYSTEM];
    uint8_t mNextDiskNum;
public:
    VirtualFileSystem();
    ~VirtualFileSystem();

    char attach(DiskDriver *driver);
    bool isAttached(char name);

    DiskDescriptor* recognize(DiskDriver *driver);
    
    bool createDir(const char *tPath, const char *tFolderName);
    vfsDir ls(char *tPath);
};

#endif //Virtual_File_System_H