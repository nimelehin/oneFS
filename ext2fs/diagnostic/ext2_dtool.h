#ifndef DIAGNOSTIC_EXT2_DTOOL_H
#define DIAGNOSTIC_EXT2_DTOOL_H

#include <stdint.h>
#include <fstream>
#include "../ext2/ext2.h"

class Ext2DTool : public Ext2{
private:

public:
    Ext2DTool(const char *filename) : Ext2(filename) {}
    void print_disk_info();
    void print_group_info(uint32_t group);
    void print_inode_bitmap(uint32_t group);
    void print_data_blocks_bitmap(uint32_t group);
    void print_inode(uint32_t inode_id);
    void print_dir(uint32_t inode_id, uint32_t file);
};

#endif // DIAGNOSTIC_EXT2_DTOOL_H