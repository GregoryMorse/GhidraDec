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
#define yyparse         cparseparse
#define yylex           cparselex
#define yyerror         cparseerror
#define yylval          cparselval
#define yychar          cparsechar
#define yydebug         cparsedebug
#define yynerrs         cparsenerrs

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 16 "grammar.y"

#include "grammar.hh"

extern int yylex(void);
extern int yyerror(const char *str);
static CParse *parse;
extern int yydebug;

/* Line 371 of yacc.c  */
#line 84 "grammar.tab.cpp"

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
   by #include "grammar.tab.h".  */
#ifndef YY_CPARSE_GRAMMAR_TAB_H_INCLUDED
# define YY_CPARSE_GRAMMAR_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int cparsedebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DOTDOTDOT = 258,
     BADTOKEN = 259,
     STRUCT = 260,
     UNION = 261,
     ENUM = 262,
     DECLARATION_RESULT = 263,
     PARAM_RESULT = 264,
     NUMBER = 265,
     IDENTIFIER = 266,
     STORAGE_CLASS_SPECIFIER = 267,
     TYPE_QUALIFIER = 268,
     FUNCTION_SPECIFIER = 269,
     TYPE_NAME = 270
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 25 "grammar.y"

  uint4 flags;
  TypeDeclarator *dec;
  vector<TypeDeclarator *> *declist;
  TypeSpecifiers *spec;
  vector<uint4> *ptrspec;
  Datatype *type;
  Enumerator *enumer;
  vector<Enumerator *> *vecenum;
  string *str;
  uintb *i;


