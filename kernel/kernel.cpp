#include <kernel.h>

bool Kernel::attach(char *hdName) {
    HD = DiskDriver(hdName);
    return true;
}

void Kernel::writeFile() {

}

void Kernel::readFile() {

}