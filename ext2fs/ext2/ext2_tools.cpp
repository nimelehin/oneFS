#include "ext2.h"
#include <cstdlib> 

uint32_t Ext2::get_block_len() {
    return (1 << (superblock->log_block_size + 10));
}

uint32_t Ext2::get_group_len() {
    return get_block_len() * get_block_len() * 8;
}

uint32_t Ext2::get_groups_cnt() {
    uint32_t sz = disk_size - SUPERBLOCK_START;
    uint32_t ans = sz / get_group_len();
    // if (sz % get_group_len()) {
    //     ans++;
    // }
    return ans;
}

uint32_t Ext2::get_group_table_start() {
    return SUPERBLOCK_START + get_block_len();
}

uint32_t Ext2::get_group_table_len() {
    return sizeof(GroupDescriptor) * get_groups_cnt();
}

uint32_t Ext2::get_offset_of_group_in_blocks(uint32_t id) {
    return 1 + id * superblock->blocks_per_group;
}

uint32_t Ext2::get_offset_of_group(uint32_t id) {
    return SUPERBLOCK_START + id * get_group_len();
}

uint32_t Ext2::get_itable_blocks_in_group(uint32_t id) {
    uint32_t inodes_per_block = get_block_len() / sizeof(Inode);
    uint32_t result = superblock->inodes_per_group / inodes_per_block;
    if (superblock->inodes_per_group % inodes_per_block) {
        result++;
    }
    return result;
}

uint32_t Ext2::get_group_table_blocks_in_group(uint32_t id) {
    uint32_t gdesc_blocks_count_per_block = get_block_len() / sizeof(GroupDescriptor);
    uint32_t groups_cnt = get_groups_cnt();
    uint32_t res = groups_cnt / gdesc_blocks_count_per_block;
    if (groups_cnt % gdesc_blocks_count_per_block) {
        res++;
    }
    return res;
}

uint32_t Ext2::get_data_blocks_in_group(uint32_t id) {
    return superblock->blocks_per_group - get_itable_blocks_in_group(id) - get_group_table_blocks_in_group(id) - 3; 
}

uint32_t Ext2::get_offset_of_group_table(uint32_t id) {
    uint32_t group_start = get_offset_of_group(id);
    return group_start + get_block_len();
}

uint32_t Ext2::get_offset_of_block(uint32_t id) {
    return SUPERBLOCK_START + (id - 1) * get_block_len();
}

uint32_t Ext2::get_offset_of_inode(uint32_t index) {
    uint32_t group_index = (index - 1) / superblock->inodes_per_group;
    uint32_t local_inode_index = (index - 1) % superblock->inodes_per_group;
    return get_offset_of_block(group_table[group_index].inode_table) + (INODE_LEN * local_inode_index);
}

bool Ext2::bitmap_change_value(uint8_t* bitmap, uint32_t offset, bool value) {
    offset--;
    uint32_t index = offset / 8; 
    uint32_t inner_offset = offset % 8;
    if (value) {
        bitmap[index] |= (1 << inner_offset);
    } else {
        bitmap[index] &= ~(1 << inner_offset);
    }
    return true;
}

bool Ext2::bitmap_get_value(uint8_t* bitmap, uint32_t offset) {
    return (bitmap[offset / 8] >> (offset % 8)) & 1;
}

bool Ext2::inode_mode_has_flag(uint16_t mode, uint16_t flag) {
    return (mode & flag) != 0;
}

int Ext2::norm_filename_len(int len) {
    return len + ((4 - (len & 0b11)) & 0b11);
}