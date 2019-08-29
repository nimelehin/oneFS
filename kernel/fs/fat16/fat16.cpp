#include <fat16.h>
#include <iostream>

Fat16::Fat16(DiskDriver *hd): FileSystem(hd) {

}

bool Fat16::testDisk(DiskDriver *disk) {
    unsigned char* res = disk->readSector();
    for (uint16_t i = 0x36; i < 0x36 + 8; i++) {
        std::cout << res[i];
    }
    return true;
}

void Fat16::readFile(char *path, char *filename) {}
void Fat16::writeFile(char *path, char *filename, char *data) {}
void Fat16::mkdir(char *path) {}