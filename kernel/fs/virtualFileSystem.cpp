#include <VirtualFileSystem.h>
#include <iostream>

using namespace std;

VirtualFileSystem::VirtualFileSystem(): nextDiskNum(0) {}

char VirtualFileSystem::attach(DiskDriver *driver) {
    disks[nextDiskNum++] = recognize(driver);
    return disks[nextDiskNum-1]->diskName;
}

DiskDescriptor* VirtualFileSystem::recognize(DiskDriver *driver) {
    if (Fat16::testDisk(driver)) {
        Fat16 *fs = new Fat16(driver);
        DiskDescriptor *desc = new DiskDescriptor(nextDiskNum+'A', (uint64_t*)fs);
        return desc;
    }
    return new DiskDescriptor();
}

bool VirtualFileSystem::isAttached(char name) {
    uint8_t diskId = name - 'A';
    DiskDescriptor* diskDesc = disks[diskId];
    Fat16 *ff = (Fat16*)diskDesc->fsObj;
    return ff->isAttached();;
}