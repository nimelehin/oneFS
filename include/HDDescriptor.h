/* 
    HD driver
    Technology: LBA28
    Sector size: 512 bytes
 */

#include <iostream>
#include <fstream>

class HDDescriptor {
private:
    char *fileName;
    FILE *hd;
public:
    HDDescriptor(char *fileName);
    ~HDDescriptor();
    bool open();
    void close();
    void seek();
    bool write(char *data); //write sector with sizw 512 bytes
    void read(); //read sector with size 512 bytes
};