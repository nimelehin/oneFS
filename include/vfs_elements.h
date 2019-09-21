#ifndef VFS_Element_H
#define VFS_Element_H

struct vfsElement {
    char filename[8];
    char filenameExtension[3];
    uint8_t attributes;
};


struct vfsDir {
    char *dirPath;
    uint8_t countOfElements;
    vfsElement *elements;
};

#endif //VFS_Element_H