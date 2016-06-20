
#pragma once

#include <string>
#include "yy/sql.tab.hh"

// declare yylex for bison
yy::SqlParser::symbol_type yylex(SelectQuery& query);

// declare yylex for flex
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
#define YY_DECL yy::SqlParser::symbol_type yylex_flex(SelectQuery& query, yyscan_t yyscanner)
YY_DECL;

class SelectQuery {
public:
  std::string rawSql;
  yy::location loc;
  yyscan_t yyscan;
};
