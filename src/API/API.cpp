#include "API.hpp"

API::API(const CatalogPtr& catalog) {
    this->catalog = catalog;
}
API::~API() {

}

void API::createTable(const std::string& schemaname, const std::string& primarykey, const std::vector<AttrPtr>& attrs) {
    catalog->createTable(schemaname, 0, attrs);
    
    SchemaPtr schema = this->catalog->schemas[schemaname];
    //create table
}

void API::dropTable(const std::string& tablename) {
    //drop table
}

void API::inserter(const std::string& tablename, const Record& record) {
    //insert
}

int API::deleter(const std::string& tablename, const Limits& limit) {
    if(limit.empty() == true) {
        //delete all
    }
    else {

    }
}

std::vector<Record> API::selecter(const std::string& tablename, const std::vector<std::string>& attrs, const Limits& limit) {

}
