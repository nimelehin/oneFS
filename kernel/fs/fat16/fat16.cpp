#include <fat16.h>
#include <iostream>

Fat16::Fat16(DiskDriver *hd): FileSystem(hd) {

}

bool Fat16::testDisk(DiskDriver *disk) {
    disk->seek(0);
    unsigned char* res = disk->readSector();
    char fat16Signatire[] = {'F', 'A', 'T', '1', '6', 0x20, 0x20, 0x20};
    bool sigCorrect = true;
    for (uint16_t i = 0x36; i < 0x36 + 8; i++) {
        sigCorrect &= (fat16Signatire[i - 0x36] == res[i]);
    }
    return sigCorrect;
}

void Fat16::readFile(char *path, char *filename) {}
void Fat16::writeFile(char *path, char *filename, char *data) {}
void Fat16::mkdir(char *path) {}
bool Fat16::isAttached() {
    return true;
}