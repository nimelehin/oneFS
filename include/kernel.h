#include <HDDriver.h>
#include <HDDescriptor.h>

class Kernel {
private:
    HDDriver HD;
public:
    bool attach(char *hdName);
    void readFile();
    void writeFile();
};