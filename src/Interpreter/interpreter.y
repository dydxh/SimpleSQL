%skeleton "lalr1.cc"
%require "3.0"
%debug
%defines
%define api.namespace {interpreter}
%define api.parser.class {Parser}
%define api.value.type variant
%locations

%code requires {
    #include "CatalogManager/CatalogManager.hpp"
    #include "RecordManager/RecordManager.hpp"
    #include "API/API.hpp"
    #include <iostream>
    #include <memory>
    #include <string>
    #include <stdexcept>
    namespace interpreter {
        class Scanner;
        class Driver;
    }
    using namespace interpreter;
}

%parse-param {Scanner& scanner}
%parse-param {Driver& driver}

%code {
    #include "Interpreter/Driver.hpp"
    #include "Interpreter/Scanner.hpp"
    #undef yylex
    #define yylex scanner.yylex
}

%define parse.error verbose
%define parse.assert

%token TERMINATE 0 "end of file"
%token AND CHAR CREATE DELETE DROP EXECFILE FLOAT FROM
%token INDEX INSERT INT INTO KEY ON PRIMARY QUIT SELECT
%token TABLE UNIQUE VALUES WHERE SEMICOLON
%token LPAREN RPAREN COMMA EQ LT LE NE GT GE STAR
%token INTEGER STRING IDENTIFIER REALNUMBER

%type <int> INTEGER
%type <float> REALNUMBER
%type <std::string> IDENTIFIER STRING
%type <std::vector<AttrPtr>> attribute_decl_list
%type <AttrPtr> attribute_decl
%type <MiddleType> attribute_type
%type <std::vector<std::string>> attribute_list
%type <RawLimits> constraint_list
%type <RawConstraint> constraint
%type <Operator> operation
%type <Record> value_list
%type <Value> value


%start program

%%
program: sqlstmt_list
    ;
    
sqlstmt_list: sqlstmt_list sqlstmt SEMICOLON {
    
    }
    | sqlstmt SEMICOLON {
        
    }
    ;

sqlstmt: create_table_stmt
    | drop_table_stmt
    | create_index_stmt
    | drop_index_stmt
    | select_stmt
    | insert_stmt
    | delete_stmt
    | quit_stmt
    | execfile_stmt
    ;

create_table_stmt: CREATE TABLE IDENTIFIER LPAREN attribute_decl_list COMMA PRIMARY KEY LPAREN IDENTIFIER RPAREN RPAREN {
        API::createTable($3, $10, $5);
    }
    ;

attribute_decl_list: attribute_decl_list COMMA attribute_decl {
        $$ = $1;
        $$.push_back($3);
    }
    | attribute_decl {
        $$ = std::vector<AttrPtr>();
        $$.push_back($1);
    }
    ;

attribute_decl: IDENTIFIER attribute_type {
        $$ = std::make_shared<Attribute>($1, 0, $2.clen, $2.type);
    }
    | IDENTIFIER attribute_type UNIQUE {
        $$ = std::make_shared<Attribute>($1, 1, $2.clen, $2.type);
    }
    ;

attribute_type: INT {
        $$.type = Type::INT;
    }
    | FLOAT {
        $$.type = Type::FLOAT;
    }
    | CHAR LPAREN INTEGER RPAREN {
        $$.type = Type::CHAR;
        $$.clen = $3;
    }
    ;

drop_table_stmt: DROP TABLE IDENTIFIER {
        API::dropTable($3);
    }
    ;

create_index_stmt: CREATE INDEX IDENTIFIER ON IDENTIFIER LPAREN IDENTIFIER RPAREN
    ;

drop_index_stmt: DROP INDEX IDENTIFIER
    ;

select_stmt: SELECT attribute_list FROM IDENTIFIER {
        API::displaymsg($4, API::selecter($4, $2), $2);
    }
    | SELECT attribute_list FROM IDENTIFIER WHERE constraint_list {
        API::displaymsg($4, API::selecter($4, $2, $6), $2);
    }
    | SELECT STAR FROM IDENTIFIER {
        API::displaymsg($4, API::selecter($4));
    }
    | SELECT STAR FROM IDENTIFIER WHERE constraint_list {
        API::displaymsg($4, API::selecter($4, std::vector<std::string>(), $6));
    }
    ;

attribute_list: attribute_list COMMA IDENTIFIER {
        $$ = $1;
        $$.push_back($3);
    }
    | IDENTIFIER {
        $$ = std::vector<std::string>();
        $$.push_back($1);
    }
    ;

constraint_list: constraint_list AND constraint {
        $$ = $1;
        $$.push_back($3);
    }
    | constraint {
        $$ = std::vector<RawConstraint>();
        $$.push_back($1);
    }
    ;

constraint: IDENTIFIER operation value {
        $$.name = $1;
        $$.op = $2;
        $$.val = $3;
    }
    ;

operation: EQ {
        $$ = Operator::EQ;
    }
    | NE {
        $$ = Operator::NEQ;
    }
    | GE {
        $$ = Operator::GEQ;
    }
    | LE {
        $$ = Operator::LEQ;
    }
    | GT {
        $$ = Operator::GT;
    }
    | LT {
        $$ = Operator::LT;
    }
    ;

value: INTEGER {
        $$.type = Type::INT;
        $$.ptr = new int($1);
    }
    | REALNUMBER {
        $$.type = Type::FLOAT;
        $$.ptr = new float($1);
    }
    | STRING {
        $$.type = Type::CHAR;
        $$.clen = $1.size();
        $$.ptr = new char[$1.size() + 1];
        memcpy($$.ptr, $1.c_str(), $$.clen + 1);
    }
    ;

insert_stmt: INSERT INTO IDENTIFIER VALUES LPAREN value_list RPAREN {
        API::inserter($3, $6);
    }
    ;

value_list: value_list COMMA value {
        $$ = $1;
        $$.push_back($3);
    }
    | value {
        $$ = std::vector<Value>();
        $$.push_back($1);
    }
    ;

delete_stmt: DELETE FROM IDENTIFIER {
        API::deleter($3);
    }
    | DELETE FROM IDENTIFIER WHERE constraint_list {
        API::deleter($3, $5);
    }
    ;

quit_stmt: QUIT
    ;

execfile_stmt: EXECFILE STRING
    ;

%%

void Parser::error(const location_type &loc, const std::string& msg) {
    std::cerr << loc << ": " << msg << std::endl;
    throw std::logic_error("Syntax error: invalid syntax");
}