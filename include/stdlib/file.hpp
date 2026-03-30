#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <map>

class FileHandle {
public:
    std::fstream file;
    bool isOpen;
    
    FileHandle() : isOpen(false) {}
    
    ~FileHandle() {
        if (isOpen) {
            file.close();
        }
    }
};

static std::map<int, FileHandle*> fileHandles;
static int nextFileHandle = 1;

auto FUNC_FILEOPEN = [](std::string filename, std::string mode) -> int {
    FileHandle* fh = new FileHandle();
    
    if (mode == "READ" || mode == "r") {
        fh->file.open(filename, std::ios::in);
    } else if (mode == "WRITE" || mode == "w") {
        fh->file.open(filename, std::ios::out);
    } else if (mode == "APPEND" || mode == "a") {
        fh->file.open(filename, std::ios::app);
    } else {
        fh->file.open(filename, std::ios::in | std::ios::out);
    }
    
    if (!fh->file.is_open()) {
        delete fh;
        return -1;
    }
    
    fh->isOpen = true;
    int handleId = nextFileHandle++;
    fileHandles[handleId] = fh;
    return handleId;
};

auto FUNC_FILECLOSE = [](int handle) -> bool {
    auto it = fileHandles.find(handle);
    if (it == fileHandles.end()) return false;
    
    it->second->file.close();
    it->second->isOpen = false;
    delete it->second;
    fileHandles.erase(it);
    return true;
};

auto FUNC_FILEREAD = [](int handle) -> std::string {
    auto it = fileHandles.find(handle);
    if (it == fileHandles.end()) return "";
    
    std::string line;
    std::getline(it->second->file, line);
    return line;
};

auto FUNC_FILEWRITE = [](int handle, std::string content) -> bool {
    auto it = fileHandles.find(handle);
    if (it == fileHandles.end()) return false;
    
    it->second->file << content;
    return true;
};

auto FUNC_FILEEXISTS = [](std::string filename) -> bool {
    std::fstream testFile(filename, std::ios::in);
    bool exists = testFile.is_open();
    testFile.close();
    return exists;
};
