#include <VirtualFileSystem.h>

char VirtualFileSystem::attach(DiskDriver *driver) {
    this->recognize(driver);

    return 1;
}
void VirtualFileSystem::recognize(DiskDriver *driver) {
    if (Fat16::testDisk(driver)) {
        
    }
}