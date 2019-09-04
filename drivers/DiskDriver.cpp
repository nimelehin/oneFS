#include <DiskDriver.h>

DiskDriver::DiskDriver(char *fileName) {
    this->fileName = fileName;
}
DiskDriver::~DiskDriver() {
    this->close();
}

bool DiskDriver::open() {
    this->hd = fopen(this->fileName, "r+");
    return hd != NULL;
}

void DiskDriver::close() {
    fclose(this->hd);
}

bool DiskDriver::writeSector(char *data) {
    return fwrite(data, 1, 512, this->hd) == 512;
}

unsigned char* DiskDriver::readSector() {
    unsigned char* data = (unsigned char*)malloc(512);
    fread(data, 1, 512, this->hd);
    return data;
}

void DiskDriver::seek(unsigned long offset) {
    fseek(this->hd, offset, SEEK_SET);
}

