/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <string>
#include <vector>
#include "select_parts.h"
#include "yy/sql.tab.hh"

using namespace std;

// declare yylex for bison
yy::SqlParser::symbol_type yylex(SelectQuery& query);

// declare yylex for flex
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
#define YY_DECL yy::SqlParser::symbol_type yylex_flex(SelectQuery& query, yyscan_t yyscanner)
YY_DECL;

// Fwd declaration of flex functions, generated header file doesn't work for us
// for some reason
int yylex_init(yyscan_t*);
struct yy_buffer_state;
#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state* YY_BUFFER_STATE;
#endif
YY_BUFFER_STATE yy_scan_string(const char*, yyscan_t);
void yy_delete_buffer(YY_BUFFER_STATE, yyscan_t);
int yylex_destroy(yyscan_t);

class SelectQuery {
public:
  SelectQuery(const string& rawSql) : rawSql(rawSql) {}
  string rawSql;
  yy::location loc;
  yyscan_t yyscanner = nullptr;

  SelectStmt selectStmt;
  FromStmt fromStmt;
  WhereStmt whereStmt;
  OrderByStmt orderByStmt;

  bool parse();
  void mark_lexer_invalid_char(char c);
  void mark_parse_error(
      const yy::SqlParser::location_type& loc,
      const string& msg);
  void removeSelectAliases();
  void preProcess();
  string str() const;
};

