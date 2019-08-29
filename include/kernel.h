#include <DiskDriver.h>
#include <DiskDescriptor.h>

class Kernel {
private:
    DiskDriver HD;
public:
    bool attach(char *hdName);
    void readFile();
    void writeFile();
};