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

bool Fat16::editClusterWithId(uint16_t tBlockId, uint16_t tNewValue) {
    uint16_t recordIdInFAT = tBlockId + 1;
    uint16_t sectorOfFATWithRecord = recordIdInFAT / (bytesPerSector / 2);
    uint16_t recordIdInSectorOfFat = 2 * (recordIdInFAT % (bytesPerSector / 2));
    disk->seek(startOfFATs + sectorOfFATWithRecord);
    uint8_t *data = disk->readSector();
    uint16_t result = data[recordIdInSectorOfFat + 1] * 0x100 + data[recordIdInSectorOfFat];
    data[recordIdInSectorOfFat] = tNewValue % 0x100;
    data[recordIdInSectorOfFat + 1] = tNewValue / 0x100;
    disk->seek(startOfFATs + sectorOfFATWithRecord);
    disk->writeSector(data);
    return true;
}

bool Fat16::takeClusterWithId(uint16_t tBlockId) {
    return editClusterWithId(tBlockId, 0xffff);
}

bool Fat16::freeClusterWithId(uint16_t tBlockId) {
    return editClusterWithId(tBlockId, 0x0000);
}

uint16_t Fat16::extendClusterWithId(uint16_t tBlockId) {
    uint16_t newBlockId = findFreeCluster();
    editClusterWithId(tBlockId, newBlockId);
    takeClusterWithId(newBlockId);
    return newBlockId;
}