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
        throw AttributeError("Primary key '" + primarykey + "' doesn't exist.");
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
    RecordManager::recordbuf[schemaname]->inserter(record);
}

int API::deleter(const std::string& schemaname, const Limits& limit) {
    int retval = 0;
    if(limit.empty() == true)
        retval = RecordManager::recordbuf[schemaname]->deleteall();
    else
        retval = RecordManager::recordbuf[schemaname]->deleter(limit);
    return retval;
}

std::vector<Record> API::selecter(const std::string& schemaname, const std::vector<std::string>& attrs, const Limits& limit) {
    std::vector<Record> retval;
    std::vector<int> idxs;
    RecordPtr tmpptr = RecordManager::recordbuf[schemaname];
    for(auto& e : attrs) {
        int idx = tmpptr->schema->getidx(e);
        if(idx == -1) throw AttributeError("Attribute '" + e + "' doesn't exist.");
        else idxs.push_back(idx);
    }
    retval = tmpptr->project(tmpptr->selecter(limit), idxs);
    return retval;
}
