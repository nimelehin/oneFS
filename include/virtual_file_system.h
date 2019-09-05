#include <disk_driver.h>
#include <disk_descriptor.h>
#include <fat16.h>

#define MAX_DISKS_IN_SYSTEM 8

class VirtualFileSystem {
    DiskDescriptor* m_disks[MAX_DISKS_IN_SYSTEM];
    uint8_t m_nextDiskNum;
public:
    VirtualFileSystem();
    char attach(DiskDriver *driver);
    DiskDescriptor* recognize(DiskDriver *driver);
    bool isAttached(char name);
};