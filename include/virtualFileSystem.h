#include <HDDriver.h>

#define MAX_DRIVES_IN_SYSTEM 8

class VirtualFileSystem {
    HDDriver* drivers[MAX_DRIVES_IN_SYSTEM];
    uint8_t nextDriveNum = 0;
public:
    char attach(HDDriver *driver);
    void recognize(HDDriver *driver);
};