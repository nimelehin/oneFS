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

bool HDDescriptor::write(char *data) {
    return fwrite(data, 1, 512, this->hd) == 512;
}


void HDDescriptor::read() {
    fseek(this->hd, 0, SEEK_SET);
    char data[10];
    fread(data, 1, 10, this->hd);
    for (int i = 0; i < 10; i++){
        std::cout << data[i];
    }
}

