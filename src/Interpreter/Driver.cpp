#include <fstream>

#include "Driver.hpp"

interpreter::Driver::Driver() : scanner(nullptr),parser(nullptr) {}

interpreter::Driver::~Driver() {
    delete(scanner);
    scanner = nullptr;
    delete(parser);
    parser = nullptr;
}

bool interpreter::Driver::Parse(const std::string& filename) {
    std::ifstream fin(filename);
    if(!fin.good()) {
        std::cout << "Cann't open file" << std::endl;
        return false;
    }
    delete(scanner);
    try {
        scanner = new interpreter::Scanner(&fin);
    }
    catch(std::bad_alloc& msg) {
        std::cerr << "Failed to allocate scanner: (" << msg.what() << "), exit." << std::endl;
        return false;
    }

    delete parser;
    try {
        parser = new interpreter::Parser(*scanner, *this);
    }
    catch(std::bad_alloc& msg) {
        std::cerr << "Failed to allocate parser: (" << msg.what() << "), exit." << std::endl;
        return false;
    }

    const int accept = 0;
    if(parser->parse() != accept) {
        std::cerr << "Parse failed." << std::endl;
        return false;
    }
    return true;
}