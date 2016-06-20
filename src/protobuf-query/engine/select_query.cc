
#include "select_query.h"

using namespace std;

yy::SqlParser::symbol_type yylex(SelectQuery& query) {
  return yylex_flex(query, query.yyscanner);
}

bool SelectQuery::parse() {
  if (yylex_init(&yyscanner) != 0) {
    cerr << "yylex_init failed with errno=" << errno << endl;
    return false;
  }
  YY_BUFFER_STATE buffer = yy_scan_string(rawSql.c_str(), yyscanner);

//  yy::SqlParser::token_type token;
//  do {
//    yy::SqlParser::symbol_type symbol = yylex_flex(*this, yyscanner);
//    token = symbol.token();
//    cout << "TOKEN: " << token << " location: " << symbol.location << endl;
//  } while (token != yy::SqlParser::token_type::TOK_END);

  yy::SqlParser parser(*this);
  int res = parser.parse();

  yy_delete_buffer(buffer, yyscanner);
  if (yylex_destroy(yyscanner) != 0) {
    cerr << "yylex_destroy failed with errno=" << errno << endl;
    return false;
  }
  return res==0;
}

void SelectQuery::mark_lexer_invalid_char(char c) {
  cerr << "parse error at " << loc << ": ignoring invalid char '"
       << c << "'" << endl;
}

void SelectQuery::mark_parse_error(
    const yy::SqlParser::location_type& loc,
    const std::string& msg) {
  cerr << "parse error at " << loc << ": " << msg << endl;
}
