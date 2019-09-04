#include <DiskDriver.h>
#include <DiskDescriptor.h>
#include <fat16.h>

#define MAX_DISKS_IN_SYSTEM 8

class VirtualFileSystem {
    DiskDescriptor* disks[MAX_DISKS_IN_SYSTEM];
    uint8_t nextDiskNum;
public:
    VirtualFileSystem();
    char attach(DiskDriver *driver);
    DiskDescriptor* recognize(DiskDriver *driver);
    bool isAttached(char name);
};