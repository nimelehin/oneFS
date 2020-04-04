#include <kernel.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include <fat16/dir_cache.h>

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

void Kernel::addToPath(const char *tAddPath) {
    uint8_t addPathLen = strlen(tAddPath);
    std::cout << (int)addPathLen << "\n";
    uint8_t nxtId = 0;
    char folderName[8];
    for (int i = 0; i < addPathLen; i++) {

    }
    for (int i = mPathLen; i < mPathLen + addPathLen; i++) {
        mPath[i] = tAddPath[i - mPathLen];
    }
    if (tAddPath[addPathLen - 1] != '/') {
        mPath[mPathLen+addPathLen] = '/';
        addPathLen++;
    }
    mPathLen += addPathLen;
    mPath[mPathLen] = 0;
    if (!mVfs.existPath(mPath)) {
        mPathLen -= addPathLen;
        mPath[mPathLen] = 0;
    }
}

void Kernel::startCmd() {
    using namespace std;
    addToPath("A:/");
    string currentLine;
    Fat16DirCache cache = Fat16DirCache();
    while (true) {
        cout << mPath << ">";
        cin >> currentLine;
        if (currentLine == "c") {
            // testing cache
            uint16_t pcluster;
            uint16_t cluster;
            string dirPath;
            cin >> pcluster >> dirPath >> cluster;
            cache.update(pcluster, dirPath.c_str(), cluster);
        }
        if (currentLine == "g") {
            // testing cache
            uint16_t pcluster;
            uint16_t cluster;
            string dirPath;
            cin >> pcluster >> dirPath;
            cout << cache.get(pcluster, dirPath.c_str()) << "\n";
        }
        if (currentLine == "exit" || currentLine == "e") {
            // saveFAT();
            mVfs.stopAll();
            exit(0);
        }
        if (currentLine == "cd") {
            string dirPath;
            cin >> dirPath;
            addToPath(dirPath.c_str());
        }
        if (currentLine == "rmdir") {
            string dirName;
            cin >> dirName;
            mVfs.deleteDir(mPath, dirName.c_str());
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
        if (currentLine == "rmfile") {
            string filename, filenameExtension, fileData;
            cin >> filename;
            parseFilename(&filename, &filenameExtension);
            mVfs.deleteFile(mPath, filename.c_str(), filenameExtension.c_str());
        }
        if (currentLine == "cat") {
            string filename, filenameExtension;
            cin >> filename;
            parseFilename(&filename, &filenameExtension);
            cout << mVfs.readFile(mPath, filename.c_str(), filenameExtension.c_str());
        }
        if (currentLine == "writefile") {
            string filename, filenameExtension, fromFl;
            cin >> filename >> fromFl;
            FILE *fl;
            fl = fopen(fromFl.c_str(), "r+");
            fseek(fl, 0, SEEK_END);
            int siz = ftell(fl);
            cout << siz << " -- \n";
            char* data = (char*)malloc(siz + 1);
            fseek(fl, 0, SEEK_SET);
            fread(data, 1, siz, fl);
            data[siz] = '\0';
            parseFilename(&filename, &filenameExtension);
            cout << mVfs.writeFile(mPath, filename.c_str(), filenameExtension.c_str(), data, siz);
            free(data);
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

void Kernel::runCmd(int argc, char **argv) {
    using namespace std;
    string currentLine(argv[1]);
    if (currentLine == "mkdir") {
        string path("A:"), pathP(argv[2]), dirName(argv[3]);
        path += pathP;
        mVfs.createDir(path.c_str(), dirName.c_str());
    }
    if (currentLine == "writefile") {
        string path("A:"), pathP(argv[2]), filename(argv[3]), filenameExtension, fromFl(argv[4]);
        path += pathP;
        FILE *fl;
        fl = fopen(fromFl.c_str(), "r+");
        fseek(fl, 0, SEEK_END);
        int siz = ftell(fl);
        char* data = (char*)malloc(siz + 1);
        fseek(fl, 0, SEEK_SET);
        fread(data, 1, siz, fl);
        data[siz] = '\0';
        parseFilename(&filename, &filenameExtension);
        cout << mVfs.writeFile(path.c_str(), filename.c_str(), filenameExtension.c_str(), data, siz);
        free(data);
    }
    mVfs.stopAll();
    exit(0);
}

void Kernel::writeFile() {}
void Kernel::readFile() {}
