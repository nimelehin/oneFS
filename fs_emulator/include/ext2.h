#ifndef EXT2_H
#define EXT2_H

#include <file_system.h>
#include <vfs_elements.h>

class Ext2: public FileSystem {
public:
    Ext2(DiskDriver *disk);
    ~Ext2();
    void stop();
    static bool testDisk(DiskDriver *disk);
    bool isAttached();
};

#endif // EXT2_H