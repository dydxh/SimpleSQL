#include <cstdio>
#include <cstring>
#include <iostream>
#include <cassert>
#include <sys/stat.h>
#include "FileManager.hpp"
#include "../utils/ErrorManager.hpp"

char emptybuffer[BLOCK_SIZE];
int file_counter = 0;

std::map<std::string, std::weak_ptr<FileManager>> FileManager::filebuf;

FileManager::FileManager() {
    file = nullptr;
    blockcnt = fidx = 0;
    todelete = false;
}

FileManager::FileManager(const char* filename) {
    this->filename = std::string(filename);
    file = fopen(filename, "rb+");
    if(file == nullptr)
        throw OpenFileError("Fail to open file " + this->filename);
    fseek(file, 0, SEEK_END);
    blockcnt = ftell(file) / BLOCK_SIZE;
    fidx = file_counter++;
    todelete = false;
}

FileManager::~FileManager() {
    if(file != nullptr) {
        fclose(file);
        if(todelete)
            assert(!remove(this->filename.c_str()));
    }
    blockcnt = fidx = 0;
}

bool FileManager::filexist(const char* filename) {
    struct stat tmpbuf;
    return (stat(filename, &tmpbuf) == 0);
}

void FileManager::createfile(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    fclose(fp);
}

void FileManager::writefile(const char* filename, void* src, const int size) {
    FILE* fp = fopen(filename, "wb+");
    fwrite(src, size, 1, fp);
    fwrite(emptybuffer, BLOCK_SIZE - size, 1, fp);
    fclose(fp);
}

void FileManager::openfile(const char* filename) {
    file = fopen(filename, "rb+");
    if(file == nullptr) 
        throw OpenFileError("Fail to open file " + std::string(filename));
    fseek(file, 0, SEEK_END);
    blockcnt = ftell(file) / BLOCK_SIZE;
    fidx = file_counter++;
}

int FileManager::allocblock() {
    fseek(file, 0, SEEK_END);
    int blocknum = ftell(file) / BLOCK_SIZE;
    fwrite(emptybuffer, BLOCK_SIZE, 1, file);
    fflush(file);
    blockcnt += 1;
    return blocknum;
}

void FileManager::readblock(int blocknum, void* buffer) {
    fseek(file, blocknum * BLOCK_SIZE, SEEK_SET);
    fread(buffer, BLOCK_SIZE, 1, file);
}

void FileManager::writeblock(int blocknum, void* buffer) {
    fseek(file, blocknum * BLOCK_SIZE, SEEK_SET);
    fwrite(buffer, BLOCK_SIZE, 1, file);
    fflush(file);
}
