#include <DiskDriver.h>
#include <fat16.h>

class DiskDescriptor {
    public:
    char diskName;

    Fat16 *fsObj;
    DiskDescriptor();
    DiskDescriptor(char diskName, Fat16 *fsObj);
};