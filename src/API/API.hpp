#ifndef minisql_api_header
#define minisql_api_header

#include <string>
#include <vector>
#include <utility>
#include "../CatalogManager/CatalogManager.hpp"
#include "../RecordManager/RecordManager.hpp"

class API {
public:
    CatalogPtr catalog;

    API(const CatalogPtr& catalog);
    ~API();

    void createTable(const std::string& tablename, const std::string& primarykey, const std::vector<AttrPtr>& attrs);

    void dropTable(const std::string& tablename);

    void inserter(const std::string& tablename, const Record& record);

    int deleter(const std::string& tablename, const Limits& limit);

    std::vector<Record> selecter(const std::string& tablename, const std::vector<std::string>& attrs, const Limits& limit);
};

#endif