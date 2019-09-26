#ifndef Fat16_Element_H
#define Fat16_Element_H


struct fat16Element {
    char filename[8];
    char filenameExtension[3];
    uint8_t attributes;
    uint16_t firstBlockId;
    uint16_t dataSize;
    // TODO add time of file creation 
    
};

#endif //Fat16_Element_H