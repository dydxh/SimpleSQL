#ifndef minisql_scanner_header
#define minisql_scanner_header

#ifndef yyFlexLexerOnce
#include <FlexLexer.h>
#endif

#include "Parser.hpp"
#include "location.hh"

namespace interpreter {

class Scanner: public yyFlexLexer {
public:
    Scanner(std::istream* in) : yyFlexLexer(in) {}
    virtual ~Scanner() {}

    using FlexLexer::yylex;
    virtual int yylex(interpreter::Parser::semantic_type* lval, interpreter::Parser::location_type* loc);

    void CommentError(interpreter::Parser::location_type* loc);

private:
    interpreter::Parser::semantic_type* yylval = nullptr;
};

}
#endif