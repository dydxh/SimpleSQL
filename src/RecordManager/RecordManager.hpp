#ifndef minisql_recordmanager_header
#define minisql_recordmanager_header

#include <iostream>
#include <cstdio>
#include <memory>
#include "../Type/Value.hpp"
#include "../BufferManager/BufferManager.hpp"
#include "../FileManager/FileManager.hpp"
#include "../CatalogManager/CatalogManager.hpp"

struct RecordHeader {
    long long next;
};

class RecordManager {
public:
    FilePtr file;
    BufferPtr buffer;
    SchemaPtr schema;

    RecordManager(const BufferPtr& buffer, const std::string schemaname);
    ~RecordManager();

    int inserter(const Record& record);
    void deleter() {}
    
};

using RecordPtr = std::shared_ptr<RecordManager>;

#endif