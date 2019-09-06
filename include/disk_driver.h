#ifndef DiskDriver_FILE
#define DiskDriver_FILE

/* 
    HD driver
    Technology: LBA28
    Sector size: 512 bytes
 */

#include <iostream>
#include <fstream>

class DiskDriver {
private:
    char *fileName;
    FILE *hd;
public:
    DiskDriver(char *fileName);
    ~DiskDriver();
    bool open();
    void close();
    void seek(u_int32_t offset);
    bool writeSector(uint8_t *data); //write sector with sizw 512 bytes
    uint8_t* readSector(); //read sector with size 512 bytes
};

#endif // DiskDriver_FILE