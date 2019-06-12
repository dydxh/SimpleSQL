#ifndef minisql_catalogmanager_header
#define minisql_catalogmanager_header

#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <map>
#include "Schema.hpp"
#include "../FileManager/FileManager.hpp"
#include "../BufferManager/BufferManager.hpp"
#include "../IndexManager/IndexManager.hpp"

struct CatalogHeader {
    int blocknumber;
    int schemablk;
    int indexblk;
    int freeblk;
};

class CatalogManager {
public:
    CatalogHeader header;
    FilePtr file;
    BufferPtr buffer;
    std::string name;
    std::map<std::string, SchemaPtr> schemas;
    std::map<std::string, int> name2offset;
    std::map<std::string, IndexPtr> name2index;
    std::map<std::string, std::vector<std::string>> schema2indexes;

    CatalogManager(const BufferPtr& bptr, const std::string name);
    ~CatalogManager() {};

    void readheader();
    void readbody();

    void createTable(const std::string& schemaname, const int& idx, const std::vector<AttrPtr>& attributes);
    void dropTable(const std::string& schemaname);

    void createIndex(const std::string& indexname, const std::string& schemaname, const std::string& columname);
    void dropIndex(const std::string& indexname);

    bool schema_exist(const std::string& name) {
        return schemas.find(name) != schemas.end();
    }
    bool index_exist(const std::string& name) {
        return name2index.find(name) != name2index.end();
    }
};

using CatalogPtr = std::shared_ptr<CatalogManager>;

#endif