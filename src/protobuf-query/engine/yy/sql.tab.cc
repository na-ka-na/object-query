// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.

#line 37 "sql.tab.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "sql.tab.hh"

// User implementation prologue.

#line 51 "sql.tab.cc" // lalr1.cc:412
// Unqualified %code blocks.
#line 27 "../sql.yy" // lalr1.cc:413

#include "../select_query.h"

#line 57 "sql.tab.cc" // lalr1.cc:413


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
#line 143 "sql.tab.cc" // lalr1.cc:479

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  SqlParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  SqlParser::SqlParser (SelectQuery& query_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      query (query_yyarg)
  {}

  SqlParser::~SqlParser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  inline
  SqlParser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  SqlParser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  SqlParser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  SqlParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  SqlParser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  SqlParser::symbol_number_type
  SqlParser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  SqlParser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  SqlParser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
      switch (that.type_get ())
    {
      case 55: // boolean_expr
        value.move< BooleanExpr > (that.value);
        break;

      case 56: // expr
        value.move< Expr > (that.value);
        break;

      case 39: // "STR"
      case 40: // "INT"
      case 41: // "SUM"
      case 42: // "COUNT"
      case 47: // fn1
        value.move< Fn1 > (that.value);
        break;

      case 43: // "SUBSTR"
      case 48: // fn3
        value.move< Fn3 > (that.value);
        break;

      case 53: // from_stmt
        value.move< FromStmt > (that.value);
        break;

      case 63: // order_by_field
        value.move< OrderByField > (that.value);
        break;

      case 61: // order_by_stmt
        value.move< OrderByStmt > (that.value);
        break;

      case 51: // select_field
        value.move< RawSelectField > (that.value);
        break;

      case 49: // select_stmt
        value.move< SelectStmt > (that.value);
        break;

      case 54: // where_stmt
        value.move< WhereStmt > (that.value);
        break;

      case 38: // "bool"
        value.move< bool > (that.value);
        break;

      case 37: // "double"
        value.move< double > (that.value);
        break;

      case 36: // "long"
        value.move< long > (that.value);
        break;

      case 33: // "identifier"
      case 34: // "star_identifier"
      case 35: // "string"
      case 52: // as_alias
        value.move< std::string > (that.value);
        break;

      case 62: // order_by_fields
        value.move< std::vector<OrderByField> > (that.value);
        break;

      case 50: // select_fields
        value.move< std::vector<RawSelectField> > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  SqlParser::stack_symbol_type&
  SqlParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 55: // boolean_expr
        value.copy< BooleanExpr > (that.value);
        break;

      case 56: // expr
        value.copy< Expr > (that.value);
        break;

      case 39: // "STR"
      case 40: // "INT"
      case 41: // "SUM"
      case 42: // "COUNT"
      case 47: // fn1
        value.copy< Fn1 > (that.value);
        break;

      case 43: // "SUBSTR"
      case 48: // fn3
        value.copy< Fn3 > (that.value);
        break;

      case 53: // from_stmt
        value.copy< FromStmt > (that.value);
        break;

      case 63: // order_by_field
        value.copy< OrderByField > (that.value);
        break;

      case 61: // order_by_stmt
        value.copy< OrderByStmt > (that.value);
        break;

      case 51: // select_field
        value.copy< RawSelectField > (that.value);
        break;

      case 49: // select_stmt
        value.copy< SelectStmt > (that.value);
        break;

      case 54: // where_stmt
        value.copy< WhereStmt > (that.value);
        break;

      case 38: // "bool"
        value.copy< bool > (that.value);
        break;

      case 37: // "double"
        value.copy< double > (that.value);
        break;

      case 36: // "long"
        value.copy< long > (that.value);
        break;

      case 33: // "identifier"
      case 34: // "star_identifier"
      case 35: // "string"
      case 52: // as_alias
        value.copy< std::string > (that.value);
        break;

      case 62: // order_by_fields
        value.copy< std::vector<OrderByField> > (that.value);
        break;

      case 50: // select_fields
        value.copy< std::vector<RawSelectField> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  SqlParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  SqlParser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    switch (yytype)
    {
            case 33: // "identifier"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< std::string > (); }
#line 454 "sql.tab.cc" // lalr1.cc:636
        break;

      case 34: // "star_identifier"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< std::string > (); }
#line 461 "sql.tab.cc" // lalr1.cc:636
        break;

      case 35: // "string"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< std::string > (); }
#line 468 "sql.tab.cc" // lalr1.cc:636
        break;

      case 36: // "long"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< long > (); }
#line 475 "sql.tab.cc" // lalr1.cc:636
        break;

      case 37: // "double"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< double > (); }
#line 482 "sql.tab.cc" // lalr1.cc:636
        break;

      case 38: // "bool"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< bool > (); }
#line 489 "sql.tab.cc" // lalr1.cc:636
        break;

      case 39: // "STR"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn1 > (); }
#line 496 "sql.tab.cc" // lalr1.cc:636
        break;

      case 40: // "INT"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn1 > (); }
#line 503 "sql.tab.cc" // lalr1.cc:636
        break;

      case 41: // "SUM"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn1 > (); }
#line 510 "sql.tab.cc" // lalr1.cc:636
        break;

      case 42: // "COUNT"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn1 > (); }
#line 517 "sql.tab.cc" // lalr1.cc:636
        break;

      case 43: // "SUBSTR"

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn3 > (); }
#line 524 "sql.tab.cc" // lalr1.cc:636
        break;

      case 47: // fn1

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn1 > (); }
#line 531 "sql.tab.cc" // lalr1.cc:636
        break;

      case 48: // fn3

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn3 > (); }
#line 538 "sql.tab.cc" // lalr1.cc:636
        break;

      case 49: // select_stmt

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< SelectStmt > (); }
#line 545 "sql.tab.cc" // lalr1.cc:636
        break;

      case 50: // select_fields

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< std::vector<RawSelectField> > (); }
#line 552 "sql.tab.cc" // lalr1.cc:636
        break;

      case 51: // select_field

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< RawSelectField > (); }
#line 559 "sql.tab.cc" // lalr1.cc:636
        break;

      case 52: // as_alias

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< std::string > (); }
#line 566 "sql.tab.cc" // lalr1.cc:636
        break;

      case 53: // from_stmt

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< FromStmt > (); }
#line 573 "sql.tab.cc" // lalr1.cc:636
        break;

      case 54: // where_stmt

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< WhereStmt > (); }
#line 580 "sql.tab.cc" // lalr1.cc:636
        break;

      case 55: // boolean_expr

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< BooleanExpr > (); }
#line 587 "sql.tab.cc" // lalr1.cc:636
        break;

      case 56: // expr

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Expr > (); }
#line 594 "sql.tab.cc" // lalr1.cc:636
        break;

      case 61: // order_by_stmt

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< OrderByStmt > (); }
#line 601 "sql.tab.cc" // lalr1.cc:636
        break;

      case 62: // order_by_fields

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< std::vector<OrderByField> > (); }
#line 608 "sql.tab.cc" // lalr1.cc:636
        break;

      case 63: // order_by_field

