/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define api.namespace {oq::yy}
%define parser_class_name {SqlParser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires
{
#include "../select_parts.h"
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
  AS         "AS"
  COMMA      ","
  PLUS       "+"
  MINUS      "-"
  STAR       "*"
  DIVIDE     "/"
  EQ         "="
  NE         "!="
  LT         "<"
  GT         ">"
  LE         "<="
  GE         ">="
  LIKE       "LIKE"
  IS         "IS"
  NOT        "NOT"
  NULL       "NULL"
  AND        "AND"
  OR         "OR"
  LPAREN     "("
  RPAREN     ")"
  DISTINCT   "DISTINCT"
  ASC        "ASC"
  DESC       "DESC"
;

%token <std::string> IDENTIFIER "identifier"
%token <std::string> STAR_IDENTIFIER "star_identifier"
%token <std::string> STRING "string"
%token <long> LONG "long"
%token <double> DOUBLE "double"
%token <bool> BOOL "bool"

%type <SelectStmt> select_stmt
%type <std::vector<RawSelectField>> select_fields
%type <RawSelectField> select_field
%type <std::string> as_alias

%type <OrderByStmt> order_by_stmt
%type <std::vector<OrderByField>> order_by_fields
%type <OrderByField> order_by_field

%type <FromStmt> from_stmt

%type <WhereStmt> where_stmt
%type <BooleanExpr> boolean_expr
%type <Expr> expr
%type <std::vector<Expr>> exprs
%type <std::vector<Expr>> exprs1

%printer { yyoutput << $$; } <*>;
%%

query: select_stmt
       from_stmt
       where_stmt
       group_by_stmt
       having_stmt
       order_by_stmt
 {query.selectStmt = $1;
  query.fromStmt = $2;
  query.whereStmt = $3;
  query.orderByStmt = $6;}
 ;

select_stmt: "SELECT" select_fields  {$$=SelectStmt::create($2,false);}
 | "SELECT" "DISTINCT" select_fields {$$=SelectStmt::create($3,true);}
 ;
select_fields: select_field        {$$=vector<RawSelectField>{$1};}
 | select_fields "," select_field  {$$=$1; $$.push_back($3);}
 ;
select_field: "*"    as_alias {$$=RawSelectField::create("*",$2);}
 | "star_identifier" as_alias {$$=RawSelectField::create($1,$2);}
 | expr              as_alias {$$=RawSelectField::create($1,$2);}
 ;
as_alias: %empty      {$$="";}
 | "AS" "identifier"  {$$=$2;}
 ;

from_stmt: "FROM" "identifier" {$$=FromStmt::create($2);};

where_stmt: %empty      {$$=WhereStmt::create();}
 | "WHERE" boolean_expr {$$=WhereStmt::create($2);}
 ;

%left "AND";
%left "OR";
boolean_expr:
   "(" boolean_expr ")"            {$$=$2;}
 | boolean_expr "AND" boolean_expr {$$=BooleanExpr::create(AND,$1,$3);}
 | boolean_expr "OR" boolean_expr  {$$=BooleanExpr::create(OR,$1,$3);}
 | expr "=" expr                   {$$=BooleanExpr::create(EQ,$1,$3);}
 | expr "!=" expr                  {$$=BooleanExpr::create(NE,$1,$3);}
 | expr ">" expr                   {$$=BooleanExpr::create(GT,$1,$3);}
 | expr "<" expr                   {$$=BooleanExpr::create(LT,$1,$3);}
 | expr ">=" expr                  {$$=BooleanExpr::create(GE,$1,$3);}
 | expr "<=" expr                  {$$=BooleanExpr::create(LT,$1,$3);}
 | expr "LIKE" expr                {$$=BooleanExpr::create(LIKE,$1,$3);}
 | "identifier" "IS" "NULL"        {$$=BooleanExpr::createNullary(true, $1);}
 | "identifier" "IS" "NOT" "NULL"  {$$=BooleanExpr::createNullary(false, $1);}
 ;

exprs: %empty                      {$$=vector<Expr>{};}
 | exprs1                          {$$=$1;}

exprs1: expr                       {$$=vector<Expr>{$1};}
 | exprs1 "," expr                 {$$=$1; $$.push_back($3);}

%left "+" "-";
%left "*" "/";
%precedence UMINUS;
expr:
   "(" expr ")"                       {$$=$2;}
 | expr "+" expr                      {$$=Expr::create(PLUS,$1,$3);}
 | expr "-" expr                      {$$=Expr::create(MINUS,$1,$3);}
 | expr "*" expr                      {$$=Expr::create(MULT,$1,$3);}
 | expr "/" expr                      {$$=Expr::create(DIVIDE,$1,$3);}
 | "-" expr %prec UMINUS              {$$=Expr::create(UMINUS,$2);}
 | "identifier" "(" exprs ")"         {$$=Expr::createFnCall($1,$3);}
 | "identifier"                       {$$=Expr::createIdentifier($1);}
 | "string"                           {$$=Expr::createPrimitive($1);}
 | "long"                             {$$=Expr::createPrimitive($1);}
 | "double"                           {$$=Expr::createPrimitive($1);}
 | "bool"                             {$$=Expr::createPrimitive($1);}
 ;

group_by_stmt: %empty | "GROUP" "BY" group_by_fields;
group_by_fields: group_by_field | group_by_fields "," group_by_field;
group_by_field: "identifier";

having_stmt: %empty | "HAVING" boolean_expr;

order_by_stmt: %empty                  {$$=OrderByStmt::create();}
 | "ORDER" "BY" order_by_fields        {$$=OrderByStmt::create($3);}
 ;
order_by_fields: order_by_field        {$$=vector<OrderByField>{$1};}
 | order_by_fields "," order_by_field  {$$=$1; $$.push_back($3);}
 ;
order_by_field: expr                   {$$=OrderByField::create($1,false);}
 | expr "ASC"                          {$$=OrderByField::create($1,false);}
 | expr "DESC"                         {$$=OrderByField::create($1,true);}
 ;

%%
void oq::yy::SqlParser::error(const oq::yy::SqlParser::location_type& loc,
                              const std::string& msg) {
  query.mark_parse_error(loc, msg);
}
