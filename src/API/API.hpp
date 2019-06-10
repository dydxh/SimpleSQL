#ifndef minisql_api_header
#define minisql_api_header

#include <string>
#include <vector>
#include <utility>
#include "../CatalogManager/CatalogManager.hpp"
#include "../RecordManager/RecordManager.hpp"

class API {
public:
    static void createTable(const std::string& tablename, const std::string& primarykey, const std::vector<AttrPtr>& attrs);

    static void dropTable(const std::string& tablename);

    static void inserter(const std::string& tablename, const Record& record);

    static int deleter(const std::string& tablename, const Limits& limit);

    static std::vector<Record> selecter(const std::string& tablename, const std::vector<std::string>& attrs, const Limits& limit);
};

#endif