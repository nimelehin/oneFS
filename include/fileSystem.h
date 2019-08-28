#include <HDDriver.h>

class FileSystem {
private:
    HDDriver *hd;
public:
    FileSystem(HDDriver *hd);
    virtual void readFile(char *path, char *filename) = 0;
    virtual void writeFile(char *path, char *filename, char *data) = 0;
    virtual void mkdir(char *path) = 0;
};