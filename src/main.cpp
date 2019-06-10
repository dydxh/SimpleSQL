#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include "BufferManager/BufferManager.hpp"
#include "CatalogManager/CatalogManager.hpp"
#include "RecordManager/RecordManager.hpp"
#include "utils/ErrorManager.hpp"

void adder(RecordPtr recordmanager, int a, const char* b, int c, float d) {
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

    recordmanager->inserter(tmpr);
}

int main() {
    try {
        BufferPtr buffermanager = std::make_shared<BufferManager>();
        CatalogPtr catalogmanager = std::make_shared<CatalogManager>(buffermanager, CATALOG_NAME);

        // std::vector<AttrPtr> attrs;
        // attrs.push_back(std::make_shared<Attribute>("id", (static_cast<int>(Type::INT) << 1) | 1));
        // attrs.push_back(std::make_shared<Attribute>("name", (0x10 << 3) | (static_cast<int>(Type::CHAR) << 1)));
        // attrs.push_back(std::make_shared<Attribute>("val", static_cast<int>(Type::INT) << 1));
        // attrs.push_back(std::make_shared<Attribute>("weight", static_cast<int>(Type::FLOAT) << 1));
        // catalogmanager->createTable("table_name", 0, attrs);
        // // catalogmanager->dropTable("table_name");
        // buffermanager->flush();
        // return 0;

        RecordPtr recordmanager = std::make_shared<RecordManager>(buffermanager, catalogmanager, "table_name");

        adder(recordmanager, 1, "dydxh!?", 2333, 2.345);
        adder(recordmanager, 2, "qaqovoqqqq", 3444, 3.456);
        adder(recordmanager, 3, "just a test", 4555, 4.567);
        adder(recordmanager, 4, "another testow", 5666, 5.678);

        Limits limit;
        Constraint ccc;
        Value vvv;
        vvv.type = Type::INT; vvv.ptr = new int(3000);
        ccc.attridx = 2; ccc.op = Operator::LEQ; ccc.val = vvv;
        limit.push_back(ccc);
        std::vector<Record> res = recordmanager->selecter(limit);
        for(auto& e : res) {
            std::cout << "==> ";
            for(auto& t : e) {
                std::cout << t.tostr() << ' ';
            }
            std::cout << std::endl;
        }
        recordmanager->deleter(limit);
        std::cout << "After delete" << std::endl;
        res = recordmanager->selecter(limit);
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
        res = recordmanager->selecter(limit);
        for(auto& e : res) {
            std::cout << "==> ";
            for(auto& t : e) {
                std::cout << t.tostr() << ' ';
            }
            std::cout << std::endl;
        }
        recordmanager->deleteall();
    }
    catch(BasicError &e) {
        std::cout << "Error occur" << std::endl;
        std::cout << e.msg << std::endl;
    }
    return 0;
}