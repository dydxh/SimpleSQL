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
#include "./BplusTree.hpp"

class IndexManager{
    RecordPtr recordManager;
    BufferPtr buffer;
    FilePtr recordFile;
    SchemaPtr schema;
    std::string columnName;
    std::string tableName;
    FilePtr indexFile; // the created index file

    BplusPtr bplusTree;
public:
    IndexManager(const BufferPtr &buffer, const SchemaPtr &schema, const std::string colName);
    // ~IndexManager();
    void buildIndex();
};

using IndexPtr = std::shared_ptr<IndexManager>;

#endif //MINISQL_INDEXMANAGER_H