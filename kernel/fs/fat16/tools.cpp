#include <fat16.h>

// REQUIRED BY VFS
// isAttached is supposed to let VFS know that drive is connected
bool Fat16::isAttached() {
    return true;
}

// DEBUG tool
// printFAT is supposed to print segment of FAT
void Fat16::printFAT(uint16_t tStart, uint16_t tEnd) {
    for (uint16_t i = tStart; i < tEnd; i++) {
        if (i % 32 == 0) {
            std::cout << "\n";
        }
        int fstp = mFileAllocationTable[i] / 16;
        int sstp = mFileAllocationTable[i] % 16;
        if (fstp >= 10) {
            std::cout << char('A' + fstp - 10);
        } else {
            std::cout << char('0' + fstp);
        }
        if (sstp >= 10) {
            std::cout << char('A' + sstp - 10);
        } else {
            std::cout << char('0' + sstp);
        }
        if (i % 2 == 1) {
            std::cout << " ";
        }
    }
}

// getSectorAddress is supposed to convert clousterId to disk offset
uint32_t Fat16::getSectorAddress(uint16_t tFirstClusterId) {
    if (tFirstClusterId == 0) {
        return rootDirStart;
    } 
    return bytesPerSector * (tFirstClusterId - 1) + rootDirStart + rootEntries * 32;
}

// getSectorAddress is supposed to convert clousterId to disk offset
uint32_t Fat16::getSectorAddress(fat16Element *tElement) {
    return (tElement->attributes == 0x11 ? rootDirStart : 
            bytesPerSector * (tElement->firstBlockId - 1) + rootDirStart + rootEntries * 32);
}