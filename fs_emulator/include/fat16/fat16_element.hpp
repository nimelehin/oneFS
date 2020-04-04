#ifndef Fat16_Element_H
#define Fat16_Element_H

#define FAT16_ELEMENT_FOLDER 0x10
#define FAT16_ELEMENT_ROOT_FOLDER 0x11
#define FAT16_ELEMENT_NULL 0xFF

struct fat16Element {
    char filename[8];
    char filenameExtension[3];
    uint8_t attributes;
    uint16_t firstBlockId;
    uint16_t dataSize;
    // TODO add time of file creation 
    
};

#endif //Fat16_Element_H