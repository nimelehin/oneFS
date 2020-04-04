#ifndef Disk_Descriptor_H
#define Disk_Descriptor_H


#include <disk_driver.h>
#include <fat16.h>

class DiskDescriptor {
    public:
    char diskName;
    uint64_t *fsObj;
    
    DiskDescriptor();
    DiskDescriptor(char diskName, uint64_t *fsObj);
    ~DiskDescriptor();
};

#endif //Disk_Descriptor_H