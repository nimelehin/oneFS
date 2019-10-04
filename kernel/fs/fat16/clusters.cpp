#include <fat16.h>

uint16_t Fat16::findFreeCluster() {
    for (uint32_t sector = 0; sector < sectorsPerFAT; sector++) {
        disk->seek(startOfFATs + sector * bytesPerSector);
        uint8_t *data = disk->readSector();
        for (uint16_t i = 0; i < 512; i+=2) {
            uint16_t result = data[i + 1] * 0x100 + data[i];
            if (result == 0) {
                return (sector * bytesPerSector + i) / 2 - 1;
            }
        }
    }
    return 0;
}

bool Fat16::editCluster(uint16_t tClusterId, uint16_t tNewValue) {
    uint16_t recordIdInFAT = tClusterId + 1;
    uint16_t sectorOfFATWithRecord = recordIdInFAT / (bytesPerSector / 2);
    uint16_t recordIdInSectorOfFat = 2 * (recordIdInFAT % (bytesPerSector / 2));
    disk->seek(startOfFATs + sectorOfFATWithRecord);
    uint8_t *data = disk->readSector();
    data[recordIdInSectorOfFat] = tNewValue % 0x100;
    data[recordIdInSectorOfFat + 1] = tNewValue / 0x100;
    disk->seek(startOfFATs + sectorOfFATWithRecord);
    disk->writeSector(data);
    free(data);
    return true;
}

uint16_t Fat16::getClusterValue(uint16_t tClusterId) {
    uint16_t recordIdInFAT = tClusterId + 1;
    uint16_t sectorOfFATWithRecord = recordIdInFAT / (bytesPerSector / 2);
    uint16_t recordIdInSectorOfFat = 2 * (recordIdInFAT % (bytesPerSector / 2));
    disk->seek(startOfFATs + sectorOfFATWithRecord);
    uint8_t *data = disk->readSector();
    uint16_t result = (uint16_t(data[recordIdInSectorOfFat + 1] << 8) + 
                       uint16_t(data[recordIdInSectorOfFat]));
    free(data);
    return result;
}

bool Fat16::takeCluster(uint16_t tClusterId) {
    return editCluster(tClusterId, 0xffff);
}

bool Fat16::freeCluster(uint16_t tClusterId) {
    return editCluster(tClusterId, 0x0000);
}

// freeSequenceOfClusters is supposed to free sequence of clusters
// starts from tClusterId
bool Fat16::freeSequenceOfClusters(uint16_t tClusterId) {
    uint16_t curCluster = tClusterId, nxtCluster;
    while (curCluster != 0xffff) {
        nxtCluster = getClusterValue(curCluster);
        editCluster(curCluster, 0x0000);
        curCluster = nxtCluster;
    }
    return true;
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