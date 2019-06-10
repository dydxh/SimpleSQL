#include "API.hpp"

API::API(const CatalogPtr& catalog) {
    this->catalog = catalog;
}
API::~API() {

}

void API::createTable(const std::string& schemaname, const std::string& primarykey, const std::vector<AttrPtr>& attrs) {
    int idx = -1;
    for(int i = 0; i < attrs.size(); i++) {
        if(attrs[i]->name == primarykey) {
            idx = i;
            break;
        }
    }
    if(idx == -1) {
        throw AttributeError("[Error] Primary key '" + primarykey + "' doesn't exist.");
    }
    catalog->createTable(schemaname, idx, attrs);

    RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog, schemaname);
    RecordManager::recordbuf[schemaname] = recordmanager;
}

void API::dropTable(const std::string& schemaname) {
    catalog->dropTable(schemaname);
    if(RecordManager::recordbuf[schemaname]) {
        RecordManager::recordbuf[schemaname]->droptable();
    }
}

void API::inserter(const std::string& schemaname, const Record& record) {
    SchemaPtr schema = RecordManager::recordbuf[schemaname]->schema;

    if(record.size() != schema->attrs.size())
        throw TypeError("[Error] Column number error, except " + std::to_string(record.size()) + " got " + std::to_string(schema->attrs.size()) + ".");
    for(int i = 0; i < record.size(); i++) {
        if(record[i].type != schema->attrs[i]->vtype) {
            std::string msg = "[Error] Insert " + std::to_string(i) + "th data type mismatch, except " + tostr(schema->attrs[i]->vtype) + " got " + tostr(record[i].type);
            throw TypeError(msg);
        }
        if(record[i].type == Type::CHAR && record[i].clen > schema->attrs[i]->clen) {
            std::string msg = "[Error] The string data(len < " + std::to_string(schema->attrs[i]->clen) + ") is too long.";
            throw TypeError(msg);
        }
    }

    RecordManager::recordbuf[schemaname]->inserter(record);
}

int API::deleter(const std::string& schemaname, const RawLimits& rawlimits) {
    SchemaPtr schema = RecordManager::recordbuf[schemaname]->schema;
    Limits limit = schema->translimits(rawlimits);
    checklimits(schema, limit);

    int retval = 0;
    if(limit.empty() == true)
        retval = RecordManager::recordbuf[schemaname]->deleteall();
    else
        retval = RecordManager::recordbuf[schemaname]->deleter(limit);
    return retval;
}

std::vector<Record> API::selecter(const std::string& schemaname, const std::vector<std::string>& attrs, const RawLimits& rawlimits) {
    SchemaPtr schema = RecordManager::recordbuf[schemaname]->schema;
    Limits limit = schema->translimits(rawlimits);
    checklimits(schema, limit);
    
    std::vector<Record> retval;
    std::vector<int> idxs;
    RecordPtr tmpptr = RecordManager::recordbuf[schemaname];
    for(auto& e : attrs) {
        int idx = tmpptr->schema->getidx(e);
        if(idx == -1) throw AttributeError("[Error] Attribute '" + e + "' doesn't exist.");
        else idxs.push_back(idx);
    }
    retval = tmpptr->project(tmpptr->selecter(limit), idxs);
    return retval;
}

bool API::checklimits(const SchemaPtr& schema, const Limits& limit) {
    for(auto& e : limit) {
        if(e.val.type == schema->attrs[e.attridx]->vtype)
            continue;
        throw AttributeError("[Error] Constraint on '" + schema->attrs[e.attridx]->name + "' has an unmatched value type.");
    }
    return true;
}