#line 97 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< OrderByField > (); }
#line 615 "sql.tab.cc" // lalr1.cc:636
        break;


      default:
        break;
    }
    yyo << ')';
  }
#endif

  inline
  void
  SqlParser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  SqlParser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  SqlParser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  SqlParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  SqlParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  SqlParser::debug_level_type
  SqlParser::debug_level () const
  {
    return yydebug_;
  }

  void
  SqlParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline SqlParser::state_type
  SqlParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  SqlParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  SqlParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  SqlParser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            symbol_type yylookahead (yylex (query));
            yyla.move (yylookahead);
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
        switch (yyr1_[yyn])
    {
      case 55: // boolean_expr
        yylhs.value.build< BooleanExpr > ();
        break;

      case 56: // expr
        yylhs.value.build< Expr > ();
        break;

      case 39: // "STR"
      case 40: // "INT"
      case 41: // "SUM"
      case 42: // "COUNT"
      case 47: // fn1
        yylhs.value.build< Fn1 > ();
        break;

      case 43: // "SUBSTR"
      case 48: // fn3
        yylhs.value.build< Fn3 > ();
        break;

      case 53: // from_stmt
        yylhs.value.build< FromStmt > ();
        break;

      case 63: // order_by_field
        yylhs.value.build< OrderByField > ();
        break;

      case 61: // order_by_stmt
        yylhs.value.build< OrderByStmt > ();
        break;

      case 51: // select_field
        yylhs.value.build< RawSelectField > ();
        break;

      case 49: // select_stmt
        yylhs.value.build< SelectStmt > ();
        break;

      case 54: // where_stmt
        yylhs.value.build< WhereStmt > ();
        break;

      case 38: // "bool"
        yylhs.value.build< bool > ();
        break;

      case 37: // "double"
        yylhs.value.build< double > ();
        break;

      case 36: // "long"
        yylhs.value.build< long > ();
        break;

      case 33: // "identifier"
      case 34: // "star_identifier"
      case 35: // "string"
      case 52: // as_alias
        yylhs.value.build< std::string > ();
        break;

      case 62: // order_by_fields
        yylhs.value.build< std::vector<OrderByField> > ();
        break;

      case 50: // select_fields
        yylhs.value.build< std::vector<RawSelectField> > ();
        break;

      default:
        break;
    }


      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 106 "../sql.yy" // lalr1.cc:859
    {query.selectStmt = yystack_[5].value.as< SelectStmt > ();
  query.fromStmt = yystack_[4].value.as< FromStmt > ();
  query.whereStmt = yystack_[3].value.as< WhereStmt > ();
  query.orderByStmt = yystack_[0].value.as< OrderByStmt > ();}
#line 910 "sql.tab.cc" // lalr1.cc:859
    break;

  case 3:
#line 112 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Fn1 > ()=yystack_[0].value.as< Fn1 > ();}
#line 916 "sql.tab.cc" // lalr1.cc:859
    break;

  case 4:
#line 113 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Fn1 > ()=yystack_[0].value.as< Fn1 > ();}
#line 922 "sql.tab.cc" // lalr1.cc:859
    break;

  case 5:
#line 114 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Fn1 > ()=yystack_[0].value.as< Fn1 > ();}
#line 928 "sql.tab.cc" // lalr1.cc:859
    break;

  case 6:
#line 115 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Fn1 > ()=yystack_[0].value.as< Fn1 > ();}
#line 934 "sql.tab.cc" // lalr1.cc:859
    break;

  case 7:
#line 117 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Fn3 > ()=yystack_[0].value.as< Fn3 > ();}
#line 940 "sql.tab.cc" // lalr1.cc:859
    break;

  case 8:
#line 119 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< SelectStmt > ()=SelectStmt::create(yystack_[0].value.as< std::vector<RawSelectField> > (),false);}
#line 946 "sql.tab.cc" // lalr1.cc:859
    break;

  case 9:
#line 120 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< SelectStmt > ()=SelectStmt::create(yystack_[0].value.as< std::vector<RawSelectField> > (),true);}
#line 952 "sql.tab.cc" // lalr1.cc:859
    break;

  case 10:
#line 122 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< std::vector<RawSelectField> > ()=vector<RawSelectField>{yystack_[0].value.as< RawSelectField > ()};}
#line 958 "sql.tab.cc" // lalr1.cc:859
    break;

  case 11:
#line 123 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< std::vector<RawSelectField> > ()=yystack_[2].value.as< std::vector<RawSelectField> > (); yylhs.value.as< std::vector<RawSelectField> > ().push_back(yystack_[0].value.as< RawSelectField > ());}
#line 964 "sql.tab.cc" // lalr1.cc:859
    break;

  case 12:
#line 125 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< RawSelectField > ()=RawSelectField::create("*",yystack_[0].value.as< std::string > ());}
#line 970 "sql.tab.cc" // lalr1.cc:859
    break;

  case 13:
#line 126 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< RawSelectField > ()=RawSelectField::create(yystack_[1].value.as< std::string > (),yystack_[0].value.as< std::string > ());}
#line 976 "sql.tab.cc" // lalr1.cc:859
    break;

  case 14:
#line 127 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< RawSelectField > ()=RawSelectField::create(yystack_[1].value.as< Expr > (),yystack_[0].value.as< std::string > ());}
#line 982 "sql.tab.cc" // lalr1.cc:859
    break;

  case 15:
#line 129 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< std::string > ()="";}
#line 988 "sql.tab.cc" // lalr1.cc:859
    break;

  case 16:
#line 130 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< std::string > ()=yystack_[0].value.as< std::string > ();}
#line 994 "sql.tab.cc" // lalr1.cc:859
    break;

  case 17:
#line 133 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< FromStmt > ()=FromStmt::create(yystack_[0].value.as< std::string > ());}
#line 1000 "sql.tab.cc" // lalr1.cc:859
    break;

  case 18:
#line 135 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< WhereStmt > ()=WhereStmt::create();}
#line 1006 "sql.tab.cc" // lalr1.cc:859
    break;

  case 19:
#line 136 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< WhereStmt > ()=WhereStmt::create(yystack_[0].value.as< BooleanExpr > ());}
#line 1012 "sql.tab.cc" // lalr1.cc:859
    break;

  case 20:
#line 142 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=yystack_[1].value.as< BooleanExpr > ();}
#line 1018 "sql.tab.cc" // lalr1.cc:859
    break;

  case 21:
#line 143 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(AND,yystack_[2].value.as< BooleanExpr > (),yystack_[0].value.as< BooleanExpr > ());}
#line 1024 "sql.tab.cc" // lalr1.cc:859
    break;

  case 22:
#line 144 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(OR,yystack_[2].value.as< BooleanExpr > (),yystack_[0].value.as< BooleanExpr > ());}
#line 1030 "sql.tab.cc" // lalr1.cc:859
    break;

  case 23:
#line 145 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(EQ,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1036 "sql.tab.cc" // lalr1.cc:859
    break;

  case 24:
#line 146 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(NE,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1042 "sql.tab.cc" // lalr1.cc:859
    break;

  case 25:
#line 147 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(GT,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1048 "sql.tab.cc" // lalr1.cc:859
    break;

  case 26:
#line 148 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(LT,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1054 "sql.tab.cc" // lalr1.cc:859
    break;

  case 27:
#line 149 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(GE,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1060 "sql.tab.cc" // lalr1.cc:859
    break;

  case 28:
#line 150 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(LT,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1066 "sql.tab.cc" // lalr1.cc:859
    break;

  case 29:
#line 151 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(LIKE,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1072 "sql.tab.cc" // lalr1.cc:859
    break;

  case 30:
#line 152 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::createNullary(true, yystack_[2].value.as< std::string > ());}
#line 1078 "sql.tab.cc" // lalr1.cc:859
    break;

  case 31:
#line 153 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::createNullary(false, yystack_[3].value.as< std::string > ());}
#line 1084 "sql.tab.cc" // lalr1.cc:859
    break;

  case 32:
#line 160 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=yystack_[1].value.as< Expr > ();}
#line 1090 "sql.tab.cc" // lalr1.cc:859
    break;

  case 33:
#line 161 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(PLUS,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1096 "sql.tab.cc" // lalr1.cc:859
    break;

  case 34:
#line 162 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(MINUS,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1102 "sql.tab.cc" // lalr1.cc:859
    break;

  case 35:
#line 163 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(MULT,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1108 "sql.tab.cc" // lalr1.cc:859
    break;

  case 36:
#line 164 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(DIVIDE,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1114 "sql.tab.cc" // lalr1.cc:859
    break;

  case 37:
#line 165 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(UMINUS,yystack_[0].value.as< Expr > ());}
#line 1120 "sql.tab.cc" // lalr1.cc:859
    break;

  case 38:
#line 166 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(yystack_[3].value.as< Fn1 > (),yystack_[1].value.as< Expr > ());}
#line 1126 "sql.tab.cc" // lalr1.cc:859
    break;

  case 39:
#line 167 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(yystack_[7].value.as< Fn3 > (),yystack_[5].value.as< Expr > (),yystack_[3].value.as< Expr > (),yystack_[1].value.as< Expr > ());}
#line 1132 "sql.tab.cc" // lalr1.cc:859
    break;

  case 40:
#line 168 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::createIdentifier(yystack_[0].value.as< std::string > ());}
#line 1138 "sql.tab.cc" // lalr1.cc:859
    break;

  case 41:
#line 169 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::createPrimitive(yystack_[0].value.as< std::string > ());}
#line 1144 "sql.tab.cc" // lalr1.cc:859
    break;

  case 42:
#line 170 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::createPrimitive(yystack_[0].value.as< long > ());}
#line 1150 "sql.tab.cc" // lalr1.cc:859
    break;

  case 43:
#line 171 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::createPrimitive(yystack_[0].value.as< double > ());}
#line 1156 "sql.tab.cc" // lalr1.cc:859
    break;

  case 44:
#line 172 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::createPrimitive(yystack_[0].value.as< bool > ());}
#line 1162 "sql.tab.cc" // lalr1.cc:859
    break;

  case 52:
#line 181 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< OrderByStmt > ()=OrderByStmt::create();}
#line 1168 "sql.tab.cc" // lalr1.cc:859
    break;

  case 53:
#line 182 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< OrderByStmt > ()=OrderByStmt::create(yystack_[0].value.as< std::vector<OrderByField> > ());}
#line 1174 "sql.tab.cc" // lalr1.cc:859
    break;

  case 54:
#line 184 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< std::vector<OrderByField> > ()=vector<OrderByField>{yystack_[0].value.as< OrderByField > ()};}
#line 1180 "sql.tab.cc" // lalr1.cc:859
    break;

  case 55:
#line 185 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< std::vector<OrderByField> > ()=yystack_[2].value.as< std::vector<OrderByField> > (); yylhs.value.as< std::vector<OrderByField> > ().push_back(yystack_[0].value.as< OrderByField > ());}
#line 1186 "sql.tab.cc" // lalr1.cc:859
    break;

  case 56:
#line 187 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< OrderByField > ()=OrderByField::create(yystack_[0].value.as< Expr > (),false);}
#line 1192 "sql.tab.cc" // lalr1.cc:859
    break;

  case 57:
#line 188 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< OrderByField > ()=OrderByField::create(yystack_[1].value.as< Expr > (),false);}
#line 1198 "sql.tab.cc" // lalr1.cc:859
    break;

  case 58:
#line 189 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< OrderByField > ()=OrderByField::create(yystack_[1].value.as< Expr > (),true);}
#line 1204 "sql.tab.cc" // lalr1.cc:859
    break;


