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
    void seek(unsigned long offset);
    bool writeSector(char *data); //write sector with sizw 512 bytes
    char* readSector(); //read sector with size 512 bytes
};