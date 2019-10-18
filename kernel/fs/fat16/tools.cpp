#include <fat16.h>

bool Fat16::isAttached() {
    return true;
}

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