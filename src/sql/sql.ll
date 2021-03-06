/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

%{
#include <cstdlib>
#include <iostream>
#include "../select_query.h"
#include "sql.tab.hh"
%}

%option reentrant noyywrap nounput batch debug noinput

string  '[^\n']*'
ws      [ \t]
alpha   [A-Za-z]
dig     [0-9]
name    ({alpha}|[_$:])({alpha}|{dig}|[_$:])*
iden    {name}(\.{name})*
siden   {iden}\.\*
num1    [-+]?{dig}+\.?([eE][-+]?{dig}+)?
num2    [-+]?{dig}*\.{dig}+([eE][-+]?{dig}+)?
number  {num1}|{num2}

%{
// Code run each time a pattern is matched.
#define YY_USER_ACTION  query.loc.columns(yyleng);
%}

%%

%{
// Code run each time yylex is called.
query.loc.step();
%}

{ws}+       query.loc.step();
[\n]+       query.loc.lines(yyleng); query.loc.step();
"SELECT"    return oq::yy::SqlParser::make_SELECT(query.loc);
"FROM"      return oq::yy::SqlParser::make_FROM(query.loc);
"WHERE"     return oq::yy::SqlParser::make_WHERE(query.loc);
"GROUP"     return oq::yy::SqlParser::make_GROUP(query.loc);
"HAVING"    return oq::yy::SqlParser::make_HAVING(query.loc);
"ORDER"     return oq::yy::SqlParser::make_ORDER(query.loc);
"BY"        return oq::yy::SqlParser::make_BY(query.loc);
"AS"        return oq::yy::SqlParser::make_AS(query.loc);
","         return oq::yy::SqlParser::make_COMMA(query.loc);
"+"         return oq::yy::SqlParser::make_PLUS(query.loc);
"-"         return oq::yy::SqlParser::make_MINUS(query.loc);
"*"         return oq::yy::SqlParser::make_STAR(query.loc);
"/"         return oq::yy::SqlParser::make_DIVIDE(query.loc);
"="         return oq::yy::SqlParser::make_EQ(query.loc);
"!="        return oq::yy::SqlParser::make_NE(query.loc);
"<"         return oq::yy::SqlParser::make_LT(query.loc);
">"         return oq::yy::SqlParser::make_GT(query.loc);
"<="        return oq::yy::SqlParser::make_LE(query.loc);
">="        return oq::yy::SqlParser::make_GE(query.loc);
"LIKE"      return oq::yy::SqlParser::make_LIKE(query.loc);
"IS"        return oq::yy::SqlParser::make_IS(query.loc);
"NOT"       return oq::yy::SqlParser::make_NOT(query.loc);
"NULL"      return oq::yy::SqlParser::make_NULL(query.loc);
"AND"       return oq::yy::SqlParser::make_AND(query.loc);
"OR"        return oq::yy::SqlParser::make_OR(query.loc);
"("         return oq::yy::SqlParser::make_LPAREN(query.loc);
")"         return oq::yy::SqlParser::make_RPAREN(query.loc);
"DISTINCT"  return oq::yy::SqlParser::make_DISTINCT(query.loc);
"ASC"       return oq::yy::SqlParser::make_ASC(query.loc);
"DESC"      return oq::yy::SqlParser::make_DESC(query.loc);
"TRUE"      return oq::yy::SqlParser::make_BOOL(true, query.loc);
"FALSE"     return oq::yy::SqlParser::make_BOOL(false, query.loc);
"true"      return oq::yy::SqlParser::make_BOOL(true, query.loc);
"false"     return oq::yy::SqlParser::make_BOOL(false, query.loc);

{string}    {string s = yytext; s = s.substr(1, s.size()-2);
             return oq::yy::SqlParser::make_STRING(s, query.loc);}

{dig}+      {long n = std::strtol(yytext, NULL, 10);
             return oq::yy::SqlParser::make_LONG(n, query.loc);}

{number}    {double d = std::strtod(yytext, NULL);
             return oq::yy::SqlParser::make_DOUBLE(d, query.loc);}

{iden}      return oq::yy::SqlParser::make_IDENTIFIER(yytext, query.loc);
{siden}     return oq::yy::SqlParser::make_STAR_IDENTIFIER(yytext, query.loc);

.           query.mark_lexer_invalid_char(yytext[0]);
<<EOF>>     return oq::yy::SqlParser::make_END(query.loc);
%%

