#include <DiskDriver.h>
#include <FileSystem.h>

class DiskDescriptor {
    char diskName;
    FileSystem *fsObj;
public:
    DiskDescriptor(char diskName, FileSystem *fsObj);
};