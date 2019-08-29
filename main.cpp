#include <iostream>
#include <fstream>
#include <DiskDriver.h>
#include <VirtualFileSystem.h>

using namespace std;

int main() {
    DiskDriver HD("hd.img");
    HD.open();
    VirtualFileSystem vfs = VirtualFileSystem();
    vfs.attach(&HD);
    HD.close();
    return 0;
}