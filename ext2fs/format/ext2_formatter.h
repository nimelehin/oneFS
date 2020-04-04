#ifndef FORMAT_EXT2_FORMATTER_H
#define FORMAT_EXT2_FORMATTER_H

#include "../ext2/ext2.h"

#define BACKUPS_CNT 2

struct Ext2FormatSettings {
    uint32_t block_size;
    uint32_t inodes_per_group;
};

class Ext2Formatter : public Ext2 {
private:
    void setup_superblock(Ext2FormatSettings settings);
    void setup_group_tables(Ext2FormatSettings settings);
    void setup_inode_bitmaps();
    void setup_block_bitmaps();
    void setup_root_dir();
public:
    Ext2Formatter(const char *filename) : Ext2(filename) {}
    bool format(Ext2FormatSettings settings);
};

#endif // FORMAT_EXT2_FORMATTER_H