#include "ext2.h"
#include <fstream>
#include <sys/stat.h>
#include <cassert>
#include <iostream>

Ext2::Ext2(const char *filename) {
    setup_disk(filename);
}

void Ext2::setup_disk(const char *filename) {
    disk_desc = fopen(filename, "r+");
    assert(disk_desc != NULL);
    struct stat stat_buf;
    int rc = stat(filename, &stat_buf);
    disk_size = (rc == 0 ? stat_buf.st_size : -1);

    load_superblock();
    load_group_table();
}

void Ext2::write(void *ptr, uint32_t start, uint32_t size) {
    fseek(disk_desc, start, SEEK_SET);
    fwrite(ptr, 1, size, disk_desc);
    fflush(disk_desc);
}

void Ext2::read(void *ptr, uint32_t start, uint32_t size) {
    fflush(disk_desc);
    fseek(disk_desc, start, SEEK_SET);
    fread(ptr, 1, size, disk_desc);
}

int Ext2::load_superblock() {
    superblock = (SuperBlock*)malloc(SUPERBLOCK_LEN);
    read(superblock, SUPERBLOCK_START, SUPERBLOCK_LEN);
    return 0;
}

int Ext2::load_group_table() {
    group_table = (GroupDescriptor*)malloc(get_group_table_len());
    read(group_table, get_group_table_start(), get_group_table_len());
    return 0;
}