#include <fat16.h>

// void Fat16::dummyFileCreation() {
//     // createDir("/", "abc");
//     // writeFile("/", "hello", "txt", "Hello this is new File", 22);
//     // auto tmp = cd("/Hello3/");
//     // std::cout << "\n\n";
//     // createDir("/a/", "b");

//     // readFile("/", "hello");
//     // mkdir("/", "c");
// }

bool Fat16::existPath(const char *tPath) {
    uint16_t tPathSize = strlen(tPath);
    assert(tPathSize > 0 && tPath[0] == '/');

    disk->seek(rootDirStart);
    uint8_t *curretSector = disk->readSector();

    fat16Element tmpElement;
    char currentFolderName[8];
    uint8_t nxtChar = 0;

    for (int ind = 1; ind < tPathSize; ind++) {
        if (tPath[ind] == '/') {
            tmpElement = findElementWithName(curretSector, currentFolderName);
            if (tmpElement.attributes != 0x10 && tmpElement.attributes != 0x11) {
                return false;
            }
            memset(currentFolderName, 0x0, 8);
            nxtChar = 0;
            disk->seek(sectorAddressOfElement(&tmpElement));
            curretSector = disk->readSector();
        } else {
            currentFolderName[nxtChar++] = tPath[ind];
        }
    }
    return true;
}

bool Fat16::isAttached() {
    return true;
}

uint32_t Fat16::sectorAddressOfDataCluster(uint16_t tFirstClusterId) {
    if (tFirstClusterId == 0) {
        return rootDirStart;
    } 
    return bytesPerSector * (tFirstClusterId - 1) + rootDirStart + rootEntries * 32;
}

uint32_t Fat16::sectorAddressOfElement(fat16Element *tElement) {
    return (tElement->attributes == 0x11 ? rootDirStart : 
            bytesPerSector * (tElement->firstBlockId - 1) + rootDirStart + rootEntries * 32);
}

void Fat16::convertToVfs(fat16Element *tFat16Element, vfsElement *tVfsElement) {
    memset(tVfsElement->filename, 0x0, 8);
    memset(tVfsElement->filenameExtension, 0x0, 3);
    memccpy(tVfsElement->filename, tFat16Element->filename, 0x0, 8);
    memccpy(tVfsElement->filenameExtension, tFat16Element->filenameExtension, 0x0, 3);
    tVfsElement->attributes = tFat16Element->attributes;
}