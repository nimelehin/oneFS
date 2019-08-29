#include <DiskDriver.h>
#include <HDDescriptor.h>

class Kernel {
private:
    DiskDriver HD;
public:
    bool attach(char *hdName);
    void readFile();
    void writeFile();
};