#include <iostream>
#include <fstream>
#include <DiskDriver.h>
#include <VirtualFileSystem.h>

using namespace std;

int main() {
    DiskDriver HD("hd.img");
    HD.open();
    VirtualFileSystem vfs = VirtualFileSystem();
    char hdName = vfs.attach(&HD);
    cout << hdName << "\n";
    cout << vfs.isAttached(hdName) << "\n";
    HD.close();
    return 0;
}