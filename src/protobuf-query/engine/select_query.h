
#pragma once

#include "yy/sql.tab.hh"

// delcare yylex function
yy::SqlParser::symbol_type yylex (SelectQuery& driver);

class SelectQuery {

};
