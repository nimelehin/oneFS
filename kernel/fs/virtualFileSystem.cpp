#include <virtualFileSystem.h>

char VirtualFileSystem::attach(HDDriver *driver) {
    this->recognize(driver);

    return 1;
}
void VirtualFileSystem::recognize(HDDriver *driver) {
    
}