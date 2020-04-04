#include "diagnostic/ext2_dtool.h"
#include "format/ext2_formatter.h"

enum SUBPROG_TYPES {
    FORMAT,
    DIAGNOSTIC,
};

struct launch_params {
    const char * filename;
    int subprogram_type;
};

void format_prog(const char *filename) {
    Ext2Formatter ext2 = Ext2Formatter(filename);
    Ext2FormatSettings sets;
    sets.block_size = 1024;
    sets.inodes_per_group = 1712;
    ext2.format(sets);
}

void diagnostic_prog(const char *filename) {
    Ext2DTool ext2d = Ext2DTool("hd.img");
    
    std::string cmd;
    while (true) {
        std::cout << ">> ";
        std::cin >> cmd;
        if (cmd == "exit") {
            return;
        }
        if (cmd == "inode") {
            int num;
            std::cin >> num;
            ext2d.print_inode(num);
        }
        if (cmd == "dir") {
            int inode_index, file_at;
            std::cin >> inode_index >> file_at;
            ext2d.print_dir(inode_index, file_at);
        }
        if (cmd == "superblock" || cmd == "sb") {
            ext2d.print_disk_info();
        }
        if (cmd == "inode_bitmap" || cmd == "ib") {
            int num;
            std::cin >> num;
            ext2d.print_inode_bitmap(num);
        }
        if (cmd == "blocks_bitmap" || cmd == "bb") {
            int num;
            std::cin >> num;
            ext2d.print_data_blocks_bitmap(num);
        }
        if (cmd == "group" || cmd == "gp") {
            int num;
            std::cin >> num;
            ext2d.print_group_info(num);
        }
    }
}

void parse_param(int argv, char** argc, int &i, launch_params &lp) {
    if (argc[i][0] == '-') {
        if (strcmp(argc[i], "-f") == 0) {
            lp.subprogram_type = FORMAT;
            i++;
        }
        if (strcmp(argc[i], "-d") == 0) {
            lp.subprogram_type = DIAGNOSTIC;
            i++;
        }
    } else {
        lp.filename = argc[i];
        i++;
    }
}

void launcher(launch_params lp) {
    switch (lp.subprogram_type) {
        case FORMAT:
            format_prog(lp.filename);
            break;
        case DIAGNOSTIC:
            diagnostic_prog(lp.filename);
            break;
        default:
            std::cout << "No program to run\n";
            break;
    }
}

int main(int argv, char** argc) {
    launch_params lp;

    if (argv == 1) {
        std::cout << "Print filename: ";
        std::string filename;
        std::cin >> filename;
        lp.filename = filename.c_str();
    } else {
        for (int i = 1; i < argv; parse_param(argv, argc, i, lp)) {}
    }

    launcher(lp);
}