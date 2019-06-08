#include <cstdio>
#include <iostream>
#include <cstring>
#include "BufferManager/BufferManager.hpp"
#include "CatalogManager/CatalogManager.hpp"
#include "utils/ErrorManager.hpp"

int main() {
    try {
        BufferPtr buffermanager = std::make_shared<BufferManager>();
        CatalogPtr catalogmanager = std::make_shared<CatalogManager>(buffermanager, CATALOG_NAME);

        std::vector<AttrPtr> attrs;
        attrs.push_back(std::make_shared<Attribute>("id", (static_cast<int>(Valuetype::INT) << 1) | 1));
        attrs.push_back(std::make_shared<Attribute>("name", (0x10 << 3) | (static_cast<int>(Valuetype::CHAR) << 1)));
        attrs.push_back(std::make_shared<Attribute>("val", static_cast<int>(Valuetype::INT) << 1));
        attrs.push_back(std::make_shared<Attribute>("weight", static_cast<int>(Valuetype::FLOAT) << 1));
        catalogmanager->createTable("table_name", 0, attrs);
        // catalogmanager->dropTable("table_name");
    }
    catch(BasicError &e) {
        std::cout << "Error occur" << std::endl;
        std::cout << e.msg << std::endl;
    }
    return 0;
}