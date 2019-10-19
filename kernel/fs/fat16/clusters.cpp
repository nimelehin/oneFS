#include <fat16.h>

// findFreeCluster is supposed to find free cluster using fat16
// Will return data cluster id (STARTS WITH 1)
uint16_t Fat16::findFreeCluster() {
    for (uint16_t sector = 0; sector < bytesPerSector * sectorsPerFAT; sector+=2) {
        uint16_t result = (mFileAllocationTable[sector+1] << 8) + mFileAllocationTable[sector];
        if (result == 0) {
            return sector / 2 - 1;
        }
    }
    return 0;
}

bool Fat16::editCluster(uint16_t tClusterId, uint16_t tNewValue) {
    uint16_t recordIdInFAT = 2 * (tClusterId + 1);
    mFileAllocationTable[recordIdInFAT] = tNewValue % 0x100;
    mFileAllocationTable[recordIdInFAT+1] = (tNewValue >> 8) % 0x100;
    return true;
}

uint16_t Fat16::getClusterValue(uint16_t tClusterId) {
    uint16_t recordIdInFAT = 2 * (tClusterId + 1);
    uint16_t result = (uint16_t(mFileAllocationTable[recordIdInFAT + 1] << 8) + 
                       uint16_t(mFileAllocationTable[recordIdInFAT]));
    return result;
}

bool Fat16::takeCluster(uint16_t tClusterId) {
    return editCluster(tClusterId, 0xffff);
}

// freeNextCluster is supposed to free NEXT cluster after tClusterId
bool Fat16::freeNextCluster(uint16_t tClusterId) {
    uint16_t nextCluster = getClusterValue(tClusterId);
    if (nextCluster != 0xffff) {
        return editCluster(tClusterId, 0xffff) | editCluster(nextCluster, 0x0000);
    }
    return true;
}

// freeSequenceOfClusters is supposed to free sequence of clusters
// starts from NEXT cluster after tClusterId
bool Fat16::freeSequenceOfClusters(uint16_t tClusterId) {
    uint16_t curCluster = tClusterId, nxtCluster;
    while (curCluster != 0xffff) {
        nxtCluster = getClusterValue(curCluster);
        editCluster(curCluster, 0x0000);
        curCluster = nxtCluster;
    }
    return true;
}

// makeClusterLast is supposed to free all next clusters and set
// tClusterId as last cluster
bool Fat16::makeClusterLast(uint16_t tClusterId) {
    uint16_t nextCluster = getClusterValue(tClusterId);
    freeSequenceOfClusters(nextCluster);
    return editCluster(tClusterId, 0xffff);
}

// allocateCluster is supposed to atomic allocate a new cluster
uint16_t Fat16::allocateCluster() {
    uint16_t clusterId = findFreeCluster();
    bool result = takeCluster(clusterId);
    return clusterId;
}

// extendCluster is supposed to extend cluster pushing
// a new allocated cluster back
uint16_t Fat16::extendCluster(uint16_t tClusterId) {
    uint16_t newBlockId = allocateCluster();
    editCluster(tClusterId, newBlockId);
    return newBlockId;
}

// getNextCluster is supposed to return cluster id if there is a
// cluster after the given cluster if not it extends the given cluster
uint16_t Fat16::getNextCluster(uint16_t tClusterId) {
    uint16_t nextCluster = getClusterValue(tClusterId);
    if (nextCluster == 0xffff) {
        nextCluster = extendCluster(tClusterId);
    }
    return nextCluster;
}