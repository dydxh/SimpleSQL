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

Record cons(int a, const char* b, int c, float d) {
    Record tmpr;
    Value tmp;
    tmp.type = Type::INT;
    tmp.ptr = new int(a);
    tmpr.push_back(tmp);
    tmp.type = Type::CHAR;
    tmp.clen = strlen(b) + 1;
    tmp.ptr = new char[tmp.clen];
    strcpy((char*)tmp.ptr, b);
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
        BufferPtr buffermanager = std::make_shared<BufferManager>();
        CatalogPtr catalogmanager = std::make_shared<CatalogManager>(buffermanager, CATALOG_NAME);
        APIPtr api = std::make_shared<API>(catalogmanager);

        std::vector<AttrPtr> attrs;
        attrs.push_back(std::make_shared<Attribute>("id", (static_cast<int>(Type::INT) << 1) | 1));
        attrs.push_back(std::make_shared<Attribute>("name", (0x10 << 3) | (static_cast<int>(Type::CHAR) << 1)));
        attrs.push_back(std::make_shared<Attribute>("val", static_cast<int>(Type::INT) << 1));
        attrs.push_back(std::make_shared<Attribute>("weight", static_cast<int>(Type::FLOAT) << 1));
        api->createTable("table_name", "id", attrs);
        // catalogmanager->createTable("table_name", 0, attrs);
        // catalogmanager->dropTable("table_name");
        // buffermanager->flush();
        // return 0;

        RecordPtr recordmanager = RecordManager::recordbuf["table_name"];

        api->inserter("table_name", cons(1, "dydxh!?", 2333, 2.345));
        api->inserter("table_name", cons(2, "qaqovoqqqq", 3444, 3.456));
        api->inserter("table_name", cons(3, "just a test", 4555, 4.567));
        api->inserter("table_name", cons(4, "another testow", 5666, 5.678));

        RawLimits limit;
        RawConstraint ccc;
        Value vvv;
        vvv.type = Type::FLOAT; vvv.ptr = new float(4.000);
        ccc.name = "weight"; ccc.op = Operator::LEQ; ccc.val = vvv;
        limit.push_back(ccc);

        std::vector<std::string> tmper;
        tmper.push_back("id");
        tmper.push_back("weight");
        tmper.push_back("val");
        tmper.push_back("name");
        std::vector<Record> res = api->selecter("table_name", tmper, limit);
        
        for(auto& e : res) {
            std::cout << "==> ";
            for(auto& t : e) {
                std::cout << t.tostr() << ' ';
            }
            std::cout << std::endl;
        }

        api->deleter("table_name", limit);
        std::cout << "After delete" << std::endl;
        
        res = api->selecter("table_name", tmper, limit);
        for(auto& e : res) {
            std::cout << "==> ";
            for(auto& t : e) {
                std::cout << t.tostr() << ' ';
            }
            std::cout << std::endl;
        }
        std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;

        limit.clear();
        ccc.op = Operator::GEQ;
        limit.push_back(ccc);

        res = api->selecter("table_name", tmper, limit);
        for(auto& e : res) {
            std::cout << "==> ";
            for(auto& t : e) {
                std::cout << t.tostr() << ' ';
            }
            std::cout << std::endl;
        }
        //recordmanager->deleteall();

        // test the b+ tree
        // IndexPtr index = std::make_shared<IndexManager>(buffermanager, catalogmanager->schemas["table_name"], "id");
        // index->buildIndex();
    } 
    catch(BasicError &e) {
        std::cout << "Error occur" << std::endl;
        std::cout << e.msg << std::endl;
    }

    return 0;
}