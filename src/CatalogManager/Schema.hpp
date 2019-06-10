#ifndef minisql_scheme_header
#define minisql_scheme_header

#include <cstdio>
#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <map>
#include "../FileManager/FileManager.hpp"
#include "../BufferManager/BufferManager.hpp"
#include "Attribute.hpp"
#include "../utils/config.hpp"
#include "../Type/constraint.hpp"

struct SchemaHeader {
    int nextblk, deleted, attrcnt, primaryidx;
    char name[MAX_NAME_LEN];
};

class Schema {
public:
    SchemaHeader header;
    FilePtr file;
    BufferPtr buffer;
    std::map<std::string, AttrPtr> name2attrs;
    std::vector<AttrPtr> attrs;
    std::map<std::string, int> name2offset;
    std::vector<int> idx2offset;

    Schema(const FilePtr& fptr, const BufferPtr& bptr) : file(fptr), buffer(bptr) {}
    ~Schema() {}
    bool checkConstraint(const Limits& limit);
    int getidx(const std::string& name);
};

using SchemaPtr = std::shared_ptr<Schema>;

#endif