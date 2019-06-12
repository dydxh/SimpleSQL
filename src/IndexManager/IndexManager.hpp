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

struct IndexHeader {
    int nextblk, deleted;
    char indexname[MAX_NAME_LEN];
    char schemaname[MAX_NAME_LEN];
    char columnname[MAX_NAME_LEN];
};

class IndexManager {
public:
    IndexHeader header;
    RecordPtr recordManager;
    BufferPtr buffer;
    SchemaPtr schema;
    std::string columnName;
    FilePtr indexFile; // the created index file

    BplusPtr bplusTree;
    IndexManager(const BufferPtr &buffer, const std::string& indexname, const SchemaPtr &schema, std::string colName);

    // ~IndexManager();
    void buildIndex();

    unsigned long long findOne(Value &v);

    void removeOne(const Value &v);

    std::vector<unsigned long long> findByRange(
            bool wl, bool leq, const Value &l,
            bool wr, bool req, const Value &r
    );
    // try to delete the given value in the tree
private:
    void printRecord(unsigned long long ptr);
};

using IndexPtr = std::shared_ptr<IndexManager>;

#endif //MINISQL_INDEXMANAGER_H