#include <fat16.h>

Fat16::Fat16(HDDescriptor *hd): FileSystem(hd) {

}

void Fat16::readFile(char *path, char *filename) {}
void Fat16::writeFile(char *path, char *filename, char *data) {}
void Fat16::mkdir(char *path) {}