#line 1208 "sql.tab.cc" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  SqlParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  SqlParser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char SqlParser::yypact_ninf_ = -15;

  const signed char SqlParser::yytable_ninf_ = -1;

  const short int
  SqlParser::yypact_[] =
  {
       1,   -12,     8,     9,    75,     7,    75,    44,   -15,     7,
     -15,   -15,   -15,   -15,   -15,   -15,   -15,   -15,   -15,    13,
      25,    45,   -15,    -3,   -15,    22,    59,   -15,    56,   -15,
      34,    45,   -15,    75,    75,    44,    75,    75,    75,    75,
     -15,   -15,    91,    85,   -15,   -15,    61,    87,   -15,     0,
       0,   -15,   -15,    91,    73,    -7,   152,    96,   100,   -15,
      75,    66,   123,   126,    91,    91,    75,    75,    75,    75,
      75,    75,    75,    76,    91,   117,   142,   -15,   133,   -15,
     132,   -15,   108,   108,   108,   108,   108,   108,   108,   -15,
     149,   -15,    -7,   153,   -15,    75,   -15,    76,    75,   134,
     -15,    30,   150,   -15,   -15,   -15,   -15,    75,   -15
  };

  const unsigned char
  SqlParser::yydefact_[] =
  {
       0,     0,     0,     0,     0,    15,     0,     0,    40,    15,
      41,    42,    43,    44,     3,     4,     5,     6,     7,     0,
       0,     8,    10,    15,     1,     0,    18,    37,     0,    12,
       0,     9,    13,     0,     0,     0,     0,     0,     0,     0,
      14,    17,     0,    45,    16,    32,     0,     0,    11,    33,
      34,    35,    36,     0,    40,    19,     0,     0,    50,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    52,     0,    20,     0,    30,
      21,    22,    23,    24,    26,    25,    28,    27,    29,    49,
      46,    47,    51,     0,     2,     0,    31,     0,     0,     0,
      48,    56,    53,    54,    39,    57,    58,     0,    55
  };

  const short int
  SqlParser::yypgoto_[] =
  {
     -15,   -15,   -15,   -15,   -15,   168,   141,    31,   -15,   -15,
     -14,    -1,   -15,   -15,    80,   -15,   -15,   -15,    71
  };

  const signed char
  SqlParser::yydefgoto_[] =
  {
      -1,     2,    19,    20,     3,    21,    22,    29,    26,    43,
      55,    56,    58,    90,    91,    75,    94,   102,   103
  };

  const unsigned char
  SqlParser::yytable_[] =
  {
      23,     4,     5,    27,     1,    30,    23,    28,    24,    36,
      37,    38,    39,    25,    38,    39,     6,    28,     7,    64,
      65,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    46,    47,    23,    49,    50,    51,    52,    61,
      32,    33,    36,    37,    38,    39,    36,    37,    38,    39,
      80,    81,    62,    34,    40,    41,    35,     4,     5,    76,
      92,   105,   106,    45,    42,    82,    83,    84,    85,    86,
      87,    88,     6,    36,    37,    38,    39,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,     4,    44,
      59,    57,    64,    65,    99,    77,    63,   101,    60,    36,
      37,    38,    39,     6,     4,    73,   101,    74,     8,    89,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    53,
      36,    37,    38,    39,    54,    93,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    36,    37,    38,    39,    66,
      67,    68,    69,    70,    71,    72,    36,    37,    38,    39,
      78,    79,    45,    95,    36,    37,    38,    39,    96,    65,
      97,   107,    98,   104,    36,    37,    38,    39,    66,    67,
      68,    69,    70,    71,    72,    31,    48,   100,   108
  };

  const unsigned char
  SqlParser::yycheck_[] =
  {
       1,    13,    14,     4,     3,     6,     7,    10,     0,    12,
      13,    14,    15,     4,    14,    15,    28,    10,    30,    26,
      27,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    33,    34,    35,    36,    37,    38,    39,    53,
       9,    28,    12,    13,    14,    15,    12,    13,    14,    15,
      64,    65,    53,    28,    23,    33,    11,    13,    14,    60,
      74,    31,    32,    29,     5,    66,    67,    68,    69,    70,
      71,    72,    28,    12,    13,    14,    15,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    13,    33,
      29,     6,    26,    27,    95,    29,    23,    98,    11,    12,
      13,    14,    15,    28,    13,     9,   107,     7,    33,    33,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    28,
      12,    13,    14,    15,    33,     8,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    12,    13,    14,    15,
      24,    25,    29,    11,    12,    13,    14,    15,    25,    27,
      11,    11,     9,    29,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,     7,    35,    97,   107
  };

  const unsigned char
  SqlParser::yystos_[] =
  {
       0,     3,    46,    49,    13,    14,    28,    30,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    47,
      48,    50,    51,    56,     0,     4,    53,    56,    10,    52,
      56,    50,    52,    28,    28,    11,    12,    13,    14,    15,
      52,    33,     5,    54,    33,    29,    56,    56,    51,    56,
      56,    56,    56,    28,    33,    55,    56,     6,    57,    29,
      11,    55,    56,    23,    26,    27,    16,    17,    18,    19,
      20,    21,    22,     9,     7,    60,    56,    29,    24,    25,
      55,    55,    56,    56,    56,    56,    56,    56,    56,    33,
      58,    59,    55,     8,    61,    11,    25,    11,     9,    56,
      59,    56,    62,    63,    29,    31,    32,    11,    63
  };

  const unsigned char
  SqlParser::yyr1_[] =
  {
       0,    45,    46,    47,    47,    47,    47,    48,    49,    49,
      50,    50,    51,    51,    51,    52,    52,    53,    54,    54,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    57,    57,    58,    58,    59,
      60,    60,    61,    61,    62,    62,    63,    63,    63
  };

  const unsigned char
  SqlParser::yyr2_[] =
  {
       0,     2,     6,     1,     1,     1,     1,     1,     2,     3,
       1,     3,     2,     2,     2,     0,     2,     2,     0,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     4,     3,     3,     3,     3,     3,     2,     4,     8,
       1,     1,     1,     1,     1,     0,     3,     1,     3,     1,
       0,     2,     0,     3,     1,     3,     1,     2,     2
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const SqlParser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "\"SELECT\"", "\"FROM\"",
  "\"WHERE\"", "\"GROUP\"", "\"HAVING\"", "\"ORDER\"", "\"BY\"", "\"AS\"",
  "\",\"", "\"+\"", "\"-\"", "\"*\"", "\"/\"", "\"=\"", "\"!=\"", "\"<\"",
  "\">\"", "\"<=\"", "\">=\"", "\"LIKE\"", "\"IS\"", "\"NOT\"", "\"NULL\"",
  "\"AND\"", "\"OR\"", "\"(\"", "\")\"", "\"DISTINCT\"", "\"ASC\"",
  "\"DESC\"", "\"identifier\"", "\"star_identifier\"", "\"string\"",
  "\"long\"", "\"double\"", "\"bool\"", "\"STR\"", "\"INT\"", "\"SUM\"",
  "\"COUNT\"", "\"SUBSTR\"", "UMINUS", "$accept", "query", "fn1", "fn3",
  "select_stmt", "select_fields", "select_field", "as_alias", "from_stmt",
  "where_stmt", "boolean_expr", "expr", "group_by_stmt", "group_by_fields",
  "group_by_field", "having_stmt", "order_by_stmt", "order_by_fields",
  "order_by_field", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  SqlParser::yyrline_[] =
  {
       0,   100,   100,   112,   113,   114,   115,   117,   119,   120,
     122,   123,   125,   126,   127,   129,   130,   133,   135,   136,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   175,   175,   176,   176,   177,
     179,   179,   181,   182,   184,   185,   187,   188,   189
  };

  // Print the state stack on the debug stream.
  void
  SqlParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  SqlParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG



} // yy
#line 1662 "sql.tab.cc" // lalr1.cc:1167
#line 192 "../sql.yy" // lalr1.cc:1168

void yy::SqlParser::error(const yy::SqlParser::location_type& loc,
                          const std::string& msg) {
  query.mark_parse_error(loc, msg);
}
