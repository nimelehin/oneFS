#include <iostream>
#include <fstream>
#include <DiskDriver.h>

using namespace std;

char toLet(unsigned int kek){
    //cout << kek << "\n";
    if (kek >= 10) {
        return ('A' + kek - 10);
    }
    return ('0' + kek);
}

int main() {
    DiskDriver HD("hd.img");
    if (!HD.open()) {
        cout << "Error openning file\n";
        return 0;
    }
    unsigned char* res = HD.readSector();
    for (int i = 0; i < 512; i++){
        if (i % 16 == 0){
            cout << "\n";
        }
        cout << toLet(res[i] / 16) << toLet(res[i] % 16) << " ";
        //cout << res[i];
    }
    return 0;
}