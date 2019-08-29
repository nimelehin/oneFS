#include <VirtualFileSystem.h>
#include <iostream>

using namespace std;

VirtualFileSystem::VirtualFileSystem(): nextDiskNum(0) {}

char VirtualFileSystem::attach(DiskDriver *driver) {
    disks[nextDiskNum++] = recognize(driver);
    return 'A';
}

DiskDescriptor VirtualFileSystem::recognize(DiskDriver *driver) {
    if (Fat16::testDisk(driver)) {
        Fat16 fs = Fat16(driver);
        DiskDescriptor desc = DiskDescriptor('A', &fs);
        cout << &fs << "\n";
        (&fs)->isAttached();
        return desc;
    }
    return DiskDescriptor();
}

bool VirtualFileSystem::isAttached(char name) {
    uint8_t diskId = name - 'A';
    DiskDescriptor* diskDesc = &disks[diskId];
    Fat16 *ff = (Fat16*)diskDesc->fsObj;
    cout << ff << "\n";
    // ff.isAttached();
    //return diskDesc->fsObj->isAttached();
    return 1;
}