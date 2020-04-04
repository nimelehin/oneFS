#include "ext2_formatter.h"

#define CALL_WITH_LOG(x) std::cout << "Start " << #x;x;std::cout << " DONE\n";

void Ext2Formatter::setup_superblock(Ext2FormatSettings settings) {
    SuperBlock *sb = (SuperBlock*)malloc(SUPERBLOCK_LEN);
    
    // settings
    uint32_t group_size = settings.block_size * settings.block_size * 8;

    uint32_t disk_size_for_blocks = disk_size - SUPERBLOCK_START;
    uint32_t groups_count = disk_size_for_blocks / group_size;

    uint32_t blocks_per_groups = settings.block_size * 8;

    // gdesc
    uint32_t gdesc_blocks_count_per_block = settings.block_size / sizeof(GroupDescriptor);
    uint32_t gdesc_blocks_count = groups_count / gdesc_blocks_count_per_block;
    if (groups_count % gdesc_blocks_count_per_block) {
        gdesc_blocks_count++;
    }

    // inodes
    uint32_t inodes_count = settings.inodes_per_group * groups_count;
    uint32_t inodes_count_fit_in_block = settings.block_size / sizeof(Inode);
    uint32_t itable_blocks_count_per_group = settings.inodes_per_group / inodes_count_fit_in_block;

    // data blocks
    uint32_t dblocks_count_per_group = blocks_per_groups - itable_blocks_count_per_group - gdesc_blocks_count - 3;
    uint32_t dblocks_count = dblocks_count_per_group * groups_count;

    sb->inodes_count = inodes_count;
    sb->blocks_count = dblocks_count;
    sb->r_blocks_count = 0;
    sb->free_blocks_count = sb->blocks_count; // all are empty
    sb->free_inodes_count = sb->inodes_count; // all are empty
    sb->first_data_block = (settings.block_size == 1024 ? 1 : 0);
    sb->log_block_size = (settings.block_size >> 10) - 1;
    sb->log_frag_size = (settings.block_size >> 10) - 1; // currently haven't frags support
    sb->blocks_per_group = blocks_per_groups;
    sb->frags_per_group = blocks_per_groups; // currently haven't frags support
    sb->inodes_per_group = settings.inodes_per_group;
    sb->mtime = 0;
    sb->wtime = 0;
    sb->mnt_count = 0;
    sb->max_mnt_count = 33;
    sb->magic = EXT2_SUPER_MAGIC;
    sb->state = EXT2_VALID_FS;
    sb->errors = EXT2_ERRORS_CONTINUE;
    sb->minor_rev_level = 0; // TODO add functions
    sb->lastcheck = std::time(0); // TODO add functions
    sb->checkinterval = 0; // 180 days
    sb->creator_os = EXT2_OS_LINUX;
    sb->rev_level = EXT2_GOOD_OLD_REV;
    sb->def_resuid = 0;
    sb->def_resgid = 0;
    sb->first_ino = 11;
    sb->uuid[6] = 'c'; // gen uuid
    sb->uuid[8] = 'e';
    sb->inode_size = sizeof(Inode);
    memcpy(sb->volume_name, "fmtdr", 3);
    
    free(superblock);
    superblock = sb;

    for (int i = 0; i < BACKUPS_CNT; i++) {
        uint32_t pos_to_write = get_offset_of_group(i);
        write(superblock, pos_to_write, SUPERBLOCK_LEN);
    } 
}

void Ext2Formatter::setup_group_tables(Ext2FormatSettings settings) {
    uint32_t group_size = settings.block_size * settings.block_size * 8;
    uint32_t disk_size_for_blocks = disk_size - SUPERBLOCK_START;
    uint32_t groups_count = disk_size_for_blocks / group_size;
    uint32_t gdesc_blocks_count_per_block = settings.block_size / sizeof(GroupDescriptor);
    uint32_t group_table_blocks_taken = groups_count / gdesc_blocks_count_per_block;
    if (groups_count % gdesc_blocks_count_per_block) {
        group_table_blocks_taken++;
    }

    GroupDescriptor *gd = (GroupDescriptor*)malloc(get_group_table_len());
    
    for (int i = 0; i < get_groups_cnt(); i++) {
        uint32_t block_id = get_offset_of_group_in_blocks(i) + 1 + group_table_blocks_taken;
        gd[i].block_bitmap = block_id++;
        gd[i].inode_bitmap = block_id++;
        gd[i].inode_table  = block_id;
        gd[i].used_dirs_count = 0;
        // TODO: last groups may be smaller and will be calced wrong.
        gd[i].free_inodes_count = superblock->inodes_per_group;
        gd[i].free_blocks_count = get_data_blocks_in_group(i);
    }

    free(group_table);
    group_table = gd;

    for (int i = 0; i < BACKUPS_CNT; i++) {
        uint32_t pos_to_write = get_offset_of_group(i) + get_block_len();
        write(gd, pos_to_write, get_group_table_len());
    } 
}

void Ext2Formatter::setup_inode_bitmaps() {
    uint8_t* bitmap = (uint8_t*)malloc(get_block_len());
    for (int i = 0; i < get_groups_cnt(); i++) {
        uint32_t block_id_of_bitmap = group_table[i].inode_bitmap;
        read(bitmap, get_offset_of_block(block_id_of_bitmap), get_block_len());
        
        memset(bitmap, 0, get_block_len());
        for (int j = 1; j <= INODES_RESERVED; j++) {
            bitmap_change_value(bitmap, j, 1);
        }

        write(bitmap, get_offset_of_block(block_id_of_bitmap), get_block_len());
    }
    free(bitmap);
}

void Ext2Formatter::setup_block_bitmaps() {
    uint8_t* bitmap = (uint8_t*)malloc(get_block_len());
    for (int i = 0; i < get_groups_cnt(); i++) {
        uint32_t block_id_of_bitmap = group_table[i].block_bitmap;
        read(bitmap, get_offset_of_block(block_id_of_bitmap), get_block_len());
        
        memset(bitmap, 0, get_block_len());
        // TODO check this: group = 0 [here]                                and (here)
        for (int j = 1; j < group_table[0].inode_table+get_itable_blocks_in_group(0); j++) {
            bitmap_change_value(bitmap, j, 1);
        }

        write(bitmap, get_offset_of_block(block_id_of_bitmap), get_block_len());
    }
    free(bitmap);
}

void Ext2Formatter::setup_root_dir() {
    Inode *root_inode = get_inode(EXT2_ROOT_INO);
    root_inode->mode = EXT2_S_IFDIR | EXT2_S_IRUSR | EXT2_S_IWUSR | EXT2_S_IXUSR | EXT2_S_IRGRP | EXT2_S_IXGRP | EXT2_S_IROTH | EXT2_S_IXOTH;
    update_inode(EXT2_ROOT_INO, root_inode);

    add_to_dir(EXT2_ROOT_INO, EXT2_ROOT_INO, ".");
    add_to_dir(EXT2_ROOT_INO, EXT2_ROOT_INO, "..");
}

bool Ext2Formatter::format(Ext2FormatSettings settings) {
    CALL_WITH_LOG(setup_superblock(settings));
    CALL_WITH_LOG(setup_group_tables(settings));
    CALL_WITH_LOG(setup_inode_bitmaps());
    CALL_WITH_LOG(setup_block_bitmaps());
    CALL_WITH_LOG(setup_root_dir());
    return true;
}