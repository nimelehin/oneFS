#ifndef DirDescriptor_FILE
#define DirDescriptor_FILE


class DirDescriptor {
public:
    char* dirPath;
    uint8_t objectsCount;
    uint32_t size;
    
    DirDescriptor();
};

#endif