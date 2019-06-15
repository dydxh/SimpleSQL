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

"int"      {return token::INT;}
"float"    {return token::FLOAT;}
"and"      {return token::AND;}
"char"     {return token::CHAR;}
"create"   {return token::CREATE;}
"delete"   {return token::DELETE;}
"drop"     {return token::DROP;}
"execfile" {return token::EXECFILE;}
"from"     {return token::FROM;}
"index"    {return token::INDEX;}
"insert"   {return token::INSERT;}
"into"     {return token::INTO;}
"key"      {return token::KEY;}
"on"       {return token::ON;}
"primary"  {return token::PRIMARY;}
"quit"     {return token::QUIT;}
"select"   {return token::SELECT;}
"table"    {return token::TABLE;}
"unique"   {return token::UNIQUE;}
"values"   {return token::VALUES;}
"where"    {return token::WHERE;}

[+|-][0-9]+      {
        yylval->build<int>(std::stoi(yytext));
        return token::INTEGER;
    }
[0-9]+      {
        yylval->build<int>(std::stoi(yytext));
        return token::INTEGER;
    }
[+|-][0-9]+"."[0-9]+   {
        yylval->build<float>(std::stod(yytext));
        return token::REALNUMBER;
    }
[0-9]+"."[0-9]+   {
        yylval->build<float>(std::stod(yytext));
        return token::REALNUMBER;
    }
'({NQUOTE}|'')+'  {
        yytext[yyleng - 1] = 0;
        yylval->build<std::string>(std::string(yytext + 1));
        return token::STRING;
    }
[a-zA-Z]([a-zA-Z0-9])*  {
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

"("         {return token::LPAREN;}
")"         {return token::RPAREN;}
";"         {return token::SEMICOLON;}
","         {return token::COMMA;}
"="         {return token::EQ;}
"<"         {return token::LT;}
"<="        {return token::LE;}
"<>"        {return token::NE;}
">"         {return token::GT;}
">="        {return token::GE;}
"*"         {return token::STAR;}

[ \t\f]    {continue;}

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