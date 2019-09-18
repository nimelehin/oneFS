#include <fat16.h>

void Fat16::dummyFileCreation() {
    mkdir("/", "a");
    writeFile("/", "hello", "txt", "Hello this is new File", 22);
    //auto tmp = cd("/Hello3/");
    std::cout << "\n\n";
    mkdir("/a/", "b");

    readFile("/", "hello");
    //mkdir("/", "c");
}

bool Fat16::isAttached() {
    return true;
}

uint32_t Fat16::sectorAddressOfDataCluster(uint16_t tFirstClusterId) {
    return bytesPerSector * tFirstClusterId + rootDirStart + rootEntries * 32;
}

uint32_t Fat16::sectorAddressOfElement(fat16Element *tElement) {
    return (tElement->attributes == 0x11 ? rootDirStart : 
            bytesPerSector * tElement->firstBlockId + rootDirStart + rootEntries * 32);
}