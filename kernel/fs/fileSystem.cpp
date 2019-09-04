#include <FileSystem.h>

FileSystem::FileSystem(DiskDriver *_disk) {
    this->disk = _disk;
}