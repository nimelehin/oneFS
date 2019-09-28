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

void Kernel::parseFilename(std::string *filename, std::string *filenameExtension) {
    int splitPos = 0;
    for (int i = filename->size() - 1; i >= 0; i--) {
        if (filename->at(i) == '.') {
            splitPos = i;
        }
    }
    for (int i = splitPos+1; i < filename->size(); i++) {
        filenameExtension->push_back(filename->at(i));
    }
    int filenameSize = filename->size();
    for (int i = splitPos; i < filenameSize; i++) {
        filename->pop_back();
    }
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
            string filename, filenameExtension, fileData;
            cin >> filename;
            cin >> fileData;
            parseFilename(&filename, &filenameExtension);
            std::cout << filename << " " << filenameExtension << "\n";
            mVfs.writeFile(mPath, filename.c_str(), filenameExtension.c_str(), fileData.c_str(), fileData.size());
        }
        if (currentLine == "cat") {
            string filename, filenameExtension;
            cin >> filename;
            parseFilename(&filename, &filenameExtension);
            cout << mVfs.readFile(mPath, filename.c_str(), filenameExtension.c_str());
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