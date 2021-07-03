/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         ruleparseparse
#define yylex           ruleparselex
#define yyerror         ruleparseerror
#define yylval          ruleparselval
#define yychar          ruleparsechar
#define yydebug         ruleparsedebug
#define yynerrs         ruleparsenerrs

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 16 "ruleparse.y"

#ifdef CPUI_RULECOMPILE
#include "rulecompile.hh"

#define YYERROR_VERBOSE

extern RuleCompile *rulecompile;
extern int ruleparselex(void);
extern int ruleparseerror(const char *str);


/* Line 371 of yacc.c  */
#line 87 "ruleparse.tab.cpp"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "ruleparse.tab.h".  */
#ifndef YY_RULEPARSE_RULEPARSE_TAB_H_INCLUDED
# define YY_RULEPARSE_RULEPARSE_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ruleparsedebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     RIGHT_ARROW = 258,
     LEFT_ARROW = 259,
     DOUBLE_RIGHT_ARROW = 260,
     DOUBLE_LEFT_ARROW = 261,
     OP_BOOL_OR = 262,
     OP_BOOL_XOR = 263,
     OP_BOOL_AND = 264,
     OP_INT_OR = 265,
     OP_INT_XOR = 266,
     OP_INT_AND = 267,
     OP_FLOAT_NOTEQUAL = 268,
     OP_FLOAT_EQUAL = 269,
     OP_INT_NOTEQUAL = 270,
     OP_INT_EQUAL = 271,
     OP_FLOAT_LESSEQUAL = 272,
     OP_FLOAT_LESS = 273,
     OP_INT_SLESSEQUAL = 274,
     OP_INT_SLESS = 275,
     OP_INT_LESSEQUAL = 276,
     OP_INT_LESS = 277,
     OP_INT_SRIGHT = 278,
     OP_INT_RIGHT = 279,
     OP_INT_LEFT = 280,
     OP_FLOAT_SUB = 281,
     OP_FLOAT_ADD = 282,
     OP_INT_SUB = 283,
     OP_INT_ADD = 284,
     OP_INT_REM = 285,
     OP_INT_DIV = 286,
     OP_INT_MULT = 287,
     OP_FLOAT_DIV = 288,
     OP_FLOAT_MULT = 289,
     OP_INT_SREM = 290,
     OP_INT_SDIV = 291,
     OP_INT_NEGATE = 292,
     OP_BOOL_NEGATE = 293,
     OP_INT_ZEXT = 294,
     OP_INT_CARRY = 295,
     OP_INT_BORROW = 296,
     OP_INT_SEXT = 297,
     OP_INT_SCARRY = 298,
     OP_INT_SBORROW = 299,
     OP_FLOAT_NAN = 300,
     OP_FLOAT_ABS = 301,
     OP_FLOAT_SQRT = 302,
     OP_FLOAT_CEIL = 303,
     OP_FLOAT_FLOOR = 304,
     OP_FLOAT_ROUND = 305,
     OP_FLOAT_INT2FLOAT = 306,
     OP_FLOAT_FLOAT2FLOAT = 307,
     OP_FLOAT_TRUNC = 308,
     OP_BRANCH = 309,
     OP_BRANCHIND = 310,
     OP_CALL = 311,
     OP_CALLIND = 312,
     OP_RETURN = 313,
     OP_CBRANCH = 314,
     OP_CALLOTHER = 315,
     OP_LOAD = 316,
     OP_STORE = 317,
     OP_PIECE = 318,
     OP_SUBPIECE = 319,
     OP_COPY = 320,
     BADINTEGER = 321,
     BEFORE_KEYWORD = 322,
     AFTER_KEYWORD = 323,
     REMOVE_KEYWORD = 324,
     SET_KEYWORD = 325,
     ACTION_TICK = 326,
     ISTRUE_KEYWORD = 327,
     ISFALSE_KEYWORD = 328,
     CHAR = 329,
     INTB = 330,
     OP_IDENTIFIER = 331,
     VAR_IDENTIFIER = 332,
     CONST_IDENTIFIER = 333,
     OP_NEW_IDENTIFIER = 334,
     VAR_NEW_IDENTIFIER = 335,
     DOT_IDENTIFIER = 336
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 28 "ruleparse.y"

  char ch;
  string *str;
  int8 *big;
  int4 id;
  OpCode opcode;
  vector<OpCode> *opcodelist;
  ConstraintGroup *group;
  RHSConstant *rhsconst;


/* Line 387 of yacc.c  */
#line 223 "ruleparse.tab.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE ruleparselval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int ruleparseparse (void *YYPARSE_PARAM);
#else
int ruleparseparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int ruleparseparse (void);
#else
int ruleparseparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_RULEPARSE_RULEPARSE_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 251 "ruleparse.tab.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   653

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  91
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  140
/* YYNRULES -- Number of states.  */
#define YYNSTATES  233

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   336

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      87,    88,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    90,    86,
       2,    89,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    84,     2,    85,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    82,     2,    83,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     8,    15,    18,    23,    25,    28,    29,
      32,    35,    38,    41,    43,    47,    50,    53,    56,    60,
      64,    66,    71,    75,    79,    84,    90,    96,    98,   105,
     109,   113,   119,   125,   132,   136,   140,   145,   149,   154,
     159,   166,   173,   180,   187,   189,   195,   202,   209,   216,
     223,   231,   238,   240,   244,   246,   249,   253,   257,   261,
     265,   269,   273,   277,   281,   285,   289,   293,   297,   301,
     305,   309,   313,   314,   317,   319,   322,   324,   326,   328,
     330,   332,   334,   336,   338,   340,   342,   344,   346,   348,
     350,   352,   354,   356,   358,   360,   362,   364,   366,   368,
     370,   372,   374,   376,   378,   380,   382,   384,   386,   388,
     390,   392,   394,   396,   398,   400,   402,   404,   406,   408,
     410,   412,   414,   416,   418,   420,   422,   424,   426,   428,
     430,   432,   434,   436,   438,   440,   443,   445,   447,   449,
     451
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      92,     0,    -1,    82,    95,    94,    83,    -1,    82,    95,
      84,    93,    85,    83,    -1,    95,    94,    -1,    93,    10,
      95,    94,    -1,    71,    -1,    94,    96,    -1,    -1,    95,
      98,    -1,   102,    86,    -1,   103,    86,    -1,   104,    86,
      -1,    95,    -1,    97,    10,    95,    -1,    99,    86,    -1,
     100,    86,    -1,   101,    86,    -1,    84,    97,    85,    -1,
      87,    95,    88,    -1,   110,    -1,    99,    87,   109,    88,
      -1,   100,     4,   110,    -1,   100,     3,   110,    -1,   100,
       3,    38,   110,    -1,    99,    87,    16,   110,    88,    -1,
      99,    87,    15,   110,    88,    -1,   111,    -1,    99,     4,
      87,    75,    88,   111,    -1,    99,     4,   111,    -1,    99,
       3,   111,    -1,   100,    87,    16,   111,    88,    -1,   100,
      87,    15,   111,    88,    -1,    99,     4,    87,    75,    88,
     105,    -1,    99,    89,   110,    -1,   100,    89,   111,    -1,
     100,    89,   105,   106,    -1,   112,    89,   105,    -1,    72,
      87,   105,    88,    -1,    73,    87,   105,    88,    -1,   113,
      87,   108,    67,   110,    88,    -1,   113,    87,   108,    68,
     110,    88,    -1,   113,    87,   108,    67,   113,    88,    -1,
     113,    87,   108,    68,   113,    88,    -1,   110,    -1,   102,
      87,    70,   108,    88,    -1,   102,     5,   114,    87,    75,
      88,    -1,   102,     5,   114,    87,   111,    88,    -1,   102,
       6,    87,   105,    88,   111,    -1,   102,     6,    87,   105,
      88,   114,    -1,   102,     6,    87,   105,    88,   105,   106,
      -1,   102,     6,    87,   105,    88,    69,    -1,   107,    -1,
      87,   105,    88,    -1,   112,    -1,   111,    81,    -1,   105,
      29,   105,    -1,   105,    28,   105,    -1,   105,    12,   105,
      -1,   105,    10,   105,    -1,   105,    11,   105,    -1,   105,
      32,   105,    -1,   105,    31,   105,    -1,   105,    16,   105,
      -1,   105,    15,   105,    -1,   105,    22,   105,    -1,   105,
      21,   105,    -1,   105,    20,   105,    -1,   105,    19,   105,
      -1,   105,    25,   105,    -1,   105,    24,   105,    -1,   105,
      23,   105,    -1,    -1,    90,   105,    -1,    75,    -1,    28,
      75,    -1,     7,    -1,     9,    -1,     8,    -1,    38,    -1,
      37,    -1,    29,    -1,    28,    -1,    12,    -1,    10,    -1,
      11,    -1,    32,    -1,    31,    -1,    30,    -1,    24,    -1,
      25,    -1,    16,    -1,    15,    -1,    22,    -1,    21,    -1,
      36,    -1,    35,    -1,    23,    -1,    20,    -1,    19,    -1,
      39,    -1,    40,    -1,    42,    -1,    43,    -1,    44,    -1,
      27,    -1,    26,    -1,    34,    -1,    33,    -1,    14,    -1,
      13,    -1,    18,    -1,    17,    -1,    45,    -1,    46,    -1,
      47,    -1,    48,    -1,    49,    -1,    50,    -1,    51,    -1,
      52,    -1,    53,    -1,    54,    -1,    55,    -1,    56,    -1,
      57,    -1,    58,    -1,    59,    -1,    60,    -1,    61,    -1,
      62,    -1,    63,    -1,    64,    -1,    65,    -1,   108,    -1,
     109,   108,    -1,    76,    -1,    77,    -1,    78,    -1,    79,
      -1,    80,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    77,    77,    78,    81,    82,    85,    86,    89,    90,
      93,    94,    95,    98,    99,   102,   103,   104,   105,   106,
     109,   110,   111,   112,   113,   114,   115,   118,   119,   120,
     121,   122,   123,   126,   127,   128,   129,   130,   131,   132,
     135,   136,   137,   138,   139,   140,   143,   144,   145,   146,
     149,   150,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   175,   176,   179,   180,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   243,   244,   247,   250,   253,   256,
     259
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "RIGHT_ARROW", "LEFT_ARROW",
  "DOUBLE_RIGHT_ARROW", "DOUBLE_LEFT_ARROW", "OP_BOOL_OR", "OP_BOOL_XOR",
  "OP_BOOL_AND", "OP_INT_OR", "OP_INT_XOR", "OP_INT_AND",
  "OP_FLOAT_NOTEQUAL", "OP_FLOAT_EQUAL", "OP_INT_NOTEQUAL", "OP_INT_EQUAL",
  "OP_FLOAT_LESSEQUAL", "OP_FLOAT_LESS", "OP_INT_SLESSEQUAL",
  "OP_INT_SLESS", "OP_INT_LESSEQUAL", "OP_INT_LESS", "OP_INT_SRIGHT",
  "OP_INT_RIGHT", "OP_INT_LEFT", "OP_FLOAT_SUB", "OP_FLOAT_ADD",
  "OP_INT_SUB", "OP_INT_ADD", "OP_INT_REM", "OP_INT_DIV", "OP_INT_MULT",
  "OP_FLOAT_DIV", "OP_FLOAT_MULT", "OP_INT_SREM", "OP_INT_SDIV",
  "OP_INT_NEGATE", "OP_BOOL_NEGATE", "OP_INT_ZEXT", "OP_INT_CARRY",
  "OP_INT_BORROW", "OP_INT_SEXT", "OP_INT_SCARRY", "OP_INT_SBORROW",
  "OP_FLOAT_NAN", "OP_FLOAT_ABS", "OP_FLOAT_SQRT", "OP_FLOAT_CEIL",
  "OP_FLOAT_FLOOR", "OP_FLOAT_ROUND", "OP_FLOAT_INT2FLOAT",
  "OP_FLOAT_FLOAT2FLOAT", "OP_FLOAT_TRUNC", "OP_BRANCH", "OP_BRANCHIND",
  "OP_CALL", "OP_CALLIND", "OP_RETURN", "OP_CBRANCH", "OP_CALLOTHER",
  "OP_LOAD", "OP_STORE", "OP_PIECE", "OP_SUBPIECE", "OP_COPY",
  "BADINTEGER", "BEFORE_KEYWORD", "AFTER_KEYWORD", "REMOVE_KEYWORD",
  "SET_KEYWORD", "ACTION_TICK", "ISTRUE_KEYWORD", "ISFALSE_KEYWORD",
  "CHAR", "INTB", "OP_IDENTIFIER", "VAR_IDENTIFIER", "CONST_IDENTIFIER",
  "OP_NEW_IDENTIFIER", "VAR_NEW_IDENTIFIER", "DOT_IDENTIFIER", "'{'",
  "'}'", "'['", "']'", "';'", "'('", "')'", "'='", "':'", "$accept",
  "fullrule", "megaormid", "actionlist", "statementlist", "action",
  "orgroupmid", "statement", "opnode", "varnode", "deadnode", "opnewnode",
  "varnewnode", "deadnewnode", "rhs_const", "var_size", "number", "op_any",
  "op_list", "op_ident", "var_ident", "const_ident", "op_new_ident",
  "var_new_ident", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   123,   125,    91,    93,    59,    40,    41,    61,
      58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    91,    92,    92,    93,    93,    94,    94,    95,    95,
      96,    96,    96,    97,    97,    98,    98,    98,    98,    98,
      99,    99,    99,    99,    99,    99,    99,   100,   100,   100,
     100,   100,   100,   101,   101,   101,   101,   101,   101,   101,
     102,   102,   102,   102,   102,   102,   103,   103,   103,   103,
     104,   104,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   106,   106,   107,   107,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   109,   109,   110,   111,   112,   113,
     114
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     6,     2,     4,     1,     2,     0,     2,
       2,     2,     2,     1,     3,     2,     2,     2,     3,     3,
       1,     4,     3,     3,     4,     5,     5,     1,     6,     3,
       3,     5,     5,     6,     3,     3,     4,     3,     4,     4,
       6,     6,     6,     6,     1,     5,     6,     6,     6,     6,
       7,     6,     1,     3,     1,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     0,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     8,     0,     0,     1,     6,     0,     0,   136,   137,
     138,     8,     8,     0,     9,     0,     0,     0,    20,    27,
       0,     0,     0,     0,    13,     0,     0,   139,     2,     7,
       0,     0,     0,    44,     0,     0,     0,    15,     0,     0,
       0,     0,    16,     0,     0,    17,     0,     0,    74,     0,
       0,    52,     0,    54,     0,     8,     0,     8,     4,     8,
      18,    19,     0,     0,    10,     0,    11,    12,     0,    30,
       0,    29,    76,    78,    77,    84,    85,    83,   110,   109,
      92,    91,   112,   111,    99,    98,    94,    93,    97,    89,
      90,   106,   105,    82,    81,    88,    87,    86,   108,   107,
      96,    95,    80,    79,   100,   101,   102,   103,   104,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,     0,    34,     0,    23,    22,     0,     0,    72,    35,
      37,    75,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    38,
      55,    39,     0,     3,    13,    14,   140,     0,     0,     0,
      92,    91,     0,     0,     0,     0,    21,   135,    24,     0,
       0,     0,    36,    53,    59,    60,    58,    64,    63,    68,
      67,    66,    65,    71,    70,    69,    57,    56,    62,    61,
       5,     0,     0,     0,     0,     0,     0,    26,    25,    32,
      31,    73,     0,     0,     0,    45,     0,     0,     0,     0,
      33,    28,    46,    47,    51,    72,    48,    49,    40,    42,
      41,    43,    50
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,    23,    13,     3,    29,    25,    14,    15,    16,
      17,    30,    31,    32,    50,   182,    51,   130,   131,    18,
      52,    53,    34,   167
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -149
static const yytype_int16 yypact[] =
{
     -70,  -149,    25,   508,  -149,  -149,   -52,   -48,  -149,  -149,
    -149,  -149,  -149,   -28,  -149,     0,     4,   -43,  -149,  -149,
     -42,    -2,    -2,    -5,   525,    -1,   113,  -149,  -149,  -149,
       9,   -27,   -24,  -149,   -23,   -11,   -67,  -149,   415,    -9,
     -32,    -9,  -149,    15,    -2,  -149,    -2,     2,  -149,    -2,
     263,  -149,   -10,  -149,   286,  -149,     5,  -149,   -60,  -149,
    -149,  -149,    -8,   -13,  -149,    13,  -149,  -149,   474,  -149,
      17,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
      -9,    -9,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,   205,  -149,    -9,  -149,  -149,   -11,   -11,    94,   -10,
     356,  -149,   309,    -2,    -2,    -2,    -2,    -2,    -2,    -2,
      -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,  -149,
    -149,  -149,   525,  -149,   538,   538,  -149,     7,    -2,   474,
    -149,  -149,   -30,    12,    14,    19,  -149,  -149,  -149,    20,
      23,    -2,  -149,  -149,   379,   528,   546,   607,   607,   621,
     621,   621,   621,    50,    50,    50,    18,    18,  -149,  -149,
     -60,   -53,   333,    24,   -60,   -60,    -2,  -149,  -149,  -149,
    -149,   356,    32,    36,   -17,  -149,    39,    40,    41,    43,
     356,   -10,  -149,  -149,  -149,    94,   -10,  -149,  -149,  -149,
    -149,  -149,  -149
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -149,  -149,  -149,    -7,    42,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,    -4,  -127,  -149,   -66,  -149,   -12,
      -3,    10,  -148,   -84
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_int16 yytable[] =
{
      19,    33,   172,    35,    36,    55,   133,    40,    41,    59,
       9,    47,     1,    20,    62,    63,     8,    58,    54,    27,
      70,    19,   212,    19,     9,     4,    47,   132,   134,   135,
     136,   137,    69,    71,    20,    21,    20,   204,   205,    22,
     138,   139,   140,    45,     8,   142,    33,    46,     8,   157,
     158,    27,   224,    24,    26,    28,   217,   219,    48,    66,
       9,    10,    67,   166,    68,   177,     9,     8,   174,   175,
      49,   160,   166,    48,   168,     9,    10,   141,   155,   156,
      56,   157,   158,   169,    60,    49,    37,    38,   163,    39,
      42,    43,   173,    44,   201,    64,    65,   162,   232,   164,
     206,   165,   207,   203,   143,   144,   145,   208,   209,   146,
     147,   210,   215,   148,   149,   150,   151,   152,   153,   154,
     222,   178,   155,   156,   223,   157,   158,   228,   229,   230,
     227,   231,     0,   179,   180,     0,     0,     0,     0,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,     0,     0,     0,    19,
       0,    19,    19,     0,   202,     0,     0,     0,     0,     0,
       0,     0,    20,     0,    20,    20,     0,   211,     0,     0,
       0,     0,     0,     0,   181,     6,     7,     0,    33,     8,
       9,    10,   216,   218,     0,     0,     0,    57,   213,     0,
      12,    61,   220,   221,     0,     0,     0,     0,     0,     0,
     225,   226,    72,    73,    74,    75,    76,    77,    78,    79,
     170,   171,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,     0,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,     0,     0,   143,   144,   145,     0,     0,   146,   147,
       0,     0,   148,   149,   150,   151,   152,   153,   154,     0,
       0,   155,   156,   176,   157,   158,   143,   144,   145,     0,
       0,   146,   147,     0,     0,   148,   149,   150,   151,   152,
     153,   154,     0,     0,   155,   156,     0,   157,   158,   143,
     144,   145,     0,     0,   146,   147,     0,     0,   148,   149,
     150,   151,   152,   153,   154,     0,     0,   155,   156,     0,
     157,   158,     0,   143,   144,   145,     0,     0,   146,   147,
       0,   159,   148,   149,   150,   151,   152,   153,   154,     0,
       0,   155,   156,     0,   157,   158,   143,   144,   145,     0,
       0,   146,   147,     0,   161,   148,   149,   150,   151,   152,
     153,   154,     0,     0,   155,   156,     0,   157,   158,     0,
     144,   145,     0,     0,   146,   147,     0,   183,   148,   149,
     150,   151,   152,   153,   154,     0,     0,   155,   156,     0,
     157,   158,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   214,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,     0,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,    72,    73,    74,    75,    76,    77,    78,    79,   170,
     171,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,     0,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     145,     0,     0,   146,   147,     0,     0,   148,   149,   150,
     151,   152,   153,   154,     0,     0,   155,   156,     0,   157,
     158,   146,   147,     0,     0,   148,   149,   150,   151,   152,
     153,   154,     0,     0,   155,   156,     0,   157,   158,     5,
       6,     7,     0,     0,     8,     9,    10,     0,     0,     0,
       0,     0,    11,     0,     0,    12,     5,     6,     7,     0,
       0,     8,     9,    10,     0,     0,     0,     0,     0,    57,
       6,     7,    12,     0,     8,     9,    10,     0,     0,     0,
       0,     0,    57,     0,     0,    12,   148,   149,   150,   151,
     152,   153,   154,     0,     0,   155,   156,     0,   157,   158,
      -1,    -1,    -1,    -1,   152,   153,   154,     0,     0,   155,
     156,     0,   157,   158
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-149)))

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-1)))

static const yytype_int16 yycheck[] =
{
       3,    13,    68,     3,     4,    10,    38,     3,     4,    10,
      77,    28,    82,     3,     5,     6,    76,    24,    22,    79,
      87,    24,    75,    26,    77,     0,    28,    39,    40,    41,
      15,    16,    35,    36,    24,    87,    26,    67,    68,    87,
      44,    44,    46,    86,    76,    49,    58,    89,    76,    31,
      32,    79,    69,    11,    12,    83,   204,   205,    75,    86,
      77,    78,    86,    80,    87,   131,    77,    76,    80,    81,
      87,    81,    80,    75,    87,    77,    78,    75,    28,    29,
      85,    31,    32,    70,    85,    87,    86,    87,    83,    89,
      86,    87,    75,    89,    87,    86,    87,    55,   225,    57,
      88,    59,    88,   169,    10,    11,    12,    88,    88,    15,
      16,    88,    88,    19,    20,    21,    22,    23,    24,    25,
      88,   133,    28,    29,    88,    31,    32,    88,    88,    88,
     214,    88,    -1,   136,   137,    -1,    -1,    -1,    -1,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   162,    -1,    -1,    -1,   162,
      -1,   164,   165,    -1,   168,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   162,    -1,   164,   165,    -1,   181,    -1,    -1,
      -1,    -1,    -1,    -1,    90,    72,    73,    -1,   200,    76,
      77,    78,   204,   205,    -1,    -1,    -1,    84,   201,    -1,
      87,    88,   206,   206,    -1,    -1,    -1,    -1,    -1,    -1,
     214,   214,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    -1,    -1,    10,    11,    12,    -1,    -1,    15,    16,
      -1,    -1,    19,    20,    21,    22,    23,    24,    25,    -1,
      -1,    28,    29,    88,    31,    32,    10,    11,    12,    -1,
      -1,    15,    16,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    -1,    -1,    28,    29,    -1,    31,    32,    10,
      11,    12,    -1,    -1,    15,    16,    -1,    -1,    19,    20,
      21,    22,    23,    24,    25,    -1,    -1,    28,    29,    -1,
      31,    32,    -1,    10,    11,    12,    -1,    -1,    15,    16,
      -1,    88,    19,    20,    21,    22,    23,    24,    25,    -1,
      -1,    28,    29,    -1,    31,    32,    10,    11,    12,    -1,
      -1,    15,    16,    -1,    88,    19,    20,    21,    22,    23,
      24,    25,    -1,    -1,    28,    29,    -1,    31,    32,    -1,
      11,    12,    -1,    -1,    15,    16,    -1,    88,    19,    20,
      21,    22,    23,    24,    25,    -1,    -1,    28,    29,    -1,
      31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    88,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      12,    -1,    -1,    15,    16,    -1,    -1,    19,    20,    21,
      22,    23,    24,    25,    -1,    -1,    28,    29,    -1,    31,
      32,    15,    16,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    -1,    -1,    28,    29,    -1,    31,    32,    71,
      72,    73,    -1,    -1,    76,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    87,    71,    72,    73,    -1,
      -1,    76,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,
      72,    73,    87,    -1,    76,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    87,    19,    20,    21,    22,
      23,    24,    25,    -1,    -1,    28,    29,    -1,    31,    32,
      19,    20,    21,    22,    23,    24,    25,    -1,    -1,    28,
      29,    -1,    31,    32
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    82,    92,    95,     0,    71,    72,    73,    76,    77,
      78,    84,    87,    94,    98,    99,   100,   101,   110,   111,
     112,    87,    87,    93,    95,    97,    95,    79,    83,    96,
     102,   103,   104,   110,   113,     3,     4,    86,    87,    89,
       3,     4,    86,    87,    89,    86,    89,    28,    75,    87,
     105,   107,   111,   112,   105,    10,    85,    84,    94,    10,
      85,    88,     5,     6,    86,    87,    86,    86,    87,   111,
      87,   111,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
     108,   109,   110,    38,   110,   110,    15,    16,   105,   111,
     105,    75,   105,    10,    11,    12,    15,    16,    19,    20,
      21,    22,    23,    24,    25,    28,    29,    31,    32,    88,
      81,    88,    95,    83,    95,    95,    80,   114,    87,    70,
      15,    16,   108,    75,   110,   110,    88,   108,   110,   111,
     111,    90,   106,    88,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
      94,    87,   105,   108,    67,    68,    88,    88,    88,    88,
      88,   105,    75,   111,    88,    88,   110,   113,   110,   113,
     105,   111,    88,    88,    69,   105,   111,   114,    88,    88,
      88,    88,   106
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 77 "ruleparse.y"
    { rulecompile->setFullRule( rulecompile->mergeGroups((yyvsp[(2) - (4)].group),(yyvsp[(3) - (4)].group)) ); }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 78 "ruleparse.y"
    { (yyvsp[(2) - (6)].group)->addConstraint( (yyvsp[(4) - (6)].group) ); rulecompile->setFullRule( (yyvsp[(2) - (6)].group) ); }
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 81 "ruleparse.y"
    { (yyval.group) = rulecompile->emptyOrGroup(); rulecompile->addOr((yyval.group),rulecompile->mergeGroups((yyvsp[(1) - (2)].group),(yyvsp[(2) - (2)].group))); }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 82 "ruleparse.y"
    { (yyval.group) = rulecompile->addOr((yyvsp[(1) - (4)].group),rulecompile->mergeGroups((yyvsp[(3) - (4)].group),(yyvsp[(4) - (4)].group))); }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 85 "ruleparse.y"
    { (yyval.group) = rulecompile->emptyGroup(); }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 86 "ruleparse.y"
    { (yyval.group) = rulecompile->mergeGroups((yyvsp[(1) - (2)].group),(yyvsp[(2) - (2)].group)); }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 89 "ruleparse.y"
    { (yyval.group) = rulecompile->emptyGroup(); }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 90 "ruleparse.y"
    { (yyval.group) = rulecompile->mergeGroups((yyvsp[(1) - (2)].group),(yyvsp[(2) - (2)].group)); }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 93 "ruleparse.y"
    { (yyval.group) = (yyvsp[(1) - (2)].group); }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 94 "ruleparse.y"
    { (yyval.group) = (yyvsp[(1) - (2)].group); }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 95 "ruleparse.y"
    { (yyval.group) = (yyvsp[(1) - (2)].group); }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 98 "ruleparse.y"
    { ConstraintGroup *newbase = rulecompile->emptyOrGroup(); (yyval.group) = rulecompile->addOr(newbase,(yyvsp[(1) - (1)].group)); }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 99 "ruleparse.y"
    { (yyval.group) = rulecompile->addOr((yyvsp[(1) - (3)].group),(yyvsp[(3) - (3)].group)); }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 102 "ruleparse.y"
    { (yyval.group) = (yyvsp[(1) - (2)].group); }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 103 "ruleparse.y"
    { (yyval.group) = (yyvsp[(1) - (2)].group); }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 104 "ruleparse.y"
    { (yyval.group) = (yyvsp[(1) - (2)].group); }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 105 "ruleparse.y"
    { (yyval.group) = rulecompile->emptyGroup(); (yyval.group)->addConstraint((yyvsp[(2) - (3)].group)); }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 106 "ruleparse.y"
    { (yyval.group) = (yyvsp[(2) - (3)].group); }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 109 "ruleparse.y"
    { (yyval.group) = rulecompile->newOp((yyvsp[(1) - (1)].id)); }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 110 "ruleparse.y"
    { (yyval.group) = rulecompile->opCodeConstraint((yyvsp[(1) - (4)].group),(yyvsp[(3) - (4)].opcodelist)); }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 111 "ruleparse.y"
    { (yyval.group) = rulecompile->varDef((yyvsp[(1) - (3)].group),(yyvsp[(3) - (3)].id)); }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 112 "ruleparse.y"
    { (yyval.group) = rulecompile->varDescend((yyvsp[(1) - (3)].group),(yyvsp[(3) - (3)].id)); }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 113 "ruleparse.y"
    { (yyval.group) = rulecompile->varUniqueDescend((yyvsp[(1) - (4)].group),(yyvsp[(4) - (4)].id)); }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 114 "ruleparse.y"
    { (yyval.group) = rulecompile->opCompareConstraint((yyvsp[(1) - (5)].group),(yyvsp[(4) - (5)].id),CPUI_INT_EQUAL); }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 115 "ruleparse.y"
    { (yyval.group) = rulecompile->opCompareConstraint((yyvsp[(1) - (5)].group),(yyvsp[(4) - (5)].id),CPUI_INT_NOTEQUAL); }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 118 "ruleparse.y"
    { (yyval.group) = rulecompile->newVarnode((yyvsp[(1) - (1)].id)); }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 119 "ruleparse.y"
    { (yyval.group) = rulecompile->opInput((yyvsp[(1) - (6)].group),(yyvsp[(4) - (6)].big),(yyvsp[(6) - (6)].id)); }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 120 "ruleparse.y"
    { (yyval.group) = rulecompile->opInputAny((yyvsp[(1) - (3)].group),(yyvsp[(3) - (3)].id)); }
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 121 "ruleparse.y"
    { (yyval.group) = rulecompile->opOutput((yyvsp[(1) - (3)].group),(yyvsp[(3) - (3)].id)); }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 122 "ruleparse.y"
    { (yyval.group) = rulecompile->varCompareConstraint((yyvsp[(1) - (5)].group),(yyvsp[(4) - (5)].id),CPUI_INT_EQUAL); }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 123 "ruleparse.y"
    { (yyval.group) = rulecompile->varCompareConstraint((yyvsp[(1) - (5)].group),(yyvsp[(4) - (5)].id),CPUI_INT_NOTEQUAL); }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 126 "ruleparse.y"
    { (yyval.group) = rulecompile->opInputConstVal((yyvsp[(1) - (6)].group),(yyvsp[(4) - (6)].big),(yyvsp[(6) - (6)].rhsconst)); }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 127 "ruleparse.y"
    { (yyval.group) = rulecompile->opCopy((yyvsp[(1) - (3)].group),(yyvsp[(3) - (3)].id)); }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 128 "ruleparse.y"
    { (yyval.group) = rulecompile->varCopy((yyvsp[(1) - (3)].group),(yyvsp[(3) - (3)].id)); }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 129 "ruleparse.y"
    { (yyval.group) = rulecompile->varConst((yyvsp[(1) - (4)].group),(yyvsp[(3) - (4)].rhsconst),(yyvsp[(4) - (4)].rhsconst)); }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 130 "ruleparse.y"
    { (yyval.group) = rulecompile->constNamedExpression((yyvsp[(1) - (3)].id),(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 131 "ruleparse.y"
    { (yyval.group) = rulecompile->booleanConstraint(true,(yyvsp[(3) - (4)].rhsconst)); }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 132 "ruleparse.y"
    { (yyval.group) = rulecompile->booleanConstraint(false,(yyvsp[(3) - (4)].rhsconst)); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 135 "ruleparse.y"
    { (yyval.group) = rulecompile->opCreation((yyvsp[(1) - (6)].id),(yyvsp[(3) - (6)].opcode),false,(yyvsp[(5) - (6)].id)); }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 136 "ruleparse.y"
    { (yyval.group) = rulecompile->opCreation((yyvsp[(1) - (6)].id),(yyvsp[(3) - (6)].opcode),true,(yyvsp[(5) - (6)].id)); }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 137 "ruleparse.y"
    { (yyval.group) = rulecompile->opCreation((yyvsp[(1) - (6)].id),(yyvsp[(3) - (6)].opcode),false,(yyvsp[(5) - (6)].id)); }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 138 "ruleparse.y"
    { (yyval.group) = rulecompile->opCreation((yyvsp[(1) - (6)].id),(yyvsp[(3) - (6)].opcode),true,(yyvsp[(5) - (6)].id)); }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 139 "ruleparse.y"
    { (yyval.group) = rulecompile->newOp((yyvsp[(1) - (1)].id)); }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 140 "ruleparse.y"
    { (yyval.group) = rulecompile->newSetOpcode((yyvsp[(1) - (5)].group),(yyvsp[(4) - (5)].opcode)); }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 143 "ruleparse.y"
    { (yyval.group) = rulecompile->newUniqueOut((yyvsp[(1) - (6)].group),(yyvsp[(3) - (6)].id),-((int4)*(yyvsp[(5) - (6)].big))); delete (yyvsp[(5) - (6)].big); }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 144 "ruleparse.y"
    { (yyval.group) = rulecompile->newUniqueOut((yyvsp[(1) - (6)].group),(yyvsp[(3) - (6)].id),(yyvsp[(5) - (6)].id)); }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 145 "ruleparse.y"
    { (yyval.group) = rulecompile->newSetInput((yyvsp[(1) - (6)].group),(yyvsp[(4) - (6)].rhsconst),(yyvsp[(6) - (6)].id)); }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 146 "ruleparse.y"
    { (yyval.group) = rulecompile->newSetInput((yyvsp[(1) - (6)].group),(yyvsp[(4) - (6)].rhsconst),(yyvsp[(6) - (6)].id)); }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 149 "ruleparse.y"
    { (yyval.group) = rulecompile->newSetInputConstVal((yyvsp[(1) - (7)].group),(yyvsp[(4) - (7)].rhsconst),(yyvsp[(6) - (7)].rhsconst),(yyvsp[(7) - (7)].rhsconst)); }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 150 "ruleparse.y"
    { (yyval.group) = rulecompile->removeInput((yyval.group),(yyvsp[(4) - (6)].rhsconst)); }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 153 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constAbsolute((yyvsp[(1) - (1)].big)); }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 154 "ruleparse.y"
    { (yyval.rhsconst) = (yyvsp[(2) - (3)].rhsconst); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 155 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constNamed((yyvsp[(1) - (1)].id)); }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 156 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->dotIdentifier((yyvsp[(1) - (2)].id),(yyvsp[(2) - (2)].str)); }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 157 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_ADD,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 158 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_SUB,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 159 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_AND,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 160 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_OR,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 161 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_XOR,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 162 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_MULT,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 163 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_DIV,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 164 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_EQUAL,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 165 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_NOTEQUAL,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 166 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_LESS,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 167 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_LESSEQUAL,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 168 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_SLESS,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 169 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_SLESSEQUAL,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 170 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_LEFT,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 171 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_RIGHT,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 172 "ruleparse.y"
    { (yyval.rhsconst) = rulecompile->constBinaryExpression((yyvsp[(1) - (3)].rhsconst),CPUI_INT_SRIGHT,(yyvsp[(3) - (3)].rhsconst)); }
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 175 "ruleparse.y"
    { (yyval.rhsconst) = (RHSConstant *)0; }
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 176 "ruleparse.y"
    { (yyval.rhsconst) = (yyvsp[(2) - (2)].rhsconst); }
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 179 "ruleparse.y"
    { (yyval.big) = (yyvsp[(1) - (1)].big); }
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 180 "ruleparse.y"
    { *(yyvsp[(2) - (2)].big) = -*(yyvsp[(2) - (2)].big); (yyval.big) = (yyvsp[(2) - (2)].big); }
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 183 "ruleparse.y"
    { (yyval.opcode) = CPUI_BOOL_OR; }
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 184 "ruleparse.y"
    { (yyval.opcode) = CPUI_BOOL_AND; }
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 185 "ruleparse.y"
    { (yyval.opcode) = CPUI_BOOL_XOR; }
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 186 "ruleparse.y"
    { (yyval.opcode) = CPUI_BOOL_NEGATE; }
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 187 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_NEGATE; }
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 188 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_ADD; }
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 189 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_SUB; }
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 190 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_AND; }
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 191 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_OR; }
    break;

  case 85:
/* Line 1792 of yacc.c  */
#line 192 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_XOR; }
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 193 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_MULT; }
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 194 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_DIV; }
    break;

  case 88:
/* Line 1792 of yacc.c  */
#line 195 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_REM; }
    break;

  case 89:
/* Line 1792 of yacc.c  */
#line 196 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_RIGHT; }
    break;

  case 90:
/* Line 1792 of yacc.c  */
#line 197 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_LEFT; }
    break;

  case 91:
/* Line 1792 of yacc.c  */
#line 198 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_EQUAL; }
    break;

  case 92:
/* Line 1792 of yacc.c  */
#line 199 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_NOTEQUAL; }
    break;

  case 93:
/* Line 1792 of yacc.c  */
#line 200 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_LESS; }
    break;

  case 94:
/* Line 1792 of yacc.c  */
#line 201 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_LESSEQUAL; }
    break;

  case 95:
/* Line 1792 of yacc.c  */
#line 202 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_SDIV; }
    break;

  case 96:
/* Line 1792 of yacc.c  */
#line 203 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_SREM; }
    break;

  case 97:
/* Line 1792 of yacc.c  */
#line 204 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_SRIGHT; }
    break;

  case 98:
/* Line 1792 of yacc.c  */
#line 205 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_SLESS; }
    break;

  case 99:
/* Line 1792 of yacc.c  */
#line 206 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_SLESSEQUAL; }
    break;

  case 100:
/* Line 1792 of yacc.c  */
#line 207 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_ZEXT; }
    break;

  case 101:
/* Line 1792 of yacc.c  */
#line 208 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_CARRY; }
    break;

  case 102:
/* Line 1792 of yacc.c  */
#line 209 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_SEXT; }
    break;

  case 103:
/* Line 1792 of yacc.c  */
#line 210 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_SCARRY; }
    break;

  case 104:
/* Line 1792 of yacc.c  */
#line 211 "ruleparse.y"
    { (yyval.opcode) = CPUI_INT_SBORROW; }
    break;

  case 105:
/* Line 1792 of yacc.c  */
#line 212 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_ADD; }
    break;

  case 106:
/* Line 1792 of yacc.c  */
#line 213 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_SUB; }
    break;

  case 107:
/* Line 1792 of yacc.c  */
#line 214 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_MULT; }
    break;

  case 108:
/* Line 1792 of yacc.c  */
#line 215 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_DIV; }
    break;

  case 109:
/* Line 1792 of yacc.c  */
#line 216 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_EQUAL; }
    break;

  case 110:
/* Line 1792 of yacc.c  */
#line 217 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_NOTEQUAL; }
    break;

  case 111:
/* Line 1792 of yacc.c  */
#line 218 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_LESS; }
    break;

  case 112:
/* Line 1792 of yacc.c  */
#line 219 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_LESSEQUAL; }
    break;

  case 113:
/* Line 1792 of yacc.c  */
#line 220 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_NAN; }
    break;

  case 114:
/* Line 1792 of yacc.c  */
#line 221 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_ABS; }
    break;

  case 115:
/* Line 1792 of yacc.c  */
#line 222 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_SQRT; }
    break;

  case 116:
/* Line 1792 of yacc.c  */
#line 223 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_CEIL; }
    break;

  case 117:
/* Line 1792 of yacc.c  */
#line 224 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_FLOOR; }
    break;

  case 118:
/* Line 1792 of yacc.c  */
#line 225 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_ROUND; }
    break;

  case 119:
/* Line 1792 of yacc.c  */
#line 226 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_INT2FLOAT; }
    break;

  case 120:
/* Line 1792 of yacc.c  */
#line 227 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_FLOAT2FLOAT; }
    break;

  case 121:
/* Line 1792 of yacc.c  */
#line 228 "ruleparse.y"
    { (yyval.opcode) = CPUI_FLOAT_TRUNC; }
    break;

  case 122:
/* Line 1792 of yacc.c  */
#line 229 "ruleparse.y"
    { (yyval.opcode) = CPUI_BRANCH; }
    break;

  case 123:
/* Line 1792 of yacc.c  */
#line 230 "ruleparse.y"
    { (yyval.opcode) = CPUI_BRANCHIND; }
    break;

  case 124:
/* Line 1792 of yacc.c  */
#line 231 "ruleparse.y"
    { (yyval.opcode) = CPUI_CALL; }
    break;

  case 125:
/* Line 1792 of yacc.c  */
#line 232 "ruleparse.y"
    { (yyval.opcode) = CPUI_CALLIND; }
    break;

  case 126:
/* Line 1792 of yacc.c  */
#line 233 "ruleparse.y"
    { (yyval.opcode) = CPUI_RETURN; }
    break;

  case 127:
/* Line 1792 of yacc.c  */
#line 234 "ruleparse.y"
    { (yyval.opcode) = CPUI_CBRANCH; }
    break;

  case 128:
/* Line 1792 of yacc.c  */
#line 235 "ruleparse.y"
    { (yyval.opcode) = CPUI_CALLOTHER; }
    break;

  case 129:
/* Line 1792 of yacc.c  */
#line 236 "ruleparse.y"
    { (yyval.opcode) = CPUI_LOAD; }
    break;

  case 130:
/* Line 1792 of yacc.c  */
#line 237 "ruleparse.y"
    { (yyval.opcode) = CPUI_STORE; }
    break;

  case 131:
/* Line 1792 of yacc.c  */
#line 238 "ruleparse.y"
    { (yyval.opcode) = CPUI_PIECE; }
    break;

  case 132:
/* Line 1792 of yacc.c  */
#line 239 "ruleparse.y"
    { (yyval.opcode) = CPUI_SUBPIECE; }
    break;

  case 133:
/* Line 1792 of yacc.c  */
#line 240 "ruleparse.y"
    { (yyval.opcode) = CPUI_COPY; }
    break;

  case 134:
/* Line 1792 of yacc.c  */
#line 243 "ruleparse.y"
    { (yyval.opcodelist) = new vector<OpCode>; (yyval.opcodelist)->push_back((yyvsp[(1) - (1)].opcode)); }
    break;

  case 135:
/* Line 1792 of yacc.c  */
#line 244 "ruleparse.y"
    { (yyval.opcodelist) = (yyvsp[(1) - (2)].opcodelist); (yyval.opcodelist)->push_back((yyvsp[(2) - (2)].opcode)); }
    break;

  case 136:
/* Line 1792 of yacc.c  */
#line 247 "ruleparse.y"
    { (yyval.id) = rulecompile->findIdentifier((yyvsp[(1) - (1)].str)); }
    break;

  case 137:
/* Line 1792 of yacc.c  */
#line 250 "ruleparse.y"
    { (yyval.id) = rulecompile->findIdentifier((yyvsp[(1) - (1)].str)); }
    break;

  case 138:
/* Line 1792 of yacc.c  */
#line 253 "ruleparse.y"
    { (yyval.id) = rulecompile->findIdentifier((yyvsp[(1) - (1)].str)); }
    break;

  case 139:
/* Line 1792 of yacc.c  */
#line 256 "ruleparse.y"
    { (yyval.id) = rulecompile->findIdentifier((yyvsp[(1) - (1)].str)); }
    break;

  case 140:
/* Line 1792 of yacc.c  */
#line 259 "ruleparse.y"
    { (yyval.id) = rulecompile->findIdentifier((yyvsp[(1) - (1)].str)); }
    break;


/* Line 1792 of yacc.c  */
#line 2605 "ruleparse.tab.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2055 of yacc.c  */
#line 262 "ruleparse.y"


inline int ruleparselex(void)

{
  return rulecompile->nextToken();
}

int ruleparseerror(const char *s)

{
  rulecompile->ruleError(s);
  return 0;
}

#endif
