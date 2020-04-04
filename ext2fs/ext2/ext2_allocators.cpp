#include "ext2.h"


int Ext2::find_free_block(uint8_t* bitmap) {
    for (int i = 0; i < get_block_len(); i++) {
        if (bitmap_get_value(bitmap, i) == 0) {
            return i;
        }
    }
    return -1;
}

int Ext2::allocate_block(uint32_t inode_index) {
    uint32_t group_index = (inode_index - 1) / superblock->inodes_per_group;
    uint32_t start_group_index = group_index;
    uint32_t groups_cnt = get_groups_cnt();
    uint8_t *bitmap = (uint8_t*)malloc(get_block_len());
    int found_block = -1;
    do {
        uint32_t read_from = get_offset_of_block(group_table[group_index].block_bitmap);
        read(bitmap, read_from, get_block_len());
        int res = find_free_block(bitmap);
        if (res != -1) {
            found_block = get_offset_of_group_in_blocks(group_index) + res;
            bitmap_change_value(bitmap, res, 1);
            write(bitmap, read_from, get_block_len());
            free(bitmap);
            break;
        }
        group_index++;
        if (group_index == groups_cnt) {
            group_index = 0;
        }
    } while (group_index != start_group_index);
    
    if (found_block == -1) {
        return -1;
    }
    
    Inode *inode = get_inode(inode_index);
    inode->size += get_block_len();
    inode->blocks += get_block_len() / 512;
    for (int i = 0; i < 15; i++) {
        if (inode->block[i] == 0) {
            inode->block[i] = found_block;
            update_inode(inode_index, inode);
            free(inode);
            return found_block;
        }
    }

    assert(false && "No free space in the INODE");
}

int Ext2::try_to_add_to_dir_block(uint32_t block_index, uint32_t added_inode_index, char *filename) {
    int filename_real_len = strlen(filename);
    int filename_len = norm_filename_len(filename_real_len);
    int min_rec_len = 8 + filename_len;

    DirectoryEntry* block_data = (DirectoryEntry*)malloc(get_block_len());
    read(block_data, get_offset_of_block(block_index), get_block_len());
    DirectoryEntry* start_of_entry = block_data;
    DirectoryEntry* start_of_new_entry = block_data;
    DirectoryEntry new_entry;
    for (;;) {
        if (start_of_entry->rec_len == 0 || start_of_entry->inode == 0) {
            // Setting it as first element in the dir.
            new_entry.inode = added_inode_index;
            new_entry.rec_len = get_block_len();
            new_entry.name_len = filename_real_len;
            start_of_new_entry = start_of_entry;
            goto update_res;
        } else {
            int cur_filename_len = norm_filename_len(start_of_entry->name_len);
            int cur_rec_len = 8 + cur_filename_len;
            
            // We have enough place to put both of them
            
            if (start_of_entry->rec_len >= cur_rec_len + min_rec_len) {
                new_entry.inode = added_inode_index;
                new_entry.rec_len = start_of_entry->rec_len - cur_rec_len;
                new_entry.name_len = filename_real_len;
                start_of_new_entry = (DirectoryEntry*)((uint64_t)start_of_entry + cur_rec_len);
                start_of_entry->rec_len = cur_rec_len;
                goto update_res;
            }
            
            start_of_entry = (DirectoryEntry*)((uint64_t)start_of_entry + start_of_entry->rec_len);
            if (start_of_entry >= block_data + get_block_len()) {
                return -1;
            }
        }
    }
update_res:
    memcpy(start_of_new_entry, &new_entry, 8);
    memcpy((void*)((uint64_t)start_of_new_entry+8), filename, filename_real_len);
    memset((void*)((uint64_t)start_of_new_entry+8+filename_real_len), 0, filename_len-filename_real_len);
    write(block_data, get_offset_of_block(block_index), get_block_len());
    return 0;
}