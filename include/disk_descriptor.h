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