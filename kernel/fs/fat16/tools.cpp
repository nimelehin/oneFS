#include <fat16.h>

void Fat16::dummyFileCreation() {
    mkdir("/", "a");
    //createFile("/", "hello", "txt")
    //auto tmp = cd("/Hello3/");
    std::cout << "\n\n";
    mkdir("/a/", "b");
    //mkdir("/", "c");
}

bool Fat16::isAttached() {
    return true;
}

uint32_t Fat16::sectorAddressOfDataBlock(fat16Element *tElement) {
    return (tElement->attributes == 0x11 ? rootDirStart : 
            bytesPerSector * tElement->firstBlockId + rootDirStart + rootEntries * 32);
}