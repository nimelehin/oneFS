#ifndef Kernel_H
#define Kernel_H

#include <disk_driver.h>
#include <disk_descriptor.h>
#include <virtual_file_system.h>
#include <string>

class Kernel {
private:
    DiskDriver mDisk;
    VirtualFileSystem mVfs;
    char mPath[256];
    uint8_t mPathLen;

    void parseFilename(std::string *filename, std::string *filenameExtension);
public:
    Kernel();
    bool attach(char *hdName);
    void addToCurrentPath(const char *tAddPath);
    void startCmd();
    void readFile();
    void writeFile();
};

#endif //Kernel_H