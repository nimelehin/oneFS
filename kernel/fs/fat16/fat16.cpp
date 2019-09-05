#include <fat16.h>
#include <iostream>

Fat16::Fat16(DiskDriver *t_disk): FileSystem(t_disk) {
    readParams();
}

bool Fat16::testDisk(DiskDriver *t_disk) {
    t_disk->seek(0);
    unsigned char* res = t_disk->readSector();
    char fat16Signature[] = {'F', 'A', 'T', '1', '6', 0x20, 0x20, 0x20};
    bool sigCorrect = true;
    for (uint16_t i = 0x36; i < 0x36 + 8; i++) {
        sigCorrect &= (fat16Signature[i - 0x36] == res[i]);
    }
    return sigCorrect;
}

void Fat16::readFile(char *t_path, char *t_filename) {}
void Fat16::writeFile(char *t_path, char *t_filename, char *t_data) {}
void Fat16::mkdir(char *t_path) {}
bool Fat16::isAttached() {
    return true;
}

void Fat16::readParams() {
    disk->seek(0);
    unsigned char* data = disk->readSector();
    bytesPerSector = data[0x0c] * 0x100 + data[0x0b];
    sectorsPerCluster = data[0x0d];
    reservedSectors = data[0x0f] * 0x100 + data[0x0e];
    numberOfFATs = data[0x10];
    sectorsPerFAT = data[0x17] * 0x100 + data[0x16];
    rootEntries = data[0x12] * 0x100 + data[0x11];
    rootDirStart = bytesPerSector * reservedSectors + numberOfFATs * sectorsPerFAT * bytesPerSector; 
    dataSegStart = rootDirStart + rootEntries * 32;
    // std::cout << (int)bytesPerSector << "-bytesPerSector\n";
    // std::cout << (int)sectorsPerCluster << "-sectorsPerCluster\n";
    // std::cout << (int)numberOfFATs << "-numberOfFATs\n";
    // std::cout << (int)sectorsPerFAT << "-sectorsPerFAT\n";
    // std::cout << (int)rootEntries << "-rootEntries\n";
    // std::cout << (int)rootDirStart << "-rootDirStart\n";
    // std::cout << (int)dataSegStart << "-dataSegStart\n";
}