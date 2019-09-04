#include <DiskDescriptor.h>

DiskDescriptor::DiskDescriptor() {
    this->diskName = 0;
    this->fsObj = nullptr;
}

DiskDescriptor::DiskDescriptor(char diskName, uint64_t *fsObj) {
    this->diskName = diskName;
    this->fsObj = fsObj;
}