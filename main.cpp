#include <iostream>
#include <fstream>
#include <HDDescriptor.h>

using namespace std;

int main() {
    HDDescriptor HD("hd.img");
    cout << HD.open();
    char data[512];
    data[0] = 'M';
    data[1] = 'y';
    data[2] = 'O';
    data[3] = 'S';
    cout << HD.write(data);
    HD.read();
}