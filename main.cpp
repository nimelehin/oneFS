#include <iostream>
#include <fstream>
#include <disk_driver.h>
#include <kernel.h>
#include <virtual_file_system.h>

using namespace std;

int main() {
    Kernel k = Kernel();
    k.startCmd();
    return 0;
}