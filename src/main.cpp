#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include "BufferManager/BufferManager.hpp"
#include "CatalogManager/CatalogManager.hpp"
#include "RecordManager/RecordManager.hpp"
#include "utils/ErrorManager.hpp"
#include "IndexManger/IndexManager.hpp"
#include "API/API.hpp"
#include "Interpreter/Driver.hpp"

Record cons(int a, const char *b, int c, float d) {
    Record tmpr;
    Value tmp;
    tmp.type = Type::INT;
    tmp.ptr = new int(a);
    tmpr.push_back(tmp);
    tmp.type = Type::CHAR;
    tmp.clen = strlen(b) + 1;
    tmp.ptr = new char[tmp.clen];
    strcpy((char *) tmp.ptr, b);
    tmpr.push_back(tmp);
    tmp.type = Type::INT;
    tmp.ptr = new int(c);
    tmpr.push_back(tmp);
    tmp.type = Type::FLOAT;
    tmp.ptr = new float(d);
    tmpr.push_back(tmp);
    return tmpr;
}

int main() {
    try {
        interpreter::Driver* driver;
        try {
            driver = new interpreter::Driver();
        }
        catch(std::bad_alloc& msg) {
            std::cerr << "Failed to allocate driver: (" << msg.what() << "), exit." << std::endl;
            return 0;
        }

        BufferPtr buffermanager = std::make_shared<BufferManager>();
        API::catalog = std::make_shared<CatalogManager>(buffermanager, CATALOG_NAME);

        std::cout << "[mystery]> ";
        driver->Parse("../test/test.sql");
    } 
    catch(BasicError &e) {
        std::cout << "Error occur" << std::endl;
        std::cout << e.msg << std::endl;
    }

    return 0;
}