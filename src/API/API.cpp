#include "API.hpp"

void createTable(const std::string& tablename, const std::string& primarykey, const std::vector<AttrPtr>& attrs) {
    //create table
}

void dropTable(const std::string& tablename) {
    //drop table
}

void inserter(const std::string& tablename, const Record& record) {
    //insert
}

int deleter(const std::string& tablename, const Limits& limit) {
    if(limit.empty() == true) {
        //delete all
    }
    else {

    }
}

std::vector<Record> selecter(const std::string& tablename, const std::vector<std::string>& attrs, const Limits& limit) {

}
