#ifndef Disk_Driver_H
#define Disk_Driver_H

/* 
    HD driver
    Technology: LBA28
    Sector size: 512 bytes
 */

#include <iostream>
#include <fstream>

class DiskDriver {
private:
    FILE *hd;
public:
char *fileName;
    
    DiskDriver(char *fileName);
    ~DiskDriver();
    bool open();
    void close();
    void seek(uint32_t offset);
    bool writeSector(uint8_t *data); //write sector with size 512 bytes
    uint8_t* readSector(); //read sector with size 512 bytes
};

#endif //Disk_Driver_H