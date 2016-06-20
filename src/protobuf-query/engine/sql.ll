%{
#include <cstdlib>
#include <iostream>
#include "../select_query.h"
#include "sql.tab.hh"
%}

%option reentrant noyywrap nounput batch debug noinput

string  '[^\n']+'
ws      [ \t]
alpha   [A-Za-z]
dig     [0-9]
name    ({alpha}|{dig}|\$)({alpha}|{dig}|[_.\-/$])*
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
"SELECT"    return yy::SqlParser::make_SELECT(query.loc);
"FROM"      return yy::SqlParser::make_FROM(query.loc);
"WHERE"     return yy::SqlParser::make_WHERE(query.loc);
"GROUP"     return yy::SqlParser::make_GROUP(query.loc);
"HAVING"    return yy::SqlParser::make_HAVING(query.loc);
"ORDER"     return yy::SqlParser::make_ORDER(query.loc);
"BY"        return yy::SqlParser::make_BY(query.loc);
","         return yy::SqlParser::make_COMMA(query.loc);
"+"         return yy::SqlParser::make_PLUS(query.loc);
"-"         return yy::SqlParser::make_MINUS(query.loc);
"*"         return yy::SqlParser::make_MULT(query.loc);
"/"         return yy::SqlParser::make_DIVIDE(query.loc);
"="         return yy::SqlParser::make_EQUALS(query.loc);
"<"         return yy::SqlParser::make_LT(query.loc);
">"         return yy::SqlParser::make_GT(query.loc);
"<="        return yy::SqlParser::make_LE(query.loc);
">="        return yy::SqlParser::make_GE(query.loc);
"LIKE"      return yy::SqlParser::make_LIKE(query.loc);
"AND"       return yy::SqlParser::make_AND(query.loc);
"OR"        return yy::SqlParser::make_OR(query.loc);
"("         return yy::SqlParser::make_LPAREN(query.loc);
")"         return yy::SqlParser::make_RPAREN(query.loc);
"SUM"       return yy::SqlParser::make_SUM(query.loc);
"COUNT"     return yy::SqlParser::make_COUNT(query.loc);
"DISTINCT"  return yy::SqlParser::make_DISTINCT(query.loc);

{string}    return yy::SqlParser::make_STRING(yytext, query.loc);

{dig}+      {long n = std::strtol(yytext, NULL, 10);
             return yy::SqlParser::make_LONG(n, query.loc);}

{number}    {double d = std::strtod(yytext, NULL);
             return yy::SqlParser::make_DOUBLE(d, query.loc);}

{name}      return yy::SqlParser::make_IDENTIFIER(yytext, query.loc);

.           std::cerr << "Ignoring invalid character at " << query.loc << std::endl;
<<EOF>>     return yy::SqlParser::make_END(query.loc);
%%

