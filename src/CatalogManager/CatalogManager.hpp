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

    CatalogManager(const BufferPtr& bptr, const std::string name);
    ~CatalogManager() {};

    void readheader();
    void readbody();

    void createTable(const std::string& schemaname, const int& idx, const std::vector<AttrPtr>& attributes);
    void dropTable(const std::string& schemaname);

    bool schema_exist(const std::string& name) {
        return schemas.find(name) != schemas.end();
    }
};

using CatalogPtr = std::shared_ptr<CatalogManager>;

#endif