%{
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <string>
#include <stdexcept>
#include <memory>
#include "Interpreter/Scanner.hpp"


#undef YY_DECL
#define YY_DECL int interpreter::Scanner::yylex(interpreter::Parser::semantic_type* lval, interpreter::Parser::location_type* loc)

using token = interpreter::Parser::token;

#define yyterminate() return token::TERMINATE;

#define YY_NO_UNISTD_H

#define YY_USER_ACTION loc->step(); loc->columns(yyleng);
%}

%option nodefault
%option c++
%option yyclass="interpreter::Scanner"
%option noyywrap
%option caseless
%option stack

NQUOTE [^']

%%
%{
    yylval = lval;
%}

"int"      {std::cout << yytext; return token::INT;}
"float"    {std::cout << yytext; return token::FLOAT;}
"and"      {std::cout << yytext; return token::AND;}
"char"     {std::cout << yytext; return token::CHAR;}
"create"   {std::cout << yytext; return token::CREATE;}
"delete"   {std::cout << yytext; return token::DELETE;}
"drop"     {std::cout << yytext; return token::DROP;}
"execfile" {std::cout << yytext; return token::EXECFILE;}
"from"     {std::cout << yytext; return token::FROM;}
"index"    {std::cout << yytext; return token::INDEX;}
"insert"   {std::cout << yytext; return token::INSERT;}
"into"     {std::cout << yytext; return token::INTO;}
"key"      {std::cout << yytext; return token::KEY;}
"on"       {std::cout << yytext; return token::ON;}
"primary"  {std::cout << yytext; return token::PRIMARY;}
"quit"     {std::cout << yytext; return token::QUIT;}
"select"   {std::cout << yytext; return token::SELECT;}
"table"    {std::cout << yytext; return token::TABLE;}
"unique"   {std::cout << yytext; return token::UNIQUE;}
"values"   {std::cout << yytext; return token::VALUES;}
"where"    {std::cout << yytext; return token::WHERE;}

[+|-][0-9]+      {
        std::cout << "Sign Integer: " << yytext;
        yylval->build<int>(std::stoi(yytext));
        return token::INTEGER;
    }
[0-9]+      {
        std::cout << "Integer: " << yytext;
        yylval->build<int>(std::stoi(yytext));
        return token::INTEGER;
    }
[+|-][0-9]+"."[0-9]+   {
        std::cout << "Sign Float: " << yytext;
        yylval->build<float>(std::stod(yytext));
        return token::REALNUMBER;
    }
[0-9]+"."[0-9]+   {
        std::cout << "Float: " << yytext;
        yylval->build<float>(std::stod(yytext));
        return token::REALNUMBER;
    }
'({NQUOTE}|'')+'  {
        yytext[yyleng - 1] = 0;
        yylval->build<std::string>(std::string(yytext + 1));
        std::cout << "STRING: " << std::string(yytext + 1);
        return token::STRING;
    }
[a-zA-Z]([a-zA-Z0-9])*  {
        std::cout << "[" << yytext << "]";
        yytext[yyleng] = 0;
        yylval->build<std::string>(std::string(yytext));
        return token::IDENTIFIER;
    }

"--" {
    char c;
    while(c = yyinput()) {
        if((c == '\n')) {
            std::cout << std::endl << "[mystery]> ";
            loc->lines();
            break;
        }
        else if(c == EOF) {
            break;
        }
    }
}

"("         {std::cout << yytext; return token::LPAREN;}
")"         {std::cout << yytext; return token::RPAREN;}
";"         {std::cout << yytext << std::endl; return token::SEMICOLON;}
","         {std::cout << yytext; return token::COMMA;}
"="         {std::cout << yytext; return token::EQ;}
"<"         {std::cout << yytext; return token::LT;}
"<="        {std::cout << yytext; return token::LE;}
"<>"        {std::cout << yytext; return token::NE;}
">"         {std::cout << yytext; return token::GT;}
">="        {std::cout << yytext; return token::GE;}
"*"         {std::cout << yytext; return token::STAR;}

[ \t\f]    {std::cout << ' '; continue;}

[\n\r]     {std::cout << std::endl << "[mystery]> "; loc->lines();}

.          {
                std::cerr << "Scanner: Error at " << *loc << ": " << std::endl;
                throw std::logic_error("Scanner: Invalid character\n");
           }

%%

void Scanner::CommentError(interpreter::Parser::location_type* loc) {
    std::cerr << "Unexcepted EOF in comment at line: " << loc->end.line << std::endl;
    throw std::runtime_error("EOF error: Unexcepted EOF in comment");
}