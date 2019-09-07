
struct fat16Element {
    char filename[8];
    char filenameExtension[3];
    uint8_t attributes;
    uint16_t sectorWithData;
    // TODO add time of file creation 
    
};