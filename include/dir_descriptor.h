#ifndef Dir_Descriptor_H
#define Dir_Descriptor_H


class DirDescriptor {
public:
    char* dirPath;
    uint8_t objectsCount;
    uint32_t size;
    
    DirDescriptor();
};

#endif //Dir_Descriptor_H