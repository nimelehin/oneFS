#include <virtual_file_system.h>
#include <iostream>

using namespace std;

VirtualFileSystem::VirtualFileSystem(): mNextDiskNum(0) {}

VirtualFileSystem::~VirtualFileSystem() {
    for (uint16_t i = 0; i < mNextDiskNum; i++) {
        delete mDisks[i];
    }
}

DiskDescriptor* VirtualFileSystem::recognize(DiskDriver *t_driver) {
    if (Fat16::testDisk(t_driver)) {
        Fat16 *fs = new Fat16(t_driver);
        fs->dummyFileCreation();
        DiskDescriptor *desc = new DiskDescriptor(mNextDiskNum+'A', (uint64_t*)fs);
        return desc;
    }
    return new DiskDescriptor();
}

char VirtualFileSystem::attach(DiskDriver *tDriver) {
    mDisks[mNextDiskNum++] = recognize(tDriver);
    return mDisks[mNextDiskNum-1]->diskName;
}

bool VirtualFileSystem::isAttached(char tName) {
    uint8_t diskId = tName - 'A';
    assert(0 <= diskId && diskId < mNextDiskNum);

    DiskDescriptor* diskDesc = mDisks[diskId];
    Fat16 *ff = (Fat16*)diskDesc->fsObj;
    return ff->isAttached();
}

fat16Element* VirtualFileSystem::ls(char *tPath) {
    uint8_t pathSize = 0;
    while (tPath[pathSize] != 0) ++pathSize;
    assert(pathSize >= 3 && tPath[1] == ':' && tPath[2] == '/');
    
    uint8_t diskId = tPath[0] - 'A';
    assert(isAttached(tPath[0]));
    
    DiskDescriptor* diskDesc = mDisks[diskId];
    Fat16 *ff = (Fat16*)diskDesc->fsObj;
    
    tPath = &tPath[2];

    return ff->ls(tPath);
}
