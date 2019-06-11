#ifndef minisql_driver_header
#define minisql_driver_header

#include "Interpreter/Scanner.hpp"
#include "Parser.hpp"

namespace interpreter {

class Driver {
public:
    Scanner* scanner;
    Parser* parser;
    explicit Driver();
    ~Driver();
    bool Parse(const std::string& filename);

};

}
#endif