//
// Created by Henry Little on 2019-06-08.
//

#ifndef MINISQL_INDEXMANAGER_H
#define MINISQL_INDEXMANAGER_H
// a index manager should initialize the index file (fileidx) and
#include <string>
#include <cassert>
#include "../BufferManager/BufferManager.hpp"
#include "../RecordManager/RecordManager.hpp"
#include "../CatalogManager/Schema.hpp"
#include "../Type/Value.hpp"

class IndexManager{
public:
    
    IndexManager(const BufferPtr &recordBuffer, const SchemaPtr &schema, const std::string colName);
    ~IndexManager();

    BufferPtr recordBuffer;
    FilePtr recordFile;
    SchemaPtr schema;
    std::string columnName;
    std::string tableName;

    FilePtr indexFile; // the created index file
private:
    Value getVal(long long roffset);
}

#endif //MINISQL_INDEXMANAGER_H
