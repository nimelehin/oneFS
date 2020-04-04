#include "file_system.h"

FileSystem::FileSystem(DiskDriver *t_disk) {
    this->disk = t_disk;
}