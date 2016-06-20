
#include "select_query.h"

using namespace std;

yy::SqlParser::symbol_type yylex(SelectQuery& query) {
  return yylex_flex(query, query.yyscanner);
}

void SelectQuery::parse() {
  if (yylex_init(&yyscanner) != 0) {
    throw runtime_error("yylex_init failed with errno=" + to_string(errno));
  }
  YY_BUFFER_STATE buffer = yy_scan_string(rawSql.c_str(), yyscanner);

  yy::SqlParser::token_type token;
  do {
    yy::SqlParser::symbol_type symbol = yylex_flex(*this, yyscanner);
    token = symbol.token();
    cout << "TOKEN: " << token << " location: " << symbol.location << endl;
  } while (token != yy::SqlParser::token_type::TOK_END);

  yy_delete_buffer(buffer, yyscanner);
  if (yylex_destroy(yyscanner) != 0) {
    throw runtime_error("yylex_destroy failed with errno=" + to_string(errno));
  }
}
