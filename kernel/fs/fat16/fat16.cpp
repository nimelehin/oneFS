#include <fat16.h>
#include <iostream>

Fat16::Fat16(DiskDriver *t_disk): FileSystem(t_disk), mDirCache(){
    readParams();
}

// REQUIRED BY VFS
// testDisk is supposed to test a disk and recognise if the disk 
// has the file system which can be operated by this driver
bool Fat16::testDisk(DiskDriver *t_disk) {
    t_disk->seek(0);
    uint8_t* res = t_disk->readSector();
    char fat16Signature[] = {'F', 'A', 'T', '1', '6', 0x20, 0x20, 0x20};
    bool isSignatureCorrect = true;
    for (uint16_t i = 0x36; i < 0x36 + 8; i++) {
        isSignatureCorrect &= (fat16Signature[i - 0x36] == res[i]);
    }
    free(res);
    return isSignatureCorrect;
}

// readParams is supposed to read all system data which need for FS
void Fat16::readParams() {
    disk->seek(0);
    uint8_t* data = disk->readSector();
    bytesPerSector = data[0x0c] * 0x100 + data[0x0b];
    sectorsPerCluster = 1; // data[0x0d]
    bytesPerCluster = bytesPerSector * sectorsPerCluster;
    reservedSectors = data[0x0f] * 0x100 + data[0x0e];
    startOfFATs = reservedSectors * bytesPerSector;
    numberOfFATs = data[0x10];
    sectorsPerFAT = data[0x17] * 0x100 + data[0x16];
    rootEntries = data[0x12] * 0x100 + data[0x11];
    rootDirStart = bytesPerSector * reservedSectors + numberOfFATs * sectorsPerFAT * bytesPerSector; 
    dataSegStart = rootDirStart + rootEntries * 32;
    free(data);
}