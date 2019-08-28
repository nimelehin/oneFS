#ifndef HDDriver_FILE
#define HDDriver_FILE

/* 
    HD driver
    Technology: LBA28
    Sector size: 512 bytes
 */

#include <iostream>
#include <fstream>

class HDDriver {
private:
    char *fileName;
    FILE *hd;
public:
    HDDriver(char *fileName);
    ~HDDriver();
    bool open();
    void close();
    void seek(unsigned long offset);
    bool writeSector(char *data); //write sector with sizw 512 bytes
    unsigned char* readSector(); //read sector with size 512 bytes
};

#endif // HDDriver