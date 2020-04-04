#include "ext2.h"
#include <cstdlib> 

Inode* Ext2::get_inode(uint32_t inode_id) {
    uint32_t inodes_per_group = superblock->inodes_per_group;
    uint32_t holder_group = (inode_id - 1) / inodes_per_group;
    uint32_t pos_inside_group = (inode_id - 1) % inodes_per_group;
    uint32_t inode_start = SUPERBLOCK_START + ((group_table[holder_group].inode_table - 1) * get_block_len() + (pos_inside_group * INODE_LEN));
    Inode* inode = (Inode*)malloc(INODE_LEN);
    read(inode, inode_start, INODE_LEN);
    return inode;
}

int Ext2::update_inode(uint32_t inode_id, Inode *inode) {
    uint32_t inodes_per_group = superblock->inodes_per_group;
    uint32_t holder_group = (inode_id - 1) / inodes_per_group;
    uint32_t pos_inside_group = (inode_id - 1) % inodes_per_group;
    uint32_t inode_start = SUPERBLOCK_START + ((group_table[holder_group].inode_table - 1) * get_block_len() + (pos_inside_group * INODE_LEN));
    write(inode, inode_start, INODE_LEN);
    return 0;
}

DirectoryEntry* Ext2::scan_linked_list_dir(uint32_t block_id, uint32_t &scanned, uint32_t need) {
    DirectoryEntry* block_data = (DirectoryEntry*)malloc(get_block_len());
    // uint8_t* block_data_8 = (uint8_t*)block_data;
    // for (int i = 0; i < get_block_len(); i++) {
    //     std::cout << (uint32_t)block_data_8[i];
    //     if (i % 16 == 15) {
    //         std::cout << std::endl;
    //     }
    // }
    read(block_data, get_offset_of_block(block_id), get_block_len());
    DirectoryEntry* start_of_entry = block_data;
    // TODO mightn't stop at right time
    for (;;) {
        scanned++;
        if (scanned == need) {
            DirectoryEntry* ret_data = (DirectoryEntry*)malloc(start_of_entry->rec_len);
            memcpy(ret_data, start_of_entry, start_of_entry->rec_len);
            free(block_data);
            return ret_data;
        }
        if (start_of_entry->rec_len != 0) {
            start_of_entry = (DirectoryEntry*)((uint64_t)start_of_entry + start_of_entry->rec_len);
            // TODO might not work
            if (start_of_entry > block_data + get_block_len()) {
                return 0;
            }
        }
    }
}

// Add support of hash-dir
DirectoryEntry* Ext2::get_dir(Inode* inode, uint32_t file_ret) {
    assert((inode != NULL));
    assert(inode_mode_has_flag(inode->mode, EXT2_S_IFDIR));
    // TODO add support of second blocks?
    uint32_t passed = 0;
    DirectoryEntry* res; 
    for (int i = 0; i < 15; i++) {
        if (inode->block[i] != 0) {
            res = scan_linked_list_dir(inode->block[i], passed, file_ret);
            if (passed == file_ret) {
                assert(res != NULL);
                return res;
            }
        }
    }
    return 0;
}

int Ext2::add_to_dir(uint32_t dir_inode_index, uint32_t added_inode_index, char *filename) {
    Inode *dir_inode = get_inode(dir_inode_index);
    for (int i = 0; i < 15; i++) {
        if (dir_inode->block[i] != 0) {
            if (try_to_add_to_dir_block(dir_inode->block[i], added_inode_index, filename) == 0) {
                Inode *added_inode = get_inode(added_inode_index);
                added_inode->links_count++;
                update_inode(added_inode_index, added_inode);
                return 0;
            }
        }
    }
    if (int block = allocate_block(dir_inode_index); block != -1) {
        if (try_to_add_to_dir_block(block, added_inode_index, filename) == 0) {
            Inode *added_inode = get_inode(added_inode_index);
            added_inode->links_count++;
            update_inode(added_inode_index, added_inode);
            return 0;
        }
    }
    return -1;
}