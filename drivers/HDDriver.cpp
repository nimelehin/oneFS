#include <HDDriver.h>

HDDriver::HDDriver(char *fileName) {
    this->fileName = fileName;
}
HDDriver::~HDDriver() {
    this->close();
}

bool HDDriver::open() {
    this->hd = fopen(this->fileName, "r+");
    return hd != NULL;
}

void HDDriver::close() {
    fclose(this->hd);
}

bool HDDriver::writeSector(char *data) {
    return fwrite(data, 1, 512, this->hd) == 512;
}

unsigned char* HDDriver::readSector() {
    this->seek(0);
    unsigned char* data = (unsigned char*)malloc(512);
    fread(data, 1, 512, this->hd);
    return data;
}

void HDDriver::seek(unsigned long offset) {
    fseek(this->hd, offset, SEEK_SET);
}

