#include <kernel.h>

bool Kernel::attach(char *hdName) {
    HD = HDDriver(hdName);
    return true;
}

void Kernel::writeFile() {

}

void Kernel::readFile() {

}