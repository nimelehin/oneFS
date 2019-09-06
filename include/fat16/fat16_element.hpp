
struct fat16Element {
    char filename[8];
    char filenameExtension[3];
    uint8_t attributes;
    // TODO add time of file creation 
    
};