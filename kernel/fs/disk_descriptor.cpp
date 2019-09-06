#include <disk_descriptor.h>

DiskDescriptor::DiskDescriptor() {
    this->diskName = 0;
    this->fsObj = nullptr;
}

DiskDescriptor::~DiskDescriptor() {
    delete fsObj;
}

DiskDescriptor::DiskDescriptor(char t_diskName, uint64_t *t_fsObj) {
    diskName = t_diskName;
    fsObj = t_fsObj;
}