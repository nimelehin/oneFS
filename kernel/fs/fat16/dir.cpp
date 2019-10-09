#include <fat16.h>

void Fat16::initDir(uint16_t firstClusterId, uint16_t rootDirClusterId, uint8_t rootDirAttributes) {
    fat16Element rootFolder;
    rootFolder.attributes = 0x10;
    memset(rootFolder.filename, 0x0, 8);
    memccpy(rootFolder.filename, "..", 0, 8);
    memset(rootFolder.filenameExtension, 0x0, 3);
    rootFolder.firstBlockId = rootDirClusterId;
    rootFolder.attributes = rootDirAttributes;
    // writing to the disk
    uint8_t *fdata = encodeElement(&rootFolder);
    saveElement(sectorAddressOfDataCluster(firstClusterId), fdata);
}

vfsDir Fat16::getDir(const char *tPath) {
    fat16Element* elements = getFilesInDir(tPath);
    vfsElement *resultElements = new vfsElement[16];
    vfsElement tmpElement;
    uint8_t nextElementId = 0;
    for (uint8_t i = 0; i < 16; i++) {
        if (elements[i].filename[0] != 0) {
            convertToVfs((elements + i), &tmpElement);
            resultElements[nextElementId++] = tmpElement;
        }
    }
    vfsDir resultDir;
    resultDir.elements = resultElements;
    resultDir.countOfElements = nextElementId;
    return resultDir;
}

bool Fat16::createDir(const char  *tPath, const char *tFolderName) {
    fat16Element saveToFolder = cd(tPath);
    
    // creating fat16 folder
    fat16Element newFolder;
    newFolder.attributes = 0x10;
    memset(newFolder.filename, 0x0, 8);
    memccpy(newFolder.filename, tFolderName, 0, 8);
    memset(newFolder.filenameExtension, 0x0, 3);

    // finding sector for the folder
    newFolder.firstBlockId = findFreeCluster();
    takeCluster(newFolder.firstBlockId);
    initDir(newFolder.firstBlockId, saveToFolder.firstBlockId, saveToFolder.attributes);
    
    // writing to the disk
    std::cout << "Saving in " << sectorAddressOfElement(&saveToFolder) << "\nEND\n";
    uint8_t *fdata = encodeElement(&newFolder);
    return saveElement(sectorAddressOfElement(&saveToFolder), fdata);
}

fat16Element* Fat16::getFilesInDir(const char *tPath) {
    fat16Element tmpElement = cd(tPath);
    disk->seek(sectorAddressOfElement(&tmpElement));
    uint8_t *data = disk->readSector();
    fat16Element *result = new fat16Element[16];
    uint8_t addId = 0;
    for (uint16_t offset = 0; offset < bytesPerSector; offset += 32) {
        uint8_t *element = (uint8_t*)malloc(32);
        memcpy(element, data+offset, 32);
        tmpElement = decodeElement(element);
        result[addId++] = tmpElement;
    }
    return result;
}