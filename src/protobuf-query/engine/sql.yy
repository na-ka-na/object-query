%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define parser_class_name {SqlParser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires
{
#include <string>
class SelectQuery;
}
%param { SelectQuery& query }

%locations
%define parse.error verbose

%code
{
#include "../select_query.h"
}

%define api.token.prefix {TOK_}

%token
  END  0  "end of file"
  SELECT     "SELECT"
  FROM       "FROM"
  WHERE      "WHERE"
  GROUP      "GROUP"
  HAVING     "HAVING"
  ORDER      "ORDER"
  BY         "BY"
  COMMA      ","
  PLUS       "+"
  MINUS      "-"
  MULT       "*"
  DIVIDE     "/"
  EQUALS     "="
  LT         "<"
  GT         ">"
  LE         "<="
  GE         ">="
  LIKE       "LIKE"
  AND        "AND"
  OR         "OR"
  LPAREN     "("
  RPAREN     ")"
  SUM        "SUM"
  COUNT      "COUNT"
  DISTINCT   "DISTINCT"
;

%token <std::string> IDENTIFIER "identifier"
%token <std::string> STRING "string"
%token <long> LONG "long"
%token <double> DOUBLE "double"

%printer { yyoutput << $$; } <*>;
%%

query: select_stmt
       from_stmt
       where_stmt
       group_by_stmt
       having_stmt
       order_by_stmt
{};

fn: "SUM" | "COUNT" | "DISTINCT";
fn_identifier: "identifier" | fn "(" fn_identifier ")";
identifiers: fn_identifier | identifiers "," fn_identifier;

select_stmt: "SELECT" identifiers
{};

from_stmt: "FROM" "(" "string" "," "string" ")" {};

where_stmt: %empty | "WHERE" boolean_expr;

%left "AND";
%left "OR";
boolean_expr:
   "(" boolean_expr ")"
 | boolean_expr "AND" boolean_expr
 | boolean_expr "OR" boolean_expr
 | expr "=" expr
 | expr ">" expr
 | expr "<" expr
 | expr ">=" expr
 | expr "<=" expr
 | expr "LIKE" expr
 ;

%left "+" "-";
%left "*" "/";
%precedence UMINUS;
expr:
   "(" expr ")"
 | expr "+" expr
 | expr "-" expr
 | expr "*" expr
 | expr "/" expr
 | "-" expr %prec UMINUS
 | fn_identifier
 | "long"
 | "double"
 | "string"
 ;

group_by_stmt: %empty | "GROUP" "BY" identifiers;

having_stmt: %empty | "HAVING" boolean_expr;

order_by_stmt: %empty | "ORDER" "BY" identifiers;

%%
void yy::SqlParser::error(const yy::SqlParser::location_type& loc,
                          const std::string& msg) {
  query.mark_parse_error(loc, msg);
}
