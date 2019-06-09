#ifndef minisql_recordmanager_header
#define minisql_recordmanager_header

#include <iostream>
#include <cstdio>
#include <memory>
#include "../Type/Value.hpp"
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
    RecordFileHeader header;
    FilePtr file;
    BufferPtr buffer;
    SchemaPtr schema;

    RecordManager(const BufferPtr& buffer, const CatalogPtr& catalog, const std::string schemaname);
    ~RecordManager();

    int inserter(const Record& record);
    void deleter() {}
    void readheader();
    
};

using RecordPtr = std::shared_ptr<RecordManager>;

#endif