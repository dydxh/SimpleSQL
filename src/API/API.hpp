#ifndef minisql_api_header
#define minisql_api_header

#include <string>
#include <vector>
#include <utility>
#include "../CatalogManager/CatalogManager.hpp"
#include "../RecordManager/RecordManager.hpp"

class API {
public:
    static CatalogPtr catalog;

    // API(const CatalogPtr& catalog);
    // ~API();

    static void createTable(const std::string& tablename, const std::string& primarykey, const std::vector<AttrPtr>& attrs);

    static void dropTable(const std::string& tablename);

    static void inserter(const std::string& tablename, Record& record);

    static int deleter(const std::string& tablename, const RawLimits& limit = std::vector<RawConstraint>());

    static std::vector<Record> selecter(const std::string& tablename, const std::vector<std::string>& attrs = std::vector<std::string>(), const RawLimits& limit = std::vector<RawConstraint>());

    static bool checklimits(const SchemaPtr& schema, const Limits& limit);

    static void displaymsg(const std::string& schemaname, const std::vector<Record>& records, const std::vector<std::string>& attrs = std::vector<std::string>());
};

using APIPtr = std::shared_ptr<API>;

#endif