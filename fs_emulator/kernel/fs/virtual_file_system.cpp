#include <virtual_file_system.h>
#include <iostream>

using namespace std;

VirtualFileSystem::VirtualFileSystem(): mNextDiskNum(0) {}

VirtualFileSystem::~VirtualFileSystem() {
    for (uint16_t i = 0; i < mNextDiskNum; i++) {
        delete mDisks[i];
    }
}

void VirtualFileSystem::stopAll() {
    for (uint16_t diskId = 0; diskId < mNextDiskNum; diskId++) {
        DiskDescriptor* diskDesc = mDisks[diskId];
        Fat16 *fs = (Fat16*)diskDesc->fsObj;
        fs->stop();
    }
}


DiskDescriptor* VirtualFileSystem::recognize(DiskDriver *t_driver) {
    if (Fat16::testDisk(t_driver)) {
        Fat16 *fs = new Fat16(t_driver);
        DiskDescriptor *desc = new DiskDescriptor(mNextDiskNum+'A', (uint64_t*)fs);
        return desc;
    }
    // if (Ext2::testDisk(t_driver)) {
    //     Fat16 *fs = new Fat16(t_driver);
    //     DiskDescriptor *desc = new DiskDescriptor(mNextDiskNum+'A', (uint64_t*)fs);
    //     return desc;
    // }
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

Fat16* VirtualFileSystem::pathProcess(const char *tPath) {
    uint8_t pathSize = 0;
    while (tPath[pathSize] != 0) ++pathSize;
    assert(pathSize >= 3 && tPath[1] == ':' && tPath[2] == '/');
    
    uint8_t diskId = tPath[0] - 'A';
    assert(isAttached(tPath[0]));
    
    DiskDescriptor* diskDesc = mDisks[diskId];
    Fat16 *fs = (Fat16*)diskDesc->fsObj;
    
    return fs;
}

bool VirtualFileSystem::existPath(const char *tPath) {
    Fat16 *fs = pathProcess(tPath);
    return fs->hasDir(&tPath[2]);
}

bool VirtualFileSystem::createDir(const char *tPath, const char *tFolderName) {
    Fat16 *fs = pathProcess(tPath);
    return fs->createDir(&tPath[2], tFolderName);
}

bool VirtualFileSystem::writeFile(const char *tPath, const char *tFilename, 
            const char *tFilenameExtension, const char *tData, uint16_t tDataSize) {
    Fat16 *fs = pathProcess(tPath);
    fs->writeFile(&tPath[2], tFilename, tFilenameExtension, tData, tDataSize);
    return 1; // TODO redo writeFile result;
}

uint8_t* VirtualFileSystem::readFile(const char *tPath, const char *tFilename, const char *tFilenameExtension) {
    Fat16 *fs = pathProcess(tPath);
    return fs->readFile(&tPath[2], tFilename, tFilenameExtension, 0, -1);
}

bool VirtualFileSystem::deleteFile(const char *tPath, const char *tFilename, const char *tFilenameExtension) {
    Fat16 *fs = pathProcess(tPath);
    return fs->deleteFile(&tPath[2], tFilename, tFilenameExtension);
}

bool VirtualFileSystem::deleteDir(const char *tPath, const char *tFilename) {
    Fat16 *fs = pathProcess(tPath);
    return fs->deleteDir(&tPath[2], tFilename);
}

vfsDir VirtualFileSystem::ls(char *tPath) {
    Fat16 *fs = pathProcess(tPath);
    return fs->getVfsDir(&tPath[2]);
}
