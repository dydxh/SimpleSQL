#ifndef minisql_filemanager_header
#define minisql_filemanager_header

#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include "../utils/config.hpp"

int file_counter = 0;

class FileManager {
    FILE* file;
    int fidx;
    unsigned int blockcnt;
public:
    FileManager();
    FileManager(const char* filename);
    ~FileManager();

    void createfile(const char* filename);
    void openfile(const char* filename);
    
    inline FILE* handler() {return file;}
    inline int blockcount() {return blockcnt;}
    inline int fileidx() {return fidx;}

    int allocblock();
    void readblock(int blockid, void* buffer);
    void writeblock(int blockid, void* buffer);
};

using FilePtr = std::shared_ptr<FileManager>;

#endif