/* Line 387 of yacc.c  */
#line 156 "grammar.tab.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE cparselval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int cparseparse (void *YYPARSE_PARAM);
#else
int cparseparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int cparseparse (void);
#else
int cparseparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_CPARSE_GRAMMAR_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 184 "grammar.tab.cpp"

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
#define YYFINAL  18
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   155

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  26
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  71
/* YYNRULES -- Number of states.  */
#define YYNSTATES  115

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   270

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      21,    22,    25,     2,    17,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    16,
       2,    20,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    23,     2,    24,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    18,     2,    19,     2,     2,     2,     2,
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
      15
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    16,    18,    20,    22,
      24,    27,    30,    33,    36,    38,    42,    44,    46,    48,
      50,    55,    61,    64,    69,    75,    78,    80,    83,    87,
      89,    92,    94,    97,    99,   103,   105,   111,   116,   123,
     129,   132,   134,   138,   140,   144,   146,   149,   151,   155,
     161,   166,   171,   173,   176,   179,   183,   185,   188,   190,
     194,   196,   200,   203,   205,   208,   210,   212,   215,   219,
     224,   229
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      27,     0,    -1,     8,    28,    -1,     9,    48,    -1,    29,
      16,    -1,    29,    30,    16,    -1,    12,    -1,    32,    -1,
      13,    -1,    14,    -1,    12,    29,    -1,    32,    29,    -1,
      13,    29,    -1,    14,    29,    -1,    31,    -1,    30,    17,
      31,    -1,    42,    -1,    15,    -1,    33,    -1,    39,    -1,
       5,    18,    34,    19,    -1,     5,    11,    18,    34,    19,
      -1,     5,    11,    -1,     6,    18,    34,    19,    -1,     6,
      11,    18,    34,    19,    -1,     6,    11,    -1,    35,    -1,
      34,    35,    -1,    36,    37,    16,    -1,    32,    -1,    32,
      36,    -1,    13,    -1,    13,    36,    -1,    38,    -1,    37,
      17,    38,    -1,    42,    -1,     7,    11,    18,    40,    19,
      -1,     7,    18,    40,    19,    -1,     7,    11,    18,    40,
      17,    19,    -1,     7,    18,    40,    17,    19,    -1,     7,
      11,    -1,    41,    -1,    40,    17,    41,    -1,    11,    -1,
      11,    20,    10,    -1,    43,    -1,    44,    43,    -1,    11,
      -1,    21,    42,    22,    -1,    43,    23,    45,    51,    24,
      -1,    43,    23,    51,    24,    -1,    43,    21,    46,    22,
      -1,    25,    -1,    25,    45,    -1,    25,    44,    -1,    25,
      45,    44,    -1,    13,    -1,    45,    13,    -1,    47,    -1,
      47,    17,     3,    -1,    48,    -1,    47,    17,    48,    -1,
      29,    42,    -1,    29,    -1,    29,    49,    -1,    44,    -1,
      50,    -1,    44,    50,    -1,    21,    49,    22,    -1,    50,
      23,    51,    24,    -1,    50,    21,    46,    22,    -1,    10,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    60,    60,    61,    65,    66,    70,    71,    72,    73,
      74,    75,    76,    77,    81,    82,    86,    91,    92,    93,
      97,    98,    99,   100,   101,   102,   106,   107,   111,   115,
     116,   117,   118,   122,   123,   127,   132,   133,   134,   135,
     136,   140,   141,   145,   146,   150,   151,   155,   156,   157,
     158,   160,   165,   166,   167,   168,   172,   173,   177,   178,
     182,   183,   187,   188,   189,   193,   194,   195,   199,   201,
     203,   207
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DOTDOTDOT", "BADTOKEN", "STRUCT",
  "UNION", "ENUM", "DECLARATION_RESULT", "PARAM_RESULT", "NUMBER",
  "IDENTIFIER", "STORAGE_CLASS_SPECIFIER", "TYPE_QUALIFIER",
  "FUNCTION_SPECIFIER", "TYPE_NAME", "';'", "','", "'{'", "'}'", "'='",
  "'('", "')'", "'['", "']'", "'*'", "$accept", "document", "declaration",
  "declaration_specifiers", "init_declarator_list", "init_declarator",
  "type_specifier", "struct_or_union_specifier", "struct_declaration_list",
  "struct_declaration", "specifier_qualifier_list",
  "struct_declarator_list", "struct_declarator", "enum_specifier",
  "enumerator_list", "enumerator", "declarator", "direct_declarator",
  "pointer", "type_qualifier_list", "parameter_type_list",
  "parameter_list", "parameter_declaration", "abstract_declarator",
  "direct_abstract_declarator", "assignment_expression", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,    59,    44,   123,   125,
      61,    40,    41,    91,    93,    42
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    26,    27,    27,    28,    28,    29,    29,    29,    29,
      29,    29,    29,    29,    30,    30,    31,    32,    32,    32,
      33,    33,    33,    33,    33,    33,    34,    34,    35,    36,
      36,    36,    36,    37,    37,    38,    39,    39,    39,    39,
      39,    40,    40,    41,    41,    42,    42,    43,    43,    43,
      43,    43,    44,    44,    44,    44,    45,    45,    46,    46,
      47,    47,    48,    48,    48,    49,    49,    49,    50,    50,
      50,    51
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     2,     3,     1,     1,     1,     1,
       2,     2,     2,     2,     1,     3,     1,     1,     1,     1,
       4,     5,     2,     4,     5,     2,     1,     2,     3,     1,
       2,     1,     2,     1,     3,     1,     5,     4,     6,     5,
       2,     1,     3,     1,     3,     1,     2,     1,     3,     5,
       4,     4,     1,     2,     2,     3,     1,     2,     1,     3,
       1,     3,     2,     1,     2,     1,     1,     2,     3,     4,
       4,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     6,     8,     9,
      17,     2,     0,     7,    18,    19,    63,     3,     1,    22,
       0,    25,     0,    40,     0,    10,    12,    13,    47,     4,
       0,    52,     0,    14,    16,    45,     0,    11,     0,    62,
      65,    64,    66,     0,    31,    29,     0,    26,     0,     0,
       0,     0,    43,     0,    41,     0,    56,    54,    53,     5,
       0,     0,     0,    46,     0,    67,     0,     0,     0,    32,
      30,    20,    27,     0,    33,    35,     0,    23,     0,     0,
       0,    37,    48,    57,    55,    15,     0,    58,    60,    71,
       0,     0,    68,     0,     0,    21,    28,     0,    24,     0,
      36,    44,    39,    42,    51,     0,     0,    50,    70,    69,
      34,    38,    59,    61,    49
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,    11,    16,    32,    33,    45,    14,    46,    47,
      48,    73,    74,    15,    53,    54,    34,    35,    36,    58,
      86,    87,    88,    41,    42,    91
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -71
static const yytype_int16 yypact[] =
{
     114,   102,   102,    16,    41,    59,   108,   102,   102,   102,
     -71,   -71,    62,   102,   -71,   -71,     9,   -71,   -71,    31,
     105,    46,   105,    54,    68,   -71,   -71,   -71,   -71,   -71,
      12,     2,   116,   -71,   -71,   104,    70,   -71,     9,   -71,
      71,   -71,   107,   105,   105,   105,    35,   -71,    12,   105,
      38,    68,    65,    39,   -71,    91,   -71,   -71,    11,   -71,
      12,   102,     8,   104,   117,   107,   102,   128,    56,   -71,
     -71,   -71,   -71,   118,   -71,   -71,    61,   -71,   112,   130,
       3,   -71,   -71,   -71,   -71,   -71,   119,    76,   -71,   -71,
     111,   120,   -71,   121,   122,   -71,   -71,    12,   -71,    36,
     -71,   -71,   -71,   -71,   -71,    83,   123,   -71,   -71,   -71,
     -71,   -71,   -71,   -71,   -71
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -71,   -71,   -71,    93,   -71,    82,    -1,   -71,   -11,   -37,
      92,   -71,    48,   -71,    97,   -70,   -13,    63,   -12,    87,
      84,   -71,     0,   113,   115,   -62
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      13,    13,    17,    39,    40,    94,    13,    13,    13,    72,
     103,    50,    13,    72,    52,    56,    18,    55,    89,    57,
      28,    56,   102,    28,    83,    55,    40,    31,   106,   103,
      38,    72,    68,    30,    31,    75,    31,    31,    76,    72,
       4,     5,     6,     4,     5,     6,    84,    52,    44,    43,
      10,    44,    19,    10,    71,   111,    80,    77,    81,    20,
      13,     4,     5,     6,    49,    13,     4,     5,     6,    44,
      21,    10,    51,    28,    44,    95,    10,    22,    29,    52,
      98,    28,    28,    30,    75,    79,   112,    31,     4,     5,
       6,    30,    38,   105,    12,     7,     8,     9,    10,    63,
      25,    26,    27,    63,    13,   113,    37,     4,     5,     6,
       4,     5,     6,    82,     7,     8,     9,    10,    44,    23,
      10,    89,     1,     2,    83,    61,    24,    62,    66,    99,
      67,   100,    59,    60,    96,    97,    69,    70,    89,    92,
     101,   104,    85,   108,   107,   110,   109,   114,    78,    90,
      93,    64,     0,     0,     0,    65
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-71)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
       1,     2,     2,    16,    16,    67,     7,     8,     9,    46,
      80,    22,    13,    50,    11,    13,     0,    30,    10,    31,
      11,    13,    19,    11,    13,    38,    38,    25,    90,    99,
      21,    68,    43,    21,    25,    48,    25,    25,    49,    76,
       5,     6,     7,     5,     6,     7,    58,    11,    13,    18,
      15,    13,    11,    15,    19,    19,    17,    19,    19,    18,
      61,     5,     6,     7,    18,    66,     5,     6,     7,    13,
      11,    15,    18,    11,    13,    19,    15,    18,    16,    11,
      19,    11,    11,    21,    97,    20,     3,    25,     5,     6,
       7,    21,    21,    17,     1,    12,    13,    14,    15,    36,
       7,     8,     9,    40,   105,   105,    13,     5,     6,     7,
       5,     6,     7,    22,    12,    13,    14,    15,    13,    11,
      15,    10,     8,     9,    13,    21,    18,    23,    21,    17,
      23,    19,    16,    17,    16,    17,    44,    45,    10,    22,
      10,    22,    60,    22,    24,    97,    24,    24,    51,    62,
      66,    38,    -1,    -1,    -1,    40
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     8,     9,    27,     5,     6,     7,    12,    13,    14,
      15,    28,    29,    32,    33,    39,    29,    48,     0,    11,
      18,    11,    18,    11,    18,    29,    29,    29,    11,    16,
      21,    25,    30,    31,    42,    43,    44,    29,    21,    42,
      44,    49,    50,    18,    13,    32,    34,    35,    36,    18,
      34,    18,    11,    40,    41,    42,    13,    44,    45,    16,
      17,    21,    23,    43,    49,    50,    21,    23,    34,    36,
      36,    19,    35,    37,    38,    42,    34,    19,    40,    20,
      17,    19,    22,    13,    44,    31,    46,    47,    48,    10,
      45,    51,    22,    46,    51,    19,    16,    17,    19,    17,
      19,    10,    19,    41,    22,    17,    51,    24,    22,    24,
      38,    19,     3,    48,    24
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
#line 60 "grammar.y"
    { parse->setResultDeclarations((yyvsp[(2) - (2)].declist)); }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 61 "grammar.y"
    { vector<TypeDeclarator *> *res = parse->newVecDeclarator(); res->push_back((yyvsp[(2) - (2)].dec)); parse->setResultDeclarations(res); }
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 65 "grammar.y"
    { (yyval.declist) = parse->mergeSpecDecVec((yyvsp[(1) - (2)].spec)); }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 66 "grammar.y"
    { (yyval.declist) = parse->mergeSpecDecVec((yyvsp[(1) - (3)].spec),(yyvsp[(2) - (3)].declist)); }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 70 "grammar.y"
    { (yyval.spec) = parse->newSpecifier(); parse->addSpecifier((yyval.spec),(yyvsp[(1) - (1)].str)); }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 71 "grammar.y"
    { (yyval.spec) = parse->newSpecifier(); parse->addTypeSpecifier((yyval.spec),(yyvsp[(1) - (1)].type)); }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 72 "grammar.y"
    { (yyval.spec) = parse->newSpecifier(); parse->addSpecifier((yyval.spec),(yyvsp[(1) - (1)].str)); }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 73 "grammar.y"
    { (yyval.spec) = parse->newSpecifier(); parse->addFuncSpecifier((yyval.spec),(yyvsp[(1) - (1)].str)); }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 74 "grammar.y"
    { (yyval.spec) = parse->addSpecifier((yyvsp[(2) - (2)].spec),(yyvsp[(1) - (2)].str)); }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 75 "grammar.y"
    { (yyval.spec) = parse->addTypeSpecifier((yyvsp[(2) - (2)].spec),(yyvsp[(1) - (2)].type)); }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 76 "grammar.y"
    { (yyval.spec) = parse->addSpecifier((yyvsp[(2) - (2)].spec),(yyvsp[(1) - (2)].str)); }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 77 "grammar.y"
    { (yyval.spec) = parse->addFuncSpecifier((yyvsp[(2) - (2)].spec),(yyvsp[(1) - (2)].str)); }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 81 "grammar.y"
    { (yyval.declist) = parse->newVecDeclarator(); (yyval.declist)->push_back((yyvsp[(1) - (1)].dec)); }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 82 "grammar.y"
    { (yyval.declist) = (yyvsp[(1) - (3)].declist); (yyval.declist)->push_back((yyvsp[(3) - (3)].dec)); }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 86 "grammar.y"
    { (yyval.dec) = (yyvsp[(1) - (1)].dec); }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 91 "grammar.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 92 "grammar.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 93 "grammar.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 97 "grammar.y"
    { (yyval.type) = parse->newStruct("",(yyvsp[(3) - (4)].declist)); }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 98 "grammar.y"
    { (yyval.type) = parse->newStruct(*(yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].declist)); }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 99 "grammar.y"
    { (yyval.type) = parse->oldStruct(*(yyvsp[(2) - (2)].str)); }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 100 "grammar.y"
    { (yyval.type) = parse->newUnion("",(yyvsp[(3) - (4)].declist)); }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 101 "grammar.y"
    { (yyval.type) = parse->newUnion(*(yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].declist)); }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 102 "grammar.y"
    { (yyval.type) = parse->oldUnion(*(yyvsp[(2) - (2)].str)); }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 106 "grammar.y"
    { (yyval.declist) = (yyvsp[(1) - (1)].declist); }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 107 "grammar.y"
    { (yyval.declist) = (yyvsp[(1) - (2)].declist); (yyval.declist)->insert((yyval.declist)->end(),(yyvsp[(2) - (2)].declist)->begin(),(yyvsp[(2) - (2)].declist)->end()); }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 111 "grammar.y"
    { (yyval.declist) = parse->mergeSpecDecVec((yyvsp[(1) - (3)].spec),(yyvsp[(2) - (3)].declist)); }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 115 "grammar.y"
    { (yyval.spec) = parse->newSpecifier(); parse->addTypeSpecifier((yyval.spec),(yyvsp[(1) - (1)].type)); }
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 116 "grammar.y"
    { (yyval.spec) = parse->addTypeSpecifier((yyvsp[(2) - (2)].spec),(yyvsp[(1) - (2)].type)); }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 117 "grammar.y"
    { (yyval.spec) = parse->newSpecifier(); parse->addSpecifier((yyval.spec),(yyvsp[(1) - (1)].str)); }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 118 "grammar.y"
    { (yyval.spec) = parse->addSpecifier((yyvsp[(2) - (2)].spec),(yyvsp[(1) - (2)].str)); }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 122 "grammar.y"
    { (yyval.declist) = parse->newVecDeclarator(); (yyval.declist)->push_back((yyvsp[(1) - (1)].dec)); }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 123 "grammar.y"
    { (yyval.declist) = (yyvsp[(1) - (3)].declist); (yyval.declist)->push_back((yyvsp[(3) - (3)].dec)); }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 127 "grammar.y"
    { (yyval.dec) = (yyvsp[(1) - (1)].dec); }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 132 "grammar.y"
    { (yyval.type) = parse->newEnum(*(yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].vecenum)); }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 133 "grammar.y"
    { (yyval.type) = parse->newEnum("",(yyvsp[(3) - (4)].vecenum)); }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 134 "grammar.y"
    { (yyval.type) = parse->newEnum(*(yyvsp[(2) - (6)].str),(yyvsp[(4) - (6)].vecenum)); }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 135 "grammar.y"
    { (yyval.type) = parse->newEnum("",(yyvsp[(3) - (5)].vecenum)); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 136 "grammar.y"
    { (yyval.type) = parse->oldEnum(*(yyvsp[(2) - (2)].str)); }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 140 "grammar.y"
    { (yyval.vecenum) = parse->newVecEnumerator(); (yyval.vecenum)->push_back((yyvsp[(1) - (1)].enumer)); }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 141 "grammar.y"
    { (yyval.vecenum) = (yyvsp[(1) - (3)].vecenum); (yyval.vecenum)->push_back((yyvsp[(3) - (3)].enumer)); }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 145 "grammar.y"
    { (yyval.enumer) = parse->newEnumerator(*(yyvsp[(1) - (1)].str)); }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 146 "grammar.y"
    { (yyval.enumer) = parse->newEnumerator(*(yyvsp[(1) - (3)].str),*(yyvsp[(3) - (3)].i)); }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 150 "grammar.y"
    { (yyval.dec) = (yyvsp[(1) - (1)].dec); }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 151 "grammar.y"
    { (yyval.dec) = parse->mergePointer((yyvsp[(1) - (2)].ptrspec),(yyvsp[(2) - (2)].dec)); }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 155 "grammar.y"
    { (yyval.dec) = parse->newDeclarator((yyvsp[(1) - (1)].str)); }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 156 "grammar.y"
    { (yyval.dec) = (yyvsp[(2) - (3)].dec); }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 157 "grammar.y"
    { (yyval.dec) = parse->newArray((yyvsp[(1) - (5)].dec),(yyvsp[(3) - (5)].flags),(yyvsp[(4) - (5)].i)); }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 158 "grammar.y"
    { (yyval.dec) = parse->newArray((yyvsp[(1) - (4)].dec),0,(yyvsp[(3) - (4)].i)); }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 160 "grammar.y"
    { (yyval.dec) = parse->newFunc((yyvsp[(1) - (4)].dec),(yyvsp[(3) - (4)].declist)); }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 165 "grammar.y"
    { (yyval.ptrspec) = parse->newPointer(); (yyval.ptrspec)->push_back(0); }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 166 "grammar.y"
    { (yyval.ptrspec) = parse->newPointer(); (yyval.ptrspec)->push_back((yyvsp[(2) - (2)].flags)); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 167 "grammar.y"
    { (yyval.ptrspec) = (yyvsp[(2) - (2)].ptrspec); (yyval.ptrspec)->push_back(0); }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 168 "grammar.y"
    { (yyval.ptrspec) = (yyvsp[(3) - (3)].ptrspec); (yyval.ptrspec)->push_back((yyvsp[(2) - (3)].flags)); }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 172 "grammar.y"
    { (yyval.flags) = parse->convertFlag((yyvsp[(1) - (1)].str)); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 173 "grammar.y"
    { (yyval.flags) = (yyvsp[(1) - (2)].flags); (yyval.flags) |= parse->convertFlag((yyvsp[(2) - (2)].str)); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 177 "grammar.y"
    { (yyval.declist) = (yyvsp[(1) - (1)].declist); }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 178 "grammar.y"
    { (yyval.declist) = (yyvsp[(1) - (3)].declist); (yyval.declist)->push_back((TypeDeclarator *)0); }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 182 "grammar.y"
    { (yyval.declist) = parse->newVecDeclarator(); (yyval.declist)->push_back((yyvsp[(1) - (1)].dec)); }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 183 "grammar.y"
    { (yyval.declist) = (yyvsp[(1) - (3)].declist); (yyval.declist)->push_back((yyvsp[(3) - (3)].dec)); }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 187 "grammar.y"
    { (yyval.dec) = parse->mergeSpecDec((yyvsp[(1) - (2)].spec),(yyvsp[(2) - (2)].dec)); }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 188 "grammar.y"
    { (yyval.dec) = parse->mergeSpecDec((yyvsp[(1) - (1)].spec)); }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 189 "grammar.y"
    { (yyval.dec) = parse->mergeSpecDec((yyvsp[(1) - (2)].spec),(yyvsp[(2) - (2)].dec)); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 193 "grammar.y"
    { (yyval.dec) = parse->newDeclarator(); parse->mergePointer((yyvsp[(1) - (1)].ptrspec),(yyval.dec)); }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 194 "grammar.y"
    { (yyval.dec) = (yyvsp[(1) - (1)].dec); }
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 195 "grammar.y"
    { (yyval.dec) = parse->mergePointer((yyvsp[(1) - (2)].ptrspec),(yyvsp[(2) - (2)].dec)); }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 199 "grammar.y"
    { (yyval.dec) = (yyvsp[(2) - (3)].dec); }
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 201 "grammar.y"
    { (yyval.dec) = parse->newArray((yyvsp[(1) - (4)].dec),0,(yyvsp[(3) - (4)].i)); }
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 203 "grammar.y"
    { (yyval.dec) = parse->newFunc((yyvsp[(1) - (4)].dec),(yyvsp[(3) - (4)].declist)); }
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 207 "grammar.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;


/* Line 1792 of yacc.c  */
#line 1912 "grammar.tab.cpp"
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
#line 210 "grammar.y"


void GrammarToken::set(uint4 tp)

{
  type = tp;
}

void GrammarToken::set(uint4 tp,char *ptr,int4 len)

{
  type = tp;
  switch(tp) {
  case integer:
    {
      string charstring(ptr,len);
      istringstream s(charstring);
      s.unsetf(ios::dec | ios::hex | ios::oct);
      intb val;
      s >> val;
      value.integer = (uintb)val;
    }
    break;
  case identifier:
  case stringval:
    value.stringval = new string(ptr,len);
    break;
  case charconstant:
    if (len==1)
      value.integer = (uintb)*ptr;
    else {			// Backslash
      switch(ptr[1]) {
      case 'n':
	value.integer = 10;
	break;
      case '0':
	value.integer = 0;
	break;
      case 'a':
	value.integer = 7;
	break;
      case 'b':
	value.integer = 8;
	break;
      case 't':
	value.integer = 9;
	break;
      case 'v':
	value.integer = 11;
	break;
      case 'f':
	value.integer = 12;
	break;
      case 'r':
	value.integer = 13;
	break;
      default:
	value.integer = (uintb)ptr[1];
	break;
      }
    }
    break;
  default:
    throw LowlevelError("Bad internal grammar token set");
  }
}

GrammarToken::GrammarToken(void)

{
  type = 0;
  value.integer = 0;
}

GrammarLexer::GrammarLexer(int4 maxbuffer)

{
  buffersize = maxbuffer;
  buffer = new char[ maxbuffer ];
  bufstart = 0;
  bufend = 0;
  curlineno = 0;
  state = start;
  in = (istream *)0;
  endoffile = true;
}

GrammarLexer::~GrammarLexer(void)

{
  delete [] buffer;
}

void GrammarLexer::bumpLine(void)

{				// Keep track of a newline
  curlineno += 1;
  bufstart = 0;
  bufend = 0;
}

uint4 GrammarLexer::moveState(char lookahead)

{ // Change finite state machine based on lookahead
  uint4 res;
  bool newline = false;

  if (lookahead<32) {
    if ((lookahead == 9)||(lookahead==11)||(lookahead==12)||
	(lookahead==13))
      lookahead = ' ';
    else if (lookahead == '\n') {
      newline = true;
      lookahead = ' ';
    }
    else {
      setError("Illegal character");
      return GrammarToken::badtoken;
    }
  }
  else if (lookahead >= 127) {
    setError("Illegal character");
    return GrammarToken::badtoken;
  }

  res = 0;
  bool syntaxerror = false;
  switch(state) {
  case start:
    switch(lookahead) {
    case '/':
      state = slash;
      break;
    case '.':
      state = dot1;
      break;
    case '*':
    case ',':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case ';':
    case '=':
      state = punctuation;
      bufstart = bufend-1;
      break;
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      state = number;
      bufstart = bufend-1;
      break;
    case ' ':
      break;			// Ignore since we are already open
    case '\"':
      state = doublequote;
      bufstart = bufend-1;
      break;
    case '\'':
      state = singlequote;
      break;
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
      state = identifier;
      bufstart = bufend-1;
      break;
    default:
      setError("Illegal character");
      return GrammarToken::badtoken;
    }
    break;
  case slash:
    if (lookahead=='*')
      state = c_comment;
    else if (lookahead == '/')
      state = endofline_comment;
    else
      syntaxerror = true;
    break;
  case dot1:
    if (lookahead=='.')
      state = dot2;
    else
      syntaxerror = true;
    break;
  case dot2:
    if (lookahead=='.')
      state = dot3;
    else
      syntaxerror = true;
    break;
  case dot3:
    state = start;
    res = GrammarToken::dotdotdot;
    break;
  case punctuation:
    state = start;
    res = (uint4)buffer[bufstart];
    break;
  case endofline_comment:
    if (newline)
      state = start;
    break;			// Anything else is part of comment
  case c_comment:
    if (lookahead == '/') {
      if ((bufend >1)&&(buffer[bufend-2]=='*'))
	state = start;
    }
    break;			// Anything else is part of comment
  case doublequote:
    if (lookahead == '\"')
      state = doublequoteend;
    break;			// Anything else is part of string
  case doublequoteend:
    state = start;
    res = GrammarToken::stringval;
    break;
  case singlequote:
    if (lookahead == '\\')
      state = singlebackslash;
    else if (lookahead == '\'')
      state = singlequoteend;
    break;			// Anything else is part of string
  case singlequoteend:
    state = start;
    res = GrammarToken::charconstant;
    break;
  case singlebackslash:	// Seen backslash in a single quoted string
    state = singlequote;
    break;
  case number:
    if (lookahead=='x') {
      if (((bufend-bufstart)!=2)||(buffer[bufstart]!='0'))
	syntaxerror = true;	// x only allowed as 0x hex indicator
    }
    else if ((lookahead>='0')&&(lookahead<='9')) {
    }
    else if ((lookahead>='A')&&(lookahead<='Z')) {
    }
    else if ((lookahead>='a')&&(lookahead<='z')) {
    }
    else if (lookahead == '_') {
    }
    else {
      state = start;
      res = GrammarToken::integer;
    }
    break;
  case identifier:
    if ((lookahead>='0')&&(lookahead<='9')) {
    }
    else if ((lookahead>='A')&&(lookahead<='Z')) {
    }
    else if ((lookahead>='a')&&(lookahead<='z')) {
    }
    else if (lookahead == '_' || lookahead == ':') {
    }
    else {
      state = start;
      res = GrammarToken::identifier;
    }
    break;
  }
  if (syntaxerror) {
    setError("Syntax error");
    return GrammarToken::badtoken;
  }
  if (newline) bumpLine();
  return res;
}

void GrammarLexer::establishToken(GrammarToken &token,uint4 val)

{
  if (val < GrammarToken::integer)
    token.set(val);
  else {
    token.set(val,buffer+bufstart,(bufend-bufstart)-1);
  }
  token.setPosition(filestack.back(),curlineno,bufstart);
}

void GrammarLexer::clear(void)

{ // Clear lexer for a brand new parse
  filenamemap.clear();
  streammap.clear();
  filestack.clear();
  bufstart = 0;
  bufend = 0;
  curlineno = 0;
  state = start;
  in = (istream *)0;
  endoffile = true;
  error.clear();
}

void GrammarLexer::writeLocation(ostream &s,int4 line,int4 filenum)

{
  s << " at line " << dec << line;
  s << " in " << filenamemap[filenum];
}

void GrammarLexer::writeTokenLocation(ostream &s,int4 line,int4 colno)

{
  if (line!=curlineno) return;	// Does line match current line in buffer
  for(int4 i=0;i<bufend;++i)
    s << buffer[i];
  s << '\n';
  for(int4 i=0;i<colno;++i)
    s << ' ';
  s << "^--\n";
}

void GrammarLexer::pushFile(const string &filename,istream *i)

{
  int4 filenum = filenamemap.size();
  filenamemap[filenum] = filename;
  streammap[filenum] = i;
  filestack.push_back(filenum);
  in = i;
  endoffile = false;
}

void GrammarLexer::popFile(void)

{
  filestack.pop_back();
  if (filestack.empty()) {
    endoffile = true;
    return;
  }
  int4 filenum = filestack.back();
  in = streammap[filenum];	// Get previous stream
}

void GrammarLexer::getNextToken(GrammarToken &token)

{ // Read next token, return true if end of stream
  char nextchar;
  uint4 tok = GrammarToken::badtoken;
  bool firsttimethru = true;

  if (endoffile) {
    token.set(GrammarToken::endoffile);
    return;
  }
  do {
    if ((!firsttimethru)||(bufend==0)) {
      if (bufend >= buffersize) {
	setError("Line too long");
	tok = GrammarToken::badtoken;
	break;
      }
      in->get(nextchar);
      if (!(*in)) {
	endoffile = true;
	break;
      }
      buffer[bufend++] = nextchar;
    }
    else
      nextchar = buffer[bufend-1]; // Get old lookahead token
    tok = moveState(nextchar);
    firsttimethru = false;
  } while(tok == 0);
  if (endoffile) {
    buffer[bufend++] = ' ';	// Simulate a space
    tok = moveState(' ');	// to let the final token resolve
    if ((tok==0)&&(state != start)&&(state != endofline_comment)) {
      setError("Incomplete token");
      tok = GrammarToken::badtoken;
    }
  }
  establishToken(token,tok);
}

Datatype *PointerModifier::modType(Datatype *base,const TypeDeclarator *decl,Architecture *glb) const

{
  int4 addrsize = glb->getDefaultDataSpace()->getAddrSize();
  Datatype *restype;
  restype = glb->types->getTypePointer(addrsize,base,glb->getDefaultDataSpace()->getWordSize());
  return restype;
}

Datatype *ArrayModifier::modType(Datatype *base,const TypeDeclarator *decl,Architecture *glb) const

{
  Datatype *restype = glb->types->getTypeArray(arraysize,base);
  return restype;
}

FunctionModifier::FunctionModifier(const vector<TypeDeclarator *> *p,bool dtdtdt)

{
  paramlist = *p;
  if (paramlist.size()==1) {
    TypeDeclarator *decl = paramlist[0];
    if (decl->numModifiers()==0) { // Check for void as an inputtype
      Datatype *ct = decl->getBaseType();
      if ((ct != (Datatype *)0)&&(ct->getMetatype()==TYPE_VOID))
	paramlist.clear();
    }
  }
  dotdotdot = dtdtdt;
}

void FunctionModifier::getInTypes(vector<Datatype *> &intypes,Architecture *glb) const

{
  for(uint4 i=0;i<paramlist.size();++i) {
    Datatype *ct = paramlist[i]->buildType(glb);
    intypes.push_back( ct );
  }
}

void FunctionModifier::getInNames(vector<string> &innames) const

{
  for(uint4 i=0;i<paramlist.size();++i)
    innames.push_back(paramlist[i]->getIdentifier());
}

bool FunctionModifier::isValid(void) const

{
  for(uint4 i=0;i<paramlist.size();++i) {
    TypeDeclarator *decl = paramlist[i];
    if (!decl->isValid()) return false;
    if (decl->numModifiers()==0) {
      Datatype *ct = decl->getBaseType();
      if ((ct != (Datatype *)0)&&(ct->getMetatype()==TYPE_VOID))
	return false;		// Extra void type
    }
  }
  return true;
}

Datatype *FunctionModifier::modType(Datatype *base,const TypeDeclarator *decl,Architecture *glb) const

{
  vector<Datatype *> intypes;

  // Varargs is encoded as extra null pointer in paramlist
  bool dotdotdot = false;
  if ((!paramlist.empty())&&(paramlist.back() == (TypeDeclarator *)0)) {
    dotdotdot = true;
  }

  getInTypes(intypes,glb);

  ProtoModel *protomodel = decl->getModel(glb);
  return glb->types->getTypeCode(protomodel,base,intypes,dotdotdot);
}

TypeDeclarator::~TypeDeclarator(void)

{
  for(uint4 i=0;i<mods.size();++i)
    delete mods[i];
}

Datatype *TypeDeclarator::buildType(Architecture *glb) const

{ // Apply modifications to the basetype, (in reverse order of binding)
  Datatype *restype = basetype;
  vector<TypeModifier *>::const_iterator iter;
  iter = mods.end();
  while(iter != mods.begin()) {
    --iter;
    restype = (*iter)->modType(restype,this,glb);
  }
  return restype;
}

ProtoModel *TypeDeclarator::getModel(Architecture *glb) const

{
  // Get prototype model
  ProtoModel *protomodel = (ProtoModel *)0;
  if (model.size()!=0)
    protomodel = glb->getModel(model);
  if (protomodel == (ProtoModel *)0)
    protomodel = glb->defaultfp;
  return protomodel;
}

bool TypeDeclarator::getPrototype(PrototypePieces &pieces,Architecture *glb) const

{
  TypeModifier *mod = (TypeModifier *)0;
  if (mods.size() > 0)
    mod = mods[0];
  if ((mod == (TypeModifier *)0)||(mod->getType()!=TypeModifier::function_mod))
    return false;
  FunctionModifier *fmod = (FunctionModifier *)mod;

  pieces.model = getModel(glb);
  pieces.name = ident;
  pieces.intypes.clear();
  fmod->getInTypes(pieces.intypes,glb);
  pieces.innames.clear();
  fmod->getInNames(pieces.innames);
  pieces.dotdotdot = fmod->isDotdotdot();

  // Construct the output type
  pieces.outtype = basetype;
  vector<TypeModifier *>::const_iterator iter;
  iter = mods.end();
  --iter;			// At least one modification
  while(iter != mods.begin()) { // Do not apply function modifier
    pieces.outtype = (*iter)->modType(pieces.outtype,this,glb);
    --iter;
  }
  return true;
}

bool TypeDeclarator::isValid(void) const

{
  if (basetype == (Datatype *)0)
    return false;		// No basetype

  int4 count=0;
  if ((flags & CParse::f_typedef)!=0)
    count += 1;
  if ((flags & CParse::f_extern)!=0)
    count += 1;
  if ((flags & CParse::f_static)!=0)
    count += 1;
  if ((flags & CParse::f_auto)!=0)
    count += 1;
  if ((flags & CParse::f_register)!=0)
    count += 1;
  if (count > 1)
    throw ParseError("Multiple storage specifiers");

  count = 0;
  if ((flags & CParse::f_const)!=0)
    count += 1;
  if ((flags & CParse::f_restrict)!=0)
    count += 1;
  if ((flags & CParse::f_volatile)!=0)
    count += 1;
  if (count > 1)
    throw ParseError("Multiple type qualifiers");
  
  for(uint4 i=0;i<mods.size();++i) {
    if (!mods[i]->isValid())
      return false;
  }
  return true;
}

CParse::CParse(Architecture *g,int4 maxbuf)
  : lexer(maxbuf)
{
  glb = g;
  firsttoken = -1;
  lastdecls = (vector<TypeDeclarator *> *)0;
  keywords["typedef"] = f_typedef;
  keywords["extern"] = f_extern;
  keywords["static"] = f_static;
  keywords["auto"] = f_auto;
  keywords["register"] = f_register;
  keywords["const"] = f_const;
  keywords["restrict"] = f_restrict;
  keywords["volatile"] = f_volatile;
  keywords["inline"] = f_inline;
  keywords["struct"] = f_struct;
  keywords["union"] = f_union;
  keywords["enum"] = f_enum;
}

CParse::~CParse(void)

{
  clearAllocation();
}

void CParse::clear(void)

{
  clearAllocation();
  lasterror.clear();
  lastdecls = (vector<TypeDeclarator *> *)0;
  lexer.clear();
  firsttoken = -1;
}

TypeDeclarator *CParse::mergeSpecDec(TypeSpecifiers *spec,TypeDeclarator *dec)

{
  dec->basetype = spec->type_specifier;
  dec->model = spec->function_specifier;
  dec->flags |= spec->flags;
  return dec;
}

TypeDeclarator *CParse::mergeSpecDec(TypeSpecifiers *spec)

{
  TypeDeclarator *dec = new TypeDeclarator();
  typedec_alloc.push_back(dec);
  return mergeSpecDec(spec,dec);
}

vector<TypeDeclarator *> *CParse::mergeSpecDecVec(TypeSpecifiers *spec,vector<TypeDeclarator *> *declist)

{
  for(uint4 i=0;i<declist->size();++i)
    mergeSpecDec(spec,(*declist)[i]);
  return declist;
}

vector<TypeDeclarator *> *CParse::mergeSpecDecVec(TypeSpecifiers *spec)

{
  vector<TypeDeclarator *> *declist;
  declist = new vector<TypeDeclarator *>();
  vecdec_alloc.push_back(declist);
  TypeDeclarator *dec = new TypeDeclarator();
  typedec_alloc.push_back(dec);
  declist->push_back( dec );
  return mergeSpecDecVec(spec,declist);
}

uint4 CParse::convertFlag(string *str)

{
  map<string,uint4>::const_iterator iter;

  iter = keywords.find(*str);
  if (iter != keywords.end())
    return (*iter).second;
  setError("Unknown qualifier");
  return 0;
}

TypeSpecifiers *CParse::addSpecifier(TypeSpecifiers *spec,string *str)

{
  uint4 flag = convertFlag(str);
  spec->flags |= flag;
  return spec;
}

TypeSpecifiers *CParse::addTypeSpecifier(TypeSpecifiers *spec,Datatype *tp)

{
  if (spec->type_specifier!=(Datatype *)0)
    setError("Multiple type specifiers");
  spec->type_specifier = tp;
  return spec;
}

TypeSpecifiers *CParse::addFuncSpecifier(TypeSpecifiers *spec,string *str)

{
  map<string,uint4>::const_iterator iter;

  iter = keywords.find(*str);
  if (iter != keywords.end())
    spec->flags |= (*iter).second; // A reserved specifier
  else {
    if (spec->function_specifier.size()!=0)
      setError("Multiple parameter models");
    spec->function_specifier = *str;
  }
  return spec;
}

TypeDeclarator *CParse::mergePointer(vector<uint4> *ptr,TypeDeclarator *dec)

{
  for(uint4 i=0;i<ptr->size();++i) {
    PointerModifier *newmod = new PointerModifier((*ptr)[i]);
    dec->mods.push_back(newmod);
  }
  return dec;
}

TypeDeclarator *CParse::newDeclarator(string *str)

{
  TypeDeclarator *res = new TypeDeclarator(*str);
  typedec_alloc.push_back(res);
  return res;
}

TypeDeclarator *CParse::newDeclarator(void)

{
  TypeDeclarator *res = new TypeDeclarator();
  typedec_alloc.push_back(res);
  return res;
}

TypeSpecifiers *CParse::newSpecifier(void)

{
  TypeSpecifiers *spec = new TypeSpecifiers();
  typespec_alloc.push_back(spec);
  return spec;
}

vector<TypeDeclarator *> *CParse::newVecDeclarator(void)

{
  vector<TypeDeclarator *> *res = new vector<TypeDeclarator *>();
  vecdec_alloc.push_back(res);
  return res;
}

vector<uint4> *CParse::newPointer(void)

{
  vector<uint4> *res = new vector<uint4>();
  vecuint4_alloc.push_back(res);
  return res;
}

TypeDeclarator *CParse::newArray(TypeDeclarator *dec,uint4 flags,uintb *num)

{
  ArrayModifier *newmod = new ArrayModifier(flags,(int4)*num);
  dec->mods.push_back(newmod);
  return dec;
}

TypeDeclarator *CParse::newFunc(TypeDeclarator *dec,vector<TypeDeclarator *> *declist)

{
  bool dotdotdot = false;
  if (!declist->empty()) {
    if (declist->back() == (TypeDeclarator *)0) {
      dotdotdot = true;
      declist->pop_back();
    }
  }
  FunctionModifier *newmod = new FunctionModifier(declist,dotdotdot);
  dec->mods.push_back(newmod);
  return dec;
}

Datatype *CParse::newStruct(const string &ident,vector<TypeDeclarator *> *declist)

{ // Build a new structure
  TypeStruct *res = glb->types->getTypeStruct(ident); // Create stub (for recursion)
  vector<TypeField> sublist;
  
  for(uint4 i=0;i<declist->size();++i) {
    TypeDeclarator *decl = (*declist)[i];
    if (!decl->isValid()) {
      setError("Invalid structure declarator");
      glb->types->destroyType(res);
      return (Datatype *)0;
    }
    sublist.push_back(TypeField());
    sublist.back().type = decl->buildType(glb);
    sublist.back().name = decl->getIdentifier();
    sublist.back().offset = -1;	// Let typegrp figure out offset
  }

  if (!glb->types->setFields(sublist,res,-1,0)) {
    setError("Bad structure definition");
    glb->types->destroyType(res);
    return (Datatype *)0;
  }
  return res;
}

Datatype *CParse::oldStruct(const string &ident)

{
  Datatype *res = glb->types->findByName(ident);
  if ((res==(Datatype *)0)||(res->getMetatype() != TYPE_STRUCT))
    setError("Identifier does not represent a struct as required");
  return res;
}

Datatype *CParse::newUnion(const string &ident,vector<TypeDeclarator *> *declist)

{
  setError("Unions are currently unsupported");
  return (Datatype *)0;
}

Datatype *CParse::oldUnion(const string &ident)

{
  setError("Unions are currently unsupported");
  return (Datatype *)0;
}

Enumerator *CParse::newEnumerator(const string &ident)

{
  Enumerator *res = new Enumerator(ident);
  enum_alloc.push_back(res);
  return res;
}

Enumerator *CParse::newEnumerator(const string &ident,uintb val)

{
  Enumerator *res = new Enumerator(ident,val);
  enum_alloc.push_back(res);
  return res;
}

vector<Enumerator *> *CParse::newVecEnumerator(void)

{
  vector<Enumerator *> *res = new vector<Enumerator *>();
  vecenum_alloc.push_back(res);
  return res;
}

Datatype *CParse::newEnum(const string &ident,vector<Enumerator *> *vecenum)

{
  TypeEnum *res = glb->types->getTypeEnum(ident);
  vector<string> namelist;
  vector<uintb> vallist;
  vector<bool> assignlist;
  for(uint4 i=0;i<vecenum->size();++i) {
    Enumerator *enumer = (*vecenum)[i];
    namelist.push_back(enumer->enumconstant);
    vallist.push_back(enumer->value);
    assignlist.push_back(enumer->constantassigned);
  }
  if (!glb->types->setEnumValues(namelist,vallist,assignlist,res)) {
    setError("Bad enumeration values");
    glb->types->destroyType(res);
    return (Datatype *)0;
  }
  return res;
}

Datatype *CParse::oldEnum(const string &ident)

{
  Datatype *res = glb->types->findByName(ident);
  if ((res==(Datatype *)0)||(!res->isEnumType()))
    setError("Identifier does not represent an enum as required");
  return res;
}

void CParse::clearAllocation(void)

{
  list<TypeDeclarator *>::iterator iter1;

  for(iter1=typedec_alloc.begin();iter1!=typedec_alloc.end();++iter1)
    delete *iter1;
  typedec_alloc.clear();

  list<TypeSpecifiers *>::iterator iter2;
  for(iter2=typespec_alloc.begin();iter2!=typespec_alloc.end();++iter2)
    delete *iter2;
  typespec_alloc.clear();

  list<vector<uint4> *>::iterator iter3;
  for(iter3=vecuint4_alloc.begin();iter3!=vecuint4_alloc.end();++iter3)
    delete *iter3;
  vecuint4_alloc.clear();

  list<vector<TypeDeclarator *> *>::iterator iter4;
  for(iter4=vecdec_alloc.begin();iter4!=vecdec_alloc.end();++iter4)
    delete *iter4;
  vecdec_alloc.clear();

  list<string *>::iterator iter5;
  for(iter5=string_alloc.begin();iter5!=string_alloc.end();++iter5)
    delete *iter5;
  string_alloc.clear();

  list<uintb *>::iterator iter6;
  for(iter6=num_alloc.begin();iter6!=num_alloc.end();++iter6)
    delete *iter6;
  num_alloc.clear();

  list<Enumerator *>::iterator iter7;
  for(iter7=enum_alloc.begin();iter7!=enum_alloc.end();++iter7)
    delete *iter7;
  enum_alloc.clear();

  list<vector<Enumerator *> *>::iterator iter8;
  for(iter8=vecenum_alloc.begin();iter8!=vecenum_alloc.end();++iter8)
    delete *iter8;
  vecenum_alloc.clear();
}

int4 CParse::lookupIdentifier(const string &nm)

{
  map<string,uint4>::const_iterator iter = keywords.find(nm);
  if (iter != keywords.end()) {
    switch( (*iter).second ) {
    case f_typedef:
    case f_extern:
    case f_static:
    case f_auto:
    case f_register:
      return STORAGE_CLASS_SPECIFIER;
    case f_const:
    case f_restrict:
    case f_volatile:
      return TYPE_QUALIFIER;
    case f_inline:
      return FUNCTION_SPECIFIER;
    case f_struct:
      return STRUCT;
    case f_union:
      return UNION;
    case f_enum:
      return ENUM;
    default:
      break;
    }
  }
  Datatype *tp = glb->types->findByName(nm);
  if (tp != (Datatype *)0) {
    yylval.type = tp;
    return TYPE_NAME;
  }
  if (glb->hasModel(nm))
    return FUNCTION_SPECIFIER;
  return IDENTIFIER;		// Unknown identifier
}

int4 CParse::lex(void)

{
  GrammarToken tok;

  if (firsttoken != -1) {
    int4 retval = firsttoken;
    firsttoken = -1;
    return retval;
  }
  if (lasterror.size()!=0)
    return BADTOKEN;
  lexer.getNextToken(tok);
  lineno = tok.getLineNo();
  colno = tok.getColNo();
  filenum = tok.getFileNum();
  switch(tok.getType()) {
  case GrammarToken::integer:
  case GrammarToken::charconstant:
    yylval.i = new uintb(tok.getInteger());
    num_alloc.push_back(yylval.i);
    return NUMBER;
  case GrammarToken::identifier:
    yylval.str = tok.getString();
    string_alloc.push_back(yylval.str);
    return lookupIdentifier(*yylval.str);
  case GrammarToken::stringval:
    delete tok.getString();
    setError("Illegal string constant");
    return BADTOKEN;
  case GrammarToken::dotdotdot:
    return DOTDOTDOT;
  case GrammarToken::badtoken:
    setError(lexer.getError());	// Error from lexer
    return BADTOKEN;
  case GrammarToken::endoffile:
    return -1;			// No more tokens
  default:
    return (int4)tok.getType();
  }
}

void CParse::setError(const string &msg)

{
  ostringstream s;

  s << msg;
  lexer.writeLocation(s,lineno,filenum);
  s << '\n';
  lexer.writeTokenLocation(s,lineno,colno);
  lasterror = s.str();
}

bool CParse::runParse(uint4 doctype)

{ // Assuming the stream has been setup, parse it
  switch(doctype) {
  case doc_declaration:
    firsttoken = DECLARATION_RESULT;
    break;
  case doc_parameter_declaration:
    firsttoken = PARAM_RESULT;
    break;
  default:
    throw LowlevelError("Bad document type");
  }
  parse = this;			// Setup global object for yyparse
  int4 res = yyparse();
  if (res != 0) {
    if (lasterror.size()==0)
      setError("Syntax error");
    return false;
  }
  return true;
}

bool CParse::parseFile(const string &nm,uint4 doctype)

{ // Run the parser on a file, return true if no parse errors
  clear();			// Clear out any old parsing

  ifstream s(nm.c_str());	// open file
  if (!s)
    throw LowlevelError("Unable to open file for parsing: "+nm);

  lexer.pushFile(nm,&s); 	// Inform lexer of filename and stream
  bool res = runParse(doctype);
  s.close();
  return res;
}

bool CParse::parseStream(istream &s,uint4 doctype)

{
  clear();

  lexer.pushFile("stream",&s);
  return runParse(doctype);
}

int yylex(void)

{
  return parse->lex();
}

int yyerror(const char *str)

{
  return 0;
}

Datatype *parse_type(istream &s,string &name,Architecture *glb)

{
  CParse parser(glb,1000);

  if (!parser.parseStream(s,CParse::doc_parameter_declaration))
    throw ParseError(parser.getError());
  vector<TypeDeclarator *> *decls = parser.getResultDeclarations();
  if ((decls == (vector<TypeDeclarator *> *)0)||(decls->size()==0))
    throw ParseError("Did not parse a datatype");
  if (decls->size() > 1)
    throw ParseError("Parsed multiple declarations");
  TypeDeclarator *decl = (*decls)[0];
  if (!decl->isValid())
    throw ParseError("Parsed type is invalid");
  name = decl->getIdentifier();
  return decl->buildType(glb);
}

void parse_protopieces(PrototypePieces &pieces,
		       istream &s,Architecture *glb)
{
  CParse parser(glb,1000);

  if (!parser.parseStream(s,CParse::doc_declaration))
    throw ParseError(parser.getError());
  vector<TypeDeclarator *> *decls = parser.getResultDeclarations();
  if ((decls == (vector<TypeDeclarator *> *)0)||(decls->size()==0))
    throw ParseError("Did not parse a datatype");
  if (decls->size() > 1)
    throw ParseError("Parsed multiple declarations");
  TypeDeclarator *decl = (*decls)[0];
  if (!decl->isValid())
    throw ParseError("Parsed type is invalid");
  
  if (!decl->getPrototype(pieces,glb))
    throw ParseError("Did not parse a prototype");
}

void parse_C(Architecture *glb,istream &s)

{ // Load type data straight into datastructures
  CParse parser(glb,1000);

  if (!parser.parseStream(s,CParse::doc_declaration))
    throw ParseError(parser.getError());
  vector<TypeDeclarator *> *decls = parser.getResultDeclarations();
  if ((decls == (vector<TypeDeclarator *> *)0)||(decls->size()==0))
    throw ParseError("Did not parse a datatype");
  if (decls->size() > 1)
    throw ParseError("Parsed multiple declarations");
  TypeDeclarator *decl = (*decls)[0];
  if (!decl->isValid())
    throw ParseError("Parsed type is invalid");

  if (decl->hasProperty(CParse::f_extern)) {
    PrototypePieces pieces;
    if (!decl->getPrototype(pieces,glb))
      throw ParseError("Did not parse prototype as expected");
    glb->setPrototype(pieces);
  }
  else if (decl->hasProperty(CParse::f_typedef)) {
    Datatype *ct = decl->buildType(glb);
    if (decl->getIdentifier().size() == 0)
      throw ParseError("Missing identifier for typedef");
    glb->types->setName(ct,decl->getIdentifier());
  }
  else if (decl->getBaseType()->getMetatype()==TYPE_STRUCT) {
    // We parsed a struct, treat as a typedef
  }
  else if (decl->getBaseType()->isEnumType()) {
    // We parsed an enum, treat as a typedef
  }
  else
    throw LowlevelError("Not sure what to do with this type");
}

void parse_toseparator(istream &s,string &name)

{				// parse to next (C) separator
  char tok;

  name.erase();
  s >> ws;
  tok = s.peek();

  while((isalnum(tok))||(tok=='_')) {
    s >> tok;
    name += tok;
    tok = s.peek();
  }
}

Address parse_varnode(istream &s,int4 &size,Address &pc,uintm &uq,const TypeFactory &typegrp)

{				// Scan for a specific varnode
  char tok;
  int4 discard;

  Address loc(parse_machaddr(s,size,typegrp));
  s >> ws >> tok;
  if (tok != '(')
    throw ParseError("Missing '('");
  s >> ws;
  tok = s.peek();
  pc = Address();	// pc starts out as invalid
  if (tok == 'i')
    s >> tok;
  else if (s.peek() != ':') {
    s.unsetf(ios::dec | ios::hex | ios::oct); // Let user specify base
    pc = parse_machaddr(s,discard,typegrp,true);
  }
  s >> ws;
  if (s.peek() == ':') {	// Scan uniq
    s >> tok >> ws >> hex >> uq; // Assume uniq is in hex
  }
  else
    uq = ~((uintm)0);
  s >> ws >> tok;
  if (tok != ')')
    throw ParseError("Missing ')'");
  return loc;
}

Address parse_op(istream &s,uintm &uq,const TypeFactory &typegrp)

{
  int4 size;
  char tok;
  Address loc(parse_machaddr(s,size,typegrp,true));
  s >> ws >> tok;
  if (tok != ':')
    throw ParseError("Missing ':'");
  s >> ws >> hex >> uq;		// Assume uniq is in hex
  return loc;
}

Address parse_machaddr(istream &s,int4 &defaultsize,const TypeFactory &typegrp,bool ignorecolon)

{				// Read Address from ASCII stream
  string token;
  AddrSpace *b;
  int4 size = -1;
  int4 oversize;
  char tok;
  const AddrSpaceManager *manage = typegrp.getArch();

  s >> ws;
  tok = s.peek();
  if (tok == '[') {
    s >> tok;
    parse_toseparator(s,token);	// scan base address token
    b = manage->getSpaceByName(token);
    if (b == (AddrSpace *)0)
      throw ParseError("Bad address base");
    s >> ws >> tok;
    if (tok != ',')
      throw ParseError("Missing ',' in address");
    parse_toseparator(s,token);	// Get the offset portion of the address
    s >> ws >> tok;
    if (tok == ',') {		// Optional size specifier
      s.unsetf(ios::dec | ios::hex | ios::oct);
      s >> size;
      s >> ws >> tok;
    }
    if (tok != ']')
      throw ParseError("Missing ']' in address");
  }
  else if (tok == '{') {
    b = manage->getJoinSpace();
    s >> tok;
    s >> tok;
    while(tok != '}')		// Scan to the matching curly brace
      token += tok;
  }
  else {
    if (tok == '0') {
      b = manage->getDefaultCodeSpace();
    }
    else {
      b = manage->getSpaceByShortcut(tok);
      s >> tok;
    }
    if (b==(AddrSpace *)0) {
      s >> token;
      string errmsg = "Bad address: ";
      errmsg += tok;
      errmsg += token;
      throw ParseError(errmsg);
    }
    token.erase();
    s >> ws;
    tok = s.peek();
    if (ignorecolon) {
      while((isalnum(tok))||(tok=='_')||(tok=='+')) {
	token += tok;
	s >> tok;
	tok = s.peek();
      }
    }
    else {
      while((isalnum(tok))||(tok=='_')||(tok=='+')||(tok==':')) {
	token += tok;
	s >> tok;
	tok = s.peek();
      }
    }
  }

  Address res(b,0);
  oversize = res.read(token); // Read the address of this particular type
				// oversize is "standard size"
  if (oversize == -1)
    throw ParseError("Bad machine address");
  defaultsize = (size==-1) ? oversize : size; // If not overriden use standard
  return res;
}

