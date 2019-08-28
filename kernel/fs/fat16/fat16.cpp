#include <fat16.h>
#include <iostream>

Fat16::Fat16(HDDriver *hd): FileSystem(hd) {

}

bool Fat16::testDrive() {
    char *firstSector;
    std::cout << "Testing Fat16\n";
}

void Fat16::readFile(char *path, char *filename) {}
void Fat16::writeFile(char *path, char *filename, char *data) {}
void Fat16::mkdir(char *path) {}