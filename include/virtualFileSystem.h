#include <DiskDriver.h>

#define MAX_DRIVES_IN_SYSTEM 8

class VirtualFileSystem {
    DiskDriver* drivers[MAX_DRIVES_IN_SYSTEM];
    uint8_t nextDriveNum = 0;
public:
    char attach(DiskDriver *driver);
    void recognize(DiskDriver *driver);
};