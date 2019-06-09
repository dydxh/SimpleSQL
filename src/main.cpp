#include <cstdio>
#include <iostream>
#include <cstring>
#include "BufferManager/BufferManager.hpp"
#include "CatalogManager/CatalogManager.hpp"
#include "RecordManager/RecordManager.hpp"
#include "utils/ErrorManager.hpp"

int main() {
    try {
        BufferPtr buffermanager = std::make_shared<BufferManager>();
        CatalogPtr catalogmanager = std::make_shared<CatalogManager>(buffermanager, CATALOG_NAME);
        RecordPtr recordmanager = std::make_shared<RecordManager>(buffermanager, catalogmanager, "table_name");

        // return 0;
        Record tmpr;
        Value tmp;
        tmp.type = Type::INT;
        tmp.ptr = new int(1);
        tmpr.push_back(tmp);
        tmp.type = Type::CHAR;
        char newval[] = "dydxh?!";
        tmp.ptr = newval;
        tmp.clen = 7;
        tmpr.push_back(tmp);
        tmp.type = Type::INT;
        tmp.ptr = new int(2333);
        tmpr.push_back(tmp);
        tmp.type = Type::FLOAT;
        tmp.ptr = new float(3.1415926);
        tmpr.push_back(tmp);

        recordmanager->inserter(tmpr);
        // std::vector<AttrPtr> attrs;
        // attrs.push_back(std::make_shared<Attribute>("id", (static_cast<int>(Type::INT) << 1) | 1));
        // attrs.push_back(std::make_shared<Attribute>("name", (0x10 << 3) | (static_cast<int>(Type::CHAR) << 1)));
        // attrs.push_back(std::make_shared<Attribute>("val", static_cast<int>(Type::INT) << 1));
        // attrs.push_back(std::make_shared<Attribute>("weight", static_cast<int>(Type::FLOAT) << 1));
        // catalogmanager->createTable("table_name", 0, attrs);
        // catalogmanager->dropTable("table_name");
    }
    catch(BasicError &e) {
        std::cout << "Error occur" << std::endl;
        std::cout << e.msg << std::endl;
    }
    return 0;
}