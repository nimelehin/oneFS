#include <HDDescriptor.h>

class FileSystem {
private:
    HDDescriptor *hd;
public:
    FileSystem(HDDescriptor *hd);
    virtual void readFile(char *path, char *filename) = 0;
    virtual void writeFile(char *path, char *filename, char *data) = 0;
    virtual void mkdir(char *path) = 0;
};