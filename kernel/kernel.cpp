#include <kernel.h>
#include <iostream>

Kernel::Kernel(): mDisk("hd.img"), mVfs(), mPathLen(0) {
    mDisk.open();
    mDisk.seek(0);
    char hdName = mVfs.attach(&mDisk);
    std::cout << hdName << "\n";
    std::cout << mVfs.isAttached(hdName) << "\n";
    
}


// TODO attach method isn't ready
bool Kernel::attach(char *hdName) {
    mDisk = DiskDriver(hdName);
    char diskName = mVfs.attach(&mDisk);
    std::cout << diskName << "\n";
    return true;
}

void Kernel::addToCurrentPath(const char *tAddPath) {
    uint8_t addPathLen = strlen(tAddPath);
    std::cout << (int)addPathLen << "\n";
    for (int i = mPathLen; i < mPathLen + addPathLen; i++) {
        mPath[i] = tAddPath[i - mPathLen];
    }
    if (tAddPath[addPathLen - 1] != '/') {
        mPath[mPathLen+addPathLen] = '/';
        addPathLen++;
    }
    mPathLen += addPathLen;
    mPath[mPathLen] = 0;
}

void Kernel::startCmd() {
    using namespace std;
    addToCurrentPath("A:/");
    string currentLine;
    while (true) {
        cout << mPath << ">";
        cin >> currentLine;
        if (currentLine == "cd") {
            string dirPath;
            cin >> dirPath;
            addToCurrentPath(dirPath.c_str());
        }
        if (currentLine == "mkdir") {
            string dirName;
            cin >> dirName;
            mVfs.createDir(mPath, dirName.c_str());
        }
        if (currentLine == "mkfile") {
            string fileName, fileData;
            cin >> fileName;
            cin >> fileData;
            mVfs.writeFile(mPath, fileName.c_str(), "txt", fileData.c_str(), fileData.size());
        }
        if (currentLine == "cat") {
            string fileName;
            cin >> fileName;
            mVfs.readFile(mPath, fileName.c_str());
        }
        if (currentLine == "ls") {
            vfsDir dirDesc = mVfs.ls(mPath);
            for (int i = 0; i < dirDesc.countOfElements; i++) {
                for (int j = 0; j < 8; j++)
                    cout << dirDesc.elements[i].filename[j];
                cout << ".";
                for (int j = 0; j < 3; j++)
                    cout << dirDesc.elements[i].filenameExtension[j];
                cout << "\n";
            }
            // cout << (int)mVfs.ls(mPath).countOfElements << "\n";
        }
    }
}

void Kernel::writeFile() {}
void Kernel::readFile() {}