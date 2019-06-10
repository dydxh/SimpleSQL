#ifndef minisql_recordmanager_header
#define minisql_recordmanager_header

#include <iostream>
#include <cstdio>
#include <memory>
#include "../Type/Value.hpp"
#include "../Type/constraint.hpp"
#include "../BufferManager/BufferManager.hpp"
#include "../FileManager/FileManager.hpp"
#include "../CatalogManager/CatalogManager.hpp"
#include "Record.hpp"

struct RecordFileHeader {
    int blocknumber;
    int recordnumber;
    unsigned long long recordstart;
    unsigned long long freestart;
};

class RecordManager {
public:
    static std::map<std::string, std::weak_ptr<RecordManager>> recordbuf; // schema name
    RecordFileHeader header;
    FilePtr file;
    BufferPtr buffer;
    SchemaPtr schema;

    RecordManager(const BufferPtr& buffer, const CatalogPtr& catalog, const std::string schemaname);
    ~RecordManager();

    void inserter(const Record& record);
    int deleteall();
    int deleter(const Limits& limit);
    std::vector<Record> selecter(const Limits& limit);
    void readheader();
    void* getval(AttrPtr attr, unsigned long long offset);
    Value getVal(const char * columnName, unsigned long long roffset);
    
};

using RecordPtr = std::shared_ptr<RecordManager>;

#endif