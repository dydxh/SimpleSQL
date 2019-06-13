#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>
#include "BufferManager/BufferManager.hpp"
#include "CatalogManager/CatalogManager.hpp"
#include "RecordManager/RecordManager.hpp"
#include "utils/ErrorManager.hpp"
#include "IndexManager/IndexManager.hpp"
#include "API/API.hpp"
#include "Interpreter/Driver.hpp"

interpreter::Driver* driver;
BufferPtr buffermanager;

int main() {
    try {
        driver = new interpreter::Driver();
    }
    catch(std::bad_alloc& msg) {
        std::cerr << "Failed to allocate driver: (" << msg.what() << "), exit." << std::endl;
        return 0;
    }
    try {
        buffermanager = std::make_shared<BufferManager>();
        API::catalog = std::make_shared<CatalogManager>(buffermanager, CATALOG_NAME);
    }
    catch(BasicError &e) {
        std::cout << "Error occur" << std::endl;
        std::cout << e.msg << std::endl;
    }

    while(true) {
        try {
            try {
                std::cout << "[mystery]> ";
                driver->Parse();
            }
            catch(BasicError &e) {
                std::cout << "Error occur" << std::endl;
                std::cout << e.msg << std::endl;
            }
        }
        catch (std::logic_error &e) {
            continue;
        }
    }

    return 0;
}