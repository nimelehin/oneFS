#include <virtualFileSystem.h>

char VirtualFileSystem::attach(DiskDriver *driver) {
    this->recognize(driver);

    return 1;
}
void VirtualFileSystem::recognize(DiskDriver *driver) {
    
}