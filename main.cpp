#include <iostream>
#include <fstream>
#include <HDDescriptor.h>

using namespace std;

int main() {
    HDDescriptor HD("hd.img");
    if (!HD.open()) {
        cout << "Error openning file\n";
    }
    char data[512];
    data[0] = 'M';
    data[1] = 'y';
    data[2] = 'O';
    data[3] = 'S';
    HD.writeSector(data);
    char* res = HD.readSector();
    for (int i = 0; i < 8; i++){
        cout << res[i];
    }
}