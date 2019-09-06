#include <disk_driver.h>

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

bool DiskDriver::writeSector(uint8_t *data) {
    return fwrite(data, 1, 512, this->hd) == 512;
}

uint8_t* DiskDriver::readSector() {
    uint8_t* data = (uint8_t*)malloc(512);
    fread(data, 1, 512, this->hd);
    return data;
}

void DiskDriver::seek(uint32_t offset) {
    fseek(this->hd, offset, SEEK_SET);
}

