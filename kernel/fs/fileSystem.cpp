#include <FileSystem.h>

FileSystem::FileSystem(DiskDriver *_hd) {
    this->hd = _hd;
}