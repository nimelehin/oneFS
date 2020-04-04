
#ifndef EXT2_H
#define EXT2_H

#include <stdint.h>
#include <fstream>
#include <sys/stat.h>
#include <cassert>
#include <iostream>
#include "ext2_components.h"

class Ext2 {
protected:

    SuperBlock *superblock;
    GroupDescriptor *group_table;

    FILE *disk_desc;
    uint32_t disk_size;
    void setup_disk(const char *filename);
    void write(void *ptr, uint32_t start, uint32_t size);
    void read(void *ptr, uint32_t start, uint32_t size);

    // Tools
    bool bitmap_change_value(uint8_t* bitmap, uint32_t offset, bool value);
    bool bitmap_get_value(uint8_t* bitmap, uint32_t offset);

    // fill @superblock & @group_table vars
    int load_superblock();
    int load_group_table();

    // APIS' Helpers
    DirectoryEntry* scan_linked_list_dir(uint32_t block_id, uint32_t &scanned, uint32_t need);

    int norm_filename_len(int len);
    int find_free_block(uint8_t* bitmap);
    int try_to_add_to_dir_block(uint32_t block_index, uint32_t added_inode_index, char *filename);

public:
    Ext2(const char *filename);
    SuperBlock* get_superblock();
    Inode* get_inode(uint32_t inode_id); 
    int update_inode(uint32_t inode_id, Inode *inode); 
    DirectoryEntry* get_dir(Inode* inode, uint32_t file_ret);
    

    // Tools
    uint32_t get_block_len();
    uint32_t get_group_len();
    uint32_t get_groups_cnt();
    uint32_t get_group_table_start();
    uint32_t get_group_table_len();
  
    uint32_t get_itable_blocks_in_group(uint32_t id);
    uint32_t get_group_table_blocks_in_group(uint32_t id);
    uint32_t get_offset_of_block(uint32_t id);
    uint32_t get_offset_of_inode(uint32_t id);
    uint32_t get_offset_of_group_in_blocks(uint32_t id);
    uint32_t get_offset_of_group(uint32_t id);
    uint32_t get_offset_of_group_table(uint32_t id);
    uint32_t get_data_blocks_in_group(uint32_t id);

    bool inode_mode_has_flag(uint16_t mode, uint16_t flag);
    int allocate_block(uint32_t inode_index);
    int add_to_dir(uint32_t dir_inode_index, uint32_t added_inode_index, char *filename);
};


#endif // EXT2_H