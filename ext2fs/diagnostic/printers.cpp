#include "ext2_dtool.h"
#include "iostream"

#define print_debug(x) std::cout << #x" = " << x << std::endl
void Ext2DTool::print_disk_info() {
    print_debug(superblock->inodes_count);
    print_debug(superblock->blocks_count);
    print_debug(superblock->r_blocks_count);
    print_debug(superblock->free_blocks_count);

    print_debug(superblock->free_inodes_count);
    print_debug(superblock->first_data_block);
    print_debug(superblock->log_block_size);
    print_debug(superblock->log_frag_size);

    print_debug(superblock->blocks_per_group);
    print_debug(superblock->frags_per_group);
    print_debug(superblock->inodes_per_group);
    print_debug(superblock->mtime);

    print_debug(superblock->wtime);
    print_debug(superblock->mnt_count);
    print_debug(superblock->max_mnt_count);
    print_debug(superblock->magic);

    print_debug(superblock->state);
    print_debug(superblock->errors);
    print_debug(superblock->minor_rev_level);
    print_debug(superblock->lastcheck);

    print_debug(superblock->checkinterval);
    print_debug(superblock->creator_os);
    print_debug(superblock->rev_level);
    print_debug(superblock->def_resuid);
    print_debug(superblock->def_resgid);

    std::cout << "_______" << "\n";

    print_debug(superblock->first_ino);
    print_debug(superblock->inode_size);
    print_debug(superblock->block_group_nr);
    print_debug(superblock->feature_compat);
    print_debug(superblock->feature_incompat);
    print_debug(superblock->feature_ro_compat);

    for (int i = 0; i < 16; i++) {
        std::cout << char(superblock->uuid[i]) << " ";
    }
    std::cout << "\n";
    for (int i = 0; i < 16; i++) {
        std::cout << char(superblock->volume_name[i]);
    }
    std::cout << "\n";
    std::cout << "\n";
}

void Ext2DTool::print_group_info(uint32_t group_id) {
    print_debug(group_table[group_id].block_bitmap);
    print_debug(group_table[group_id].inode_bitmap);
    print_debug(group_table[group_id].inode_table);
    print_debug(group_table[group_id].free_blocks_count);
    print_debug(group_table[group_id].free_inodes_count);
    print_debug(group_table[group_id].used_dirs_count);
    std::cout << "\n";
}

void Ext2DTool::print_inode_bitmap(uint32_t group_id) {
    uint32_t block_id_of_bitmap = group_table[group_id].inode_bitmap;
    uint8_t* bitmap = (uint8_t*)malloc(get_block_len());
    read(bitmap, get_offset_of_block(block_id_of_bitmap), get_block_len());
    for (int i = 0; i < get_block_len(); i++) {
        std::cout << bitmap_get_value(bitmap, i);
        if (i % 64 == 63) {
            std::cout << "\n";
        }
    }
}

void Ext2DTool::print_data_blocks_bitmap(uint32_t group_id) {
    uint32_t block_id_of_bitmap = group_table[group_id].block_bitmap;
    uint8_t* bitmap = (uint8_t*)malloc(get_block_len());
    read(bitmap, get_offset_of_block(block_id_of_bitmap), get_block_len());
    for (int i = 0; i < get_block_len(); i++) {
        std::cout << bitmap_get_value(bitmap, i);
        if (i % 64 == 63) {
            std::cout << "\n";
        }
    }
}

void Ext2DTool::print_inode(uint32_t inode_id) {
    Inode *inode = get_inode(inode_id);
    print_debug(inode->mode);
    print_debug(inode->uid);
    print_debug(inode->size);
    print_debug(inode->gid);
    print_debug(inode->links_count);
    print_debug(inode->blocks);
    print_debug(inode->flags);
    std::cout << "Blocks:\n";
    for (int i = 0; i < 15; i++) {
        std::cout << inode->block[i] << " ";
    }
    std::cout << "\n";
    std::cout << "\n";
}

void Ext2DTool::print_dir(uint32_t inode_id, uint32_t file) {
    Inode *inode = get_inode(inode_id);
    DirectoryEntry *dir = get_dir(inode, file);
    print_debug(dir->inode);
    print_debug((int)dir->file_type);
    print_debug(dir->rec_len);
    print_debug((int)dir->name_len);
    char* name = (char*)&(dir->file_type) + 1;
    for (int i = 0; i < dir->name_len; i++) {
        std::cout << name[i];
    }
    std::cout << "\n";
    std::cout << "\n";
}