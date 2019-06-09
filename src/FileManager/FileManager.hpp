#ifndef minisql_filemanager_header
#define minisql_filemanager_header

#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../utils/config.hpp"

extern int file_counter;

class FileManager {
    FILE* file;
    int fidx;
    unsigned int blockcnt;
public:
    static std::map<std::string, std::weak_ptr<FileManager>> filebuf;
    FileManager();
    FileManager(const char* filename);
    ~FileManager();

    static bool filexist(const char* filename);
    static void createfile(const char* filename);
    static void writefile(const char* filename, void* src, const int size);
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