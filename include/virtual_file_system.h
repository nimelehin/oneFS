#include <disk_driver.h>
#include <disk_descriptor.h>
#include <dir_descriptor.h>
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
    
    bool mkdir(char *t_path, char *dir_name);
    DirDescriptor* ls(char *t_path);
};