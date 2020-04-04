#include <iostream>
#include <fstream>
#include <disk_driver.h>
#include <kernel.h>
#include <virtual_file_system.h>

using namespace std;

int main(int argc, char **argv) {
    Kernel k = Kernel();
    if (argc == 1) {
        k.startCmd();
    } else {
        k.runCmd(argc, argv);
    }
    return 0;
}
