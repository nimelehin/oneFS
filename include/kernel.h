#include <disk_driver.h>
#include <disk_descriptor.h>

class Kernel {
private:
    DiskDriver HD;
public:
    bool attach(char *hdName);
    void readFile();
    void writeFile();
};