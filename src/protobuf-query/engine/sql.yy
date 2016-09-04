%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
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
  COMMA      ","
  PLUS       "+"
  MINUS      "-"
  MULT       "*"
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
%token <std::string> STRING "string"
%token <long> LONG "long"
%token <double> DOUBLE "double"
%token <bool> BOOL "bool"
%token <Fn1> STR "STR"
%token <Fn1> INT "INT"
%token <Fn1> SUM "SUM"
%token <Fn1> COUNT "COUNT"
%token <Fn3> SUBSTR "SUBSTR"

%type <Fn1> fn1
%type <Fn3> fn3

%type <SelectStmt> select_stmt
%type <std::vector<SelectField>> select_fields
%type <SelectField> select_field

%type <OrderByStmt> order_by_stmt
%type <std::vector<OrderByField>> order_by_fields
%type <OrderByField> order_by_field

%type <FromStmt> from_stmt

%type <WhereStmt> where_stmt
%type <BooleanExpr> boolean_expr
%type <Expr> expr

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

fn1: "STR"  {$$=$1;}
 | "INT"    {$$=$1;}
 | "SUM"    {$$=$1;}
 | "COUNT"  {$$=$1;}
 ;
fn3: "SUBSTR" {$$=$1;};

select_stmt: "SELECT" select_fields  {$$=SelectStmt(); $$.selectFields=$2;}
 | "SELECT" "DISTINCT" select_fields {$$=SelectStmt(); $$.distinct=true; $$.selectFields=$3;}
 ;
select_fields: select_field        {$$=vector<SelectField>{$1};}
 | select_fields "," select_field  {$$=$1; $$.push_back($3);}
 ;
select_field: expr {$$=SelectField(); $$.expr=$1;};

from_stmt: "FROM" "(" "string" "," "string" ")"
 {$$=FromStmt(); $$.fromFile=$3; $$.fromRootProto=$5;}
 ;

where_stmt: %empty      {$$=WhereStmt();}
 | "WHERE" boolean_expr {$$=WhereStmt(); $$.booleanExpr=$2;};

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
 | fn1 "(" expr ")"                   {$$=Expr::create($1,$3);}
 | fn3 "(" expr "," expr "," expr ")" {$$=Expr::create($1,$3,$5,$7);}
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

order_by_stmt: %empty                  {$$=OrderByStmt();}
 | "ORDER" "BY" order_by_fields        {$$=OrderByStmt(); $$.orderByFields=$3;}
 ;
order_by_fields: order_by_field        {$$=vector<OrderByField>{$1};}
 | order_by_fields "," order_by_field  {$$=$1; $$.push_back($3);}
 ;
order_by_field: expr                   {$$=OrderByField(); $$.expr=$1;}
 | expr "ASC"                          {$$=OrderByField(); $$.expr=$1;}
 | expr "DESC"                         {$$=OrderByField(); $$.expr=$1; $$.desc=true;}
 ;

%%
void yy::SqlParser::error(const yy::SqlParser::location_type& loc,
                          const std::string& msg) {
  query.mark_parse_error(loc, msg);
}
