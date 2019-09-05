#include <virtual_file_system.h>
#include <iostream>

using namespace std;

VirtualFileSystem::VirtualFileSystem(): m_nextDiskNum(0) {}

DiskDescriptor* VirtualFileSystem::recognize(DiskDriver *t_driver) {
    if (Fat16::testDisk(t_driver)) {
        Fat16 *fs = new Fat16(t_driver);
        DiskDescriptor *desc = new DiskDescriptor(m_nextDiskNum+'A', (uint64_t*)fs);
        return desc;
    }
    return new DiskDescriptor();
}

char VirtualFileSystem::attach(DiskDriver *t_driver) {
    m_disks[m_nextDiskNum++] = recognize(t_driver);
    return m_disks[m_nextDiskNum-1]->diskName;
}

bool VirtualFileSystem::isAttached(char t_name) {
    uint8_t diskId = t_name - 'A';
    DiskDescriptor* diskDesc = m_disks[diskId];
    Fat16 *ff = (Fat16*)diskDesc->fsObj;
    return ff->isAttached();
}

