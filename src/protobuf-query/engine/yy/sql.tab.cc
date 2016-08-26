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
#line 19 "../sql.yy" // lalr1.cc:413

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
      case 50: // boolean_expr
        value.move< BooleanExpr > (that.value);
        break;

      case 51: // expr
        value.move< Expr > (that.value);
        break;

      case 35: // "STR"
      case 36: // "INT"
      case 37: // "SUM"
      case 38: // "COUNT"
      case 43: // fn1
        value.move< Fn1 > (that.value);
        break;

      case 39: // "SUBSTR"
      case 44: // fn3
        value.move< Fn3 > (that.value);
        break;

      case 48: // from_stmt
        value.move< FromStmt > (that.value);
        break;

      case 58: // order_by_field
        value.move< OrderByField > (that.value);
        break;

      case 56: // order_by_stmt
        value.move< OrderByStmt > (that.value);
        break;

      case 47: // select_field
        value.move< SelectField > (that.value);
        break;

      case 45: // select_stmt
        value.move< SelectStmt > (that.value);
        break;

      case 49: // where_stmt
        value.move< WhereStmt > (that.value);
        break;

      case 34: // "bool"
        value.move< bool > (that.value);
        break;

      case 33: // "double"
        value.move< double > (that.value);
        break;

      case 32: // "long"
        value.move< long > (that.value);
        break;

      case 30: // "identifier"
      case 31: // "string"
        value.move< std::string > (that.value);
        break;

      case 57: // order_by_fields
        value.move< std::vector<OrderByField> > (that.value);
        break;

      case 46: // select_fields
        value.move< std::vector<SelectField> > (that.value);
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
      case 50: // boolean_expr
        value.copy< BooleanExpr > (that.value);
        break;

      case 51: // expr
        value.copy< Expr > (that.value);
        break;

      case 35: // "STR"
      case 36: // "INT"
      case 37: // "SUM"
      case 38: // "COUNT"
      case 43: // fn1
        value.copy< Fn1 > (that.value);
        break;

      case 39: // "SUBSTR"
      case 44: // fn3
        value.copy< Fn3 > (that.value);
        break;

      case 48: // from_stmt
        value.copy< FromStmt > (that.value);
        break;

      case 58: // order_by_field
        value.copy< OrderByField > (that.value);
        break;

      case 56: // order_by_stmt
        value.copy< OrderByStmt > (that.value);
        break;

      case 47: // select_field
        value.copy< SelectField > (that.value);
        break;

      case 45: // select_stmt
        value.copy< SelectStmt > (that.value);
        break;

      case 49: // where_stmt
        value.copy< WhereStmt > (that.value);
        break;

      case 34: // "bool"
        value.copy< bool > (that.value);
        break;

      case 33: // "double"
        value.copy< double > (that.value);
        break;

      case 32: // "long"
        value.copy< long > (that.value);
        break;

      case 30: // "identifier"
      case 31: // "string"
        value.copy< std::string > (that.value);
        break;

      case 57: // order_by_fields
        value.copy< std::vector<OrderByField> > (that.value);
        break;

      case 46: // select_fields
        value.copy< std::vector<SelectField> > (that.value);
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
            case 30: // "identifier"

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< std::string > (); }
#line 450 "sql.tab.cc" // lalr1.cc:636
        break;

      case 31: // "string"

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< std::string > (); }
#line 457 "sql.tab.cc" // lalr1.cc:636
        break;

      case 32: // "long"

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< long > (); }
#line 464 "sql.tab.cc" // lalr1.cc:636
        break;

      case 33: // "double"

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< double > (); }
#line 471 "sql.tab.cc" // lalr1.cc:636
        break;

      case 34: // "bool"

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< bool > (); }
#line 478 "sql.tab.cc" // lalr1.cc:636
        break;

      case 35: // "STR"

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn1 > (); }
#line 485 "sql.tab.cc" // lalr1.cc:636
        break;

      case 36: // "INT"

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn1 > (); }
#line 492 "sql.tab.cc" // lalr1.cc:636
        break;

      case 37: // "SUM"

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn1 > (); }
#line 499 "sql.tab.cc" // lalr1.cc:636
        break;

      case 38: // "COUNT"

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn1 > (); }
#line 506 "sql.tab.cc" // lalr1.cc:636
        break;

      case 39: // "SUBSTR"

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn3 > (); }
#line 513 "sql.tab.cc" // lalr1.cc:636
        break;

      case 43: // fn1

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn1 > (); }
#line 520 "sql.tab.cc" // lalr1.cc:636
        break;

      case 44: // fn3

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Fn3 > (); }
#line 527 "sql.tab.cc" // lalr1.cc:636
        break;

      case 45: // select_stmt

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< SelectStmt > (); }
#line 534 "sql.tab.cc" // lalr1.cc:636
        break;

      case 46: // select_fields

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< std::vector<SelectField> > (); }
#line 541 "sql.tab.cc" // lalr1.cc:636
        break;

      case 47: // select_field

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< SelectField > (); }
#line 548 "sql.tab.cc" // lalr1.cc:636
        break;

      case 48: // from_stmt

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< FromStmt > (); }
#line 555 "sql.tab.cc" // lalr1.cc:636
        break;

      case 49: // where_stmt

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< WhereStmt > (); }
#line 562 "sql.tab.cc" // lalr1.cc:636
        break;

      case 50: // boolean_expr

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< BooleanExpr > (); }
#line 569 "sql.tab.cc" // lalr1.cc:636
        break;

      case 51: // expr

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< Expr > (); }
#line 576 "sql.tab.cc" // lalr1.cc:636
        break;

      case 56: // order_by_stmt

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< OrderByStmt > (); }
#line 583 "sql.tab.cc" // lalr1.cc:636
        break;

      case 57: // order_by_fields

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< std::vector<OrderByField> > (); }
#line 590 "sql.tab.cc" // lalr1.cc:636
        break;

      case 58: // order_by_field

#line 84 "../sql.yy" // lalr1.cc:636
        { yyoutput << yysym.value.template as< OrderByField > (); }
#line 597 "sql.tab.cc" // lalr1.cc:636
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
      case 50: // boolean_expr
        yylhs.value.build< BooleanExpr > ();
        break;

      case 51: // expr
        yylhs.value.build< Expr > ();
        break;

      case 35: // "STR"
      case 36: // "INT"
      case 37: // "SUM"
      case 38: // "COUNT"
      case 43: // fn1
        yylhs.value.build< Fn1 > ();
        break;

      case 39: // "SUBSTR"
      case 44: // fn3
        yylhs.value.build< Fn3 > ();
        break;

      case 48: // from_stmt
        yylhs.value.build< FromStmt > ();
        break;

      case 58: // order_by_field
        yylhs.value.build< OrderByField > ();
        break;

      case 56: // order_by_stmt
        yylhs.value.build< OrderByStmt > ();
        break;

      case 47: // select_field
        yylhs.value.build< SelectField > ();
        break;

      case 45: // select_stmt
        yylhs.value.build< SelectStmt > ();
        break;

      case 49: // where_stmt
        yylhs.value.build< WhereStmt > ();
        break;

      case 34: // "bool"
        yylhs.value.build< bool > ();
        break;

      case 33: // "double"
        yylhs.value.build< double > ();
        break;

      case 32: // "long"
        yylhs.value.build< long > ();
        break;

      case 30: // "identifier"
      case 31: // "string"
        yylhs.value.build< std::string > ();
        break;

      case 57: // order_by_fields
        yylhs.value.build< std::vector<OrderByField> > ();
        break;

      case 46: // select_fields
        yylhs.value.build< std::vector<SelectField> > ();
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
#line 93 "../sql.yy" // lalr1.cc:859
    {query.selectStmt = yystack_[5].value.as< SelectStmt > ();
  query.fromStmt = yystack_[4].value.as< FromStmt > ();
  query.whereStmt = yystack_[3].value.as< WhereStmt > ();
  query.orderByStmt = yystack_[0].value.as< OrderByStmt > ();}
#line 890 "sql.tab.cc" // lalr1.cc:859
    break;

  case 8:
#line 102 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< SelectStmt > ()=SelectStmt(); yylhs.value.as< SelectStmt > ().selectFields=yystack_[0].value.as< std::vector<SelectField> > ();}
#line 896 "sql.tab.cc" // lalr1.cc:859
    break;

  case 9:
#line 103 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< SelectStmt > ()=SelectStmt(); yylhs.value.as< SelectStmt > ().distinct=true; yylhs.value.as< SelectStmt > ().selectFields=yystack_[0].value.as< std::vector<SelectField> > ();}
#line 902 "sql.tab.cc" // lalr1.cc:859
    break;

  case 10:
#line 105 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< std::vector<SelectField> > ()=vector<SelectField>{yystack_[0].value.as< SelectField > ()};}
#line 908 "sql.tab.cc" // lalr1.cc:859
    break;

  case 11:
#line 106 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< std::vector<SelectField> > ()=yystack_[2].value.as< std::vector<SelectField> > (); yylhs.value.as< std::vector<SelectField> > ().push_back(yystack_[0].value.as< SelectField > ());}
#line 914 "sql.tab.cc" // lalr1.cc:859
    break;

  case 12:
#line 108 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< SelectField > ()=SelectField(); yylhs.value.as< SelectField > ().expr=yystack_[0].value.as< Expr > ();}
#line 920 "sql.tab.cc" // lalr1.cc:859
    break;

  case 13:
#line 111 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< FromStmt > ()=FromStmt(); yylhs.value.as< FromStmt > ().fromFile=yystack_[3].value.as< std::string > (); yylhs.value.as< FromStmt > ().fromRootProto=yystack_[1].value.as< std::string > ();}
#line 926 "sql.tab.cc" // lalr1.cc:859
    break;

  case 14:
#line 114 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< WhereStmt > ()=WhereStmt();}
#line 932 "sql.tab.cc" // lalr1.cc:859
    break;

  case 15:
#line 115 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< WhereStmt > ()=WhereStmt(); yylhs.value.as< WhereStmt > ().booleanExpr=yystack_[0].value.as< BooleanExpr > ();}
#line 938 "sql.tab.cc" // lalr1.cc:859
    break;

  case 16:
#line 120 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=yystack_[1].value.as< BooleanExpr > ();}
#line 944 "sql.tab.cc" // lalr1.cc:859
    break;

  case 17:
#line 121 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(AND,yystack_[2].value.as< BooleanExpr > (),yystack_[0].value.as< BooleanExpr > ());}
#line 950 "sql.tab.cc" // lalr1.cc:859
    break;

  case 18:
#line 122 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(OR,yystack_[2].value.as< BooleanExpr > (),yystack_[0].value.as< BooleanExpr > ());}
#line 956 "sql.tab.cc" // lalr1.cc:859
    break;

  case 19:
#line 123 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(EQ,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 962 "sql.tab.cc" // lalr1.cc:859
    break;

  case 20:
#line 124 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(NE,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 968 "sql.tab.cc" // lalr1.cc:859
    break;

  case 21:
#line 125 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(GT,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 974 "sql.tab.cc" // lalr1.cc:859
    break;

  case 22:
#line 126 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(LT,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 980 "sql.tab.cc" // lalr1.cc:859
    break;

  case 23:
#line 127 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(GE,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 986 "sql.tab.cc" // lalr1.cc:859
    break;

  case 24:
#line 128 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(LT,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 992 "sql.tab.cc" // lalr1.cc:859
    break;

  case 25:
#line 129 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::create(LIKE,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 998 "sql.tab.cc" // lalr1.cc:859
    break;

  case 26:
#line 130 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::createNullary(true, yystack_[2].value.as< std::string > ());}
#line 1004 "sql.tab.cc" // lalr1.cc:859
    break;

  case 27:
#line 131 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< BooleanExpr > ()=BooleanExpr::createNullary(false, yystack_[3].value.as< std::string > ());}
#line 1010 "sql.tab.cc" // lalr1.cc:859
    break;

  case 28:
#line 138 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=yystack_[1].value.as< Expr > ();}
#line 1016 "sql.tab.cc" // lalr1.cc:859
    break;

  case 29:
#line 139 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(PLUS,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1022 "sql.tab.cc" // lalr1.cc:859
    break;

  case 30:
#line 140 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(MINUS,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1028 "sql.tab.cc" // lalr1.cc:859
    break;

  case 31:
#line 141 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(MULT,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1034 "sql.tab.cc" // lalr1.cc:859
    break;

  case 32:
#line 142 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(DIVIDE,yystack_[2].value.as< Expr > (),yystack_[0].value.as< Expr > ());}
#line 1040 "sql.tab.cc" // lalr1.cc:859
    break;

  case 33:
#line 143 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(UMINUS,yystack_[0].value.as< Expr > ());}
#line 1046 "sql.tab.cc" // lalr1.cc:859
    break;

  case 34:
#line 144 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(yystack_[3].value.as< Fn1 > (),yystack_[1].value.as< Expr > ());}
#line 1052 "sql.tab.cc" // lalr1.cc:859
    break;

  case 35:
#line 145 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::create(yystack_[7].value.as< Fn3 > (),yystack_[5].value.as< Expr > (),yystack_[3].value.as< Expr > (),yystack_[1].value.as< Expr > ());}
#line 1058 "sql.tab.cc" // lalr1.cc:859
    break;

  case 36:
#line 146 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::createIdentifier(yystack_[0].value.as< std::string > ());}
#line 1064 "sql.tab.cc" // lalr1.cc:859
    break;

  case 37:
#line 147 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::createPrimitive(yystack_[0].value.as< std::string > ());}
#line 1070 "sql.tab.cc" // lalr1.cc:859
    break;

  case 38:
#line 148 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::createPrimitive(yystack_[0].value.as< long > ());}
#line 1076 "sql.tab.cc" // lalr1.cc:859
    break;

  case 39:
#line 149 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::createPrimitive(yystack_[0].value.as< double > ());}
#line 1082 "sql.tab.cc" // lalr1.cc:859
    break;

  case 40:
#line 150 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< Expr > ()=Expr::createPrimitive(yystack_[0].value.as< bool > ());}
#line 1088 "sql.tab.cc" // lalr1.cc:859
    break;

  case 48:
#line 159 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< OrderByStmt > ()=OrderByStmt();}
#line 1094 "sql.tab.cc" // lalr1.cc:859
    break;

  case 49:
#line 160 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< OrderByStmt > ()=OrderByStmt(); yylhs.value.as< OrderByStmt > ().orderByFields=yystack_[0].value.as< std::vector<OrderByField> > ();}
#line 1100 "sql.tab.cc" // lalr1.cc:859
    break;

  case 50:
#line 162 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< std::vector<OrderByField> > ()=vector<OrderByField>{yystack_[0].value.as< OrderByField > ()};}
#line 1106 "sql.tab.cc" // lalr1.cc:859
    break;

  case 51:
#line 163 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< std::vector<OrderByField> > ()=yystack_[2].value.as< std::vector<OrderByField> > (); yylhs.value.as< std::vector<OrderByField> > ().push_back(yystack_[0].value.as< OrderByField > ());}
#line 1112 "sql.tab.cc" // lalr1.cc:859
    break;

  case 52:
#line 165 "../sql.yy" // lalr1.cc:859
    {yylhs.value.as< OrderByField > ()=OrderByField(); yylhs.value.as< OrderByField > ().expr=yystack_[0].value.as< Expr > ();}
#line 1118 "sql.tab.cc" // lalr1.cc:859
    break;


#line 1122 "sql.tab.cc" // lalr1.cc:859
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


  const signed char SqlParser::yypact_ninf_ = -47;

  const signed char SqlParser::yytable_ninf_ = -1;

  const short int
  SqlParser::yypact_[] =
  {
       3,    40,    22,    41,    72,    72,    72,   -47,   -47,   -47,
     -47,   -47,   -47,   -47,   -47,   -47,   -47,   -25,    32,    58,
     -47,    36,   -47,    59,    77,   -47,    -4,    58,    72,    72,
      72,    72,    72,    72,    72,    54,    85,    81,   -47,   114,
       5,   -47,    -2,    -2,   -47,   -47,    78,    85,    67,    -5,
     136,    82,    86,   -47,    72,    61,    55,    23,     2,    85,
      85,    72,    72,    72,    72,    72,    72,    72,    65,    85,
      88,    44,    70,   -47,    76,   -47,    87,   -47,    36,    36,
      36,    36,    36,    36,    36,   -47,   104,   -47,    -5,   124,
     -47,    72,   -47,   -47,    65,    72,   118,   -47,    36,   125,
     -47,   -47,    72,   -47
  };

  const unsigned char
  SqlParser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     0,     0,    36,    37,    38,
      39,    40,     3,     4,     5,     6,     7,     0,     0,     8,
      10,    12,     1,     0,    14,    33,     0,     9,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    41,    28,     0,
       0,    11,    29,    30,    31,    32,     0,     0,    36,    15,
       0,     0,    46,    34,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,     0,    16,     0,    26,    17,    18,    19,    20,
      22,    21,    24,    23,    25,    45,    42,    43,    47,     0,
       2,     0,    13,    27,     0,     0,     0,    44,    52,    49,
      50,    35,     0,    51
  };

  const short int
  SqlParser::yypgoto_[] =
  {
     -47,   -47,   -47,   -47,   -47,   128,   106,   -47,   -47,   -46,
      -1,   -47,   -47,    43,   -47,   -47,   -47,    37
  };

  const signed char
  SqlParser::yydefgoto_[] =
  {
      -1,     2,    17,    18,     3,    19,    20,    24,    37,    49,
      50,    52,    86,    87,    70,    90,    99,   100
  };

  const unsigned char
  SqlParser::yytable_[] =
  {
      21,    56,    28,    25,    26,    21,     1,    31,    32,    33,
      34,    33,    34,    76,    77,    54,    31,    32,    33,    34,
      59,    60,    22,    88,    38,    74,    75,    39,    40,    21,
      42,    43,    44,    45,    31,    32,    33,    34,    61,    62,
      63,    64,    65,    66,    67,    23,    57,    31,    32,    33,
      34,    38,     4,    71,    91,    31,    32,    33,    34,    29,
      78,    79,    80,    81,    82,    83,    84,     5,    30,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      59,    60,    36,    73,     4,    46,    35,    51,    55,    58,
      96,    68,    72,    69,    98,    85,    89,     4,    92,     5,
      93,    98,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    47,    60,    94,    48,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    31,    32,    33,    34,    31,
      32,    33,    34,    95,    27,   102,    41,    97,     0,   103,
       0,     0,    53,     0,     0,     0,   101,    31,    32,    33,
      34,    61,    62,    63,    64,    65,    66,    67
  };

  const signed char
  SqlParser::yycheck_[] =
  {
       1,    47,    27,     4,     5,     6,     3,    11,    12,    13,
      14,    13,    14,    59,    60,    10,    11,    12,    13,    14,
      25,    26,     0,    69,    28,    23,    24,    28,    29,    30,
      31,    32,    33,    34,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     4,    47,    11,    12,    13,
      14,    28,    12,    54,    10,    11,    12,    13,    14,    27,
      61,    62,    63,    64,    65,    66,    67,    27,    10,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      25,    26,     5,    28,    12,    31,    27,     6,    10,    22,
      91,     9,    31,     7,    95,    30,     8,    12,    28,    27,
      24,   102,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    27,    26,    10,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    11,    12,    13,    14,    11,
      12,    13,    14,     9,     6,    10,    30,    94,    -1,   102,
      -1,    -1,    28,    -1,    -1,    -1,    28,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21
  };

  const unsigned char
  SqlParser::yystos_[] =
  {
       0,     3,    42,    45,    12,    27,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    43,    44,    46,
      47,    51,     0,     4,    48,    51,    51,    46,    27,    27,
      10,    11,    12,    13,    14,    27,     5,    49,    28,    51,
      51,    47,    51,    51,    51,    51,    31,    27,    30,    50,
      51,     6,    52,    28,    10,    10,    50,    51,    22,    25,
      26,    15,    16,    17,    18,    19,    20,    21,     9,     7,
      55,    51,    31,    28,    23,    24,    50,    50,    51,    51,
      51,    51,    51,    51,    51,    30,    53,    54,    50,     8,
      56,    10,    28,    24,    10,     9,    51,    54,    51,    57,
      58,    28,    10,    58
  };

  const unsigned char
  SqlParser::yyr1_[] =
  {
       0,    41,    42,    43,    43,    43,    43,    44,    45,    45,
      46,    46,    47,    48,    49,    49,    50,    50,    50,    50,
      50,    50,    50,    50,    50,    50,    50,    50,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    52,    52,    53,    53,    54,    55,    55,    56,    56,
      57,    57,    58
  };

  const unsigned char
  SqlParser::yyr2_[] =
  {
       0,     2,     6,     1,     1,     1,     1,     1,     2,     3,
       1,     3,     1,     6,     0,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     4,     3,     3,
       3,     3,     3,     2,     4,     8,     1,     1,     1,     1,
       1,     0,     3,     1,     3,     1,     0,     2,     0,     3,
       1,     3,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const SqlParser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "\"SELECT\"", "\"FROM\"",
  "\"WHERE\"", "\"GROUP\"", "\"HAVING\"", "\"ORDER\"", "\"BY\"", "\",\"",
  "\"+\"", "\"-\"", "\"*\"", "\"/\"", "\"=\"", "\"!=\"", "\"<\"", "\">\"",
  "\"<=\"", "\">=\"", "\"LIKE\"", "\"IS\"", "\"NOT\"", "\"NULL\"",
  "\"AND\"", "\"OR\"", "\"(\"", "\")\"", "\"DISTINCT\"", "\"identifier\"",
  "\"string\"", "\"long\"", "\"double\"", "\"bool\"", "\"STR\"", "\"INT\"",
  "\"SUM\"", "\"COUNT\"", "\"SUBSTR\"", "UMINUS", "$accept", "query",
  "fn1", "fn3", "select_stmt", "select_fields", "select_field",
  "from_stmt", "where_stmt", "boolean_expr", "expr", "group_by_stmt",
  "group_by_fields", "group_by_field", "having_stmt", "order_by_stmt",
  "order_by_fields", "order_by_field", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  SqlParser::yyrline_[] =
  {
       0,    87,    87,    99,    99,    99,    99,   100,   102,   103,
     105,   106,   108,   110,   114,   115,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   153,   153,   154,   154,   155,   157,   157,   159,   160,
     162,   163,   165
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
#line 1571 "sql.tab.cc" // lalr1.cc:1167
#line 167 "../sql.yy" // lalr1.cc:1168

void yy::SqlParser::error(const yy::SqlParser::location_type& loc,
                          const std::string& msg) {
  query.mark_parse_error(loc, msg);
}
