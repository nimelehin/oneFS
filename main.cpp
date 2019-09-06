#include <iostream>
#include <fstream>
#include <disk_driver.h>
#include <virtual_file_system.h>

using namespace std;

int main() {
    DiskDriver HD("hd.img");
    HD.open();
    VirtualFileSystem vfs = VirtualFileSystem();
    char hdName = vfs.attach(&HD);
    cout << hdName << "\n";
    cout << vfs.isAttached(hdName) << "\n";
    vfs.ls("A:/");
    HD.close();
    return 0;
}