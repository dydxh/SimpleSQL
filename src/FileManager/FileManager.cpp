#include <cstdio>
#include <iostream>
#include "FileManager.hpp"
#include "../utils/ErrorManager.hpp"

char emptybuffer[BLOCK_SIZE];

FileManager::FileManager() {
    file = nullptr;
    blockcnt = fidx = 0;
}

FileManager::FileManager(const char* filename) {
    file = fopen(filename, "rb+");
    if(file == nullptr)
        throw OpenFileError("Fail to open file " + std::string(filename));
    fseek(file, 0, SEEK_END);
    blockcnt = ftell(file) / BLOCK_SIZE;
    fidx = file_counter++;
}

FileManager::~FileManager() {
    if(file != nullptr)
        fclose(file);
    blockcnt = fidx = 0;
}

void FileManager::createfile(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    fclose(fp);
}

void FileManager::openfile(const char* filename) {
    file = fopen(filename, "rb+");
    if(file == nullptr) OpenFileError("Fail to open file " + std::string(filename));
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
