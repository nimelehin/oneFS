#include <HDDescriptor.h>

HDDescriptor::HDDescriptor(char *fileName) {
    this->fileName = fileName;
}
HDDescriptor::~HDDescriptor() {
    this->close();
}

bool HDDescriptor::open() {
    this->hd = fopen(this->fileName, "r+");
    return hd != NULL;
}

void HDDescriptor::close() {
    fclose(this->hd);
}

bool HDDescriptor::writeSector(char *data) {
    return fwrite(data, 1, 512, this->hd) == 512;
}

unsigned char* HDDescriptor::readSector() {
    this->seek(0);
    unsigned char* data = (unsigned char*)malloc(512);
    fread(data, 1, 512, this->hd);
    return data;
}

void HDDescriptor::seek(unsigned long offset) {
    fseek(this->hd, offset, SEEK_SET);
}

