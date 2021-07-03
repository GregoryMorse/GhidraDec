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




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 16 "slghparse.y"

#include "slgh_compile.hh"

#define YYERROR_VERBOSE

  extern SleighCompile *slgh;
  extern int4 actionon;
  extern FILE *yyin;
  extern int yydebug;
  extern int yylex(void);
  extern int yyerror(const char *str );

/* Line 371 of yacc.c  */
#line 81 "slghparse.tab.cpp"

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
   by #include "slghparse.tab.hh".  */
#ifndef YY_YY_SLGHPARSE_TAB_HH_INCLUDED
# define YY_YY_SLGHPARSE_TAB_HH_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OP_BOOL_OR = 258,
     OP_BOOL_XOR = 259,
     OP_BOOL_AND = 260,
     OP_OR = 261,
     OP_XOR = 262,
     OP_AND = 263,
     OP_FNOTEQUAL = 264,
     OP_FEQUAL = 265,
     OP_NOTEQUAL = 266,
     OP_EQUAL = 267,
     OP_FGREATEQUAL = 268,
     OP_FLESSEQUAL = 269,
     OP_FGREAT = 270,
     OP_FLESS = 271,
     OP_SGREAT = 272,
     OP_SLESSEQUAL = 273,
     OP_SGREATEQUAL = 274,
     OP_SLESS = 275,
     OP_LESSEQUAL = 276,
     OP_GREATEQUAL = 277,
     OP_SRIGHT = 278,
     OP_RIGHT = 279,
     OP_LEFT = 280,
     OP_FSUB = 281,
     OP_FADD = 282,
     OP_FDIV = 283,
     OP_FMULT = 284,
     OP_SREM = 285,
     OP_SDIV = 286,
     OP_ZEXT = 287,
     OP_CARRY = 288,
     OP_BORROW = 289,
     OP_SEXT = 290,
     OP_SCARRY = 291,
     OP_SBORROW = 292,
     OP_NAN = 293,
     OP_ABS = 294,
     OP_SQRT = 295,
     OP_CEIL = 296,
     OP_FLOOR = 297,
     OP_ROUND = 298,
     OP_INT2FLOAT = 299,
     OP_FLOAT2FLOAT = 300,
     OP_TRUNC = 301,
     OP_CPOOLREF = 302,
     OP_NEW = 303,
     BADINTEGER = 304,
     GOTO_KEY = 305,
     CALL_KEY = 306,
     RETURN_KEY = 307,
     IF_KEY = 308,
     DEFINE_KEY = 309,
     ATTACH_KEY = 310,
     MACRO_KEY = 311,
     SPACE_KEY = 312,
     TYPE_KEY = 313,
     RAM_KEY = 314,
     DEFAULT_KEY = 315,
     REGISTER_KEY = 316,
     ENDIAN_KEY = 317,
     WITH_KEY = 318,
     ALIGN_KEY = 319,
     OP_UNIMPL = 320,
     TOKEN_KEY = 321,
     SIGNED_KEY = 322,
     NOFLOW_KEY = 323,
     HEX_KEY = 324,
     DEC_KEY = 325,
     BIG_KEY = 326,
     LITTLE_KEY = 327,
     SIZE_KEY = 328,
     WORDSIZE_KEY = 329,
     OFFSET_KEY = 330,
     NAMES_KEY = 331,
     VALUES_KEY = 332,
     VARIABLES_KEY = 333,
     PCODEOP_KEY = 334,
     IS_KEY = 335,
     LOCAL_KEY = 336,
     DELAYSLOT_KEY = 337,
     CROSSBUILD_KEY = 338,
     EXPORT_KEY = 339,
     BUILD_KEY = 340,
     CONTEXT_KEY = 341,
     ELLIPSIS_KEY = 342,
     GLOBALSET_KEY = 343,
     BITRANGE_KEY = 344,
     CHAR = 345,
     INTEGER = 346,
     INTB = 347,
     STRING = 348,
     SYMBOLSTRING = 349,
     SPACESYM = 350,
     SECTIONSYM = 351,
     TOKENSYM = 352,
     USEROPSYM = 353,
     VALUESYM = 354,
     VALUEMAPSYM = 355,
     CONTEXTSYM = 356,
     NAMESYM = 357,
     VARSYM = 358,
     BITSYM = 359,
     SPECSYM = 360,
     VARLISTSYM = 361,
     OPERANDSYM = 362,
     STARTSYM = 363,
     ENDSYM = 364,
     MACROSYM = 365,
     LABELSYM = 366,
     SUBTABLESYM = 367
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 29 "slghparse.y"

  char ch;
  uintb *i;
  intb *big;
  string *str;
  vector<string> *strlist;
  vector<intb> *biglist;
  vector<ExprTree *> *param;
  SpaceQuality *spacequal;
  FieldQuality *fieldqual;
  StarQuality *starqual;
  VarnodeTpl *varnode;
  ExprTree *tree;
  vector<OpTpl *> *stmt;
  ConstructTpl *sem;
  SectionVector *sectionstart;
  Constructor *construct;
  PatternEquation *pateq;
  PatternExpression *patexp;

  vector<SleighSymbol *> *symlist;
  vector<ContextChange *> *contop;
  SleighSymbol *anysym;
  SpaceSymbol *spacesym;
  SectionSymbol *sectionsym;
  TokenSymbol *tokensym;
  UserOpSymbol *useropsym;
  MacroSymbol *macrosym;
  LabelSymbol *labelsym;
  SubtableSymbol *subtablesym;
  StartSymbol *startsym;
  EndSymbol *endsym;
  OperandSymbol *operandsym;
  VarnodeListSymbol *varlistsym;
  VarnodeSymbol *varsym;
  BitrangeSymbol *bitsym;
  NameSymbol *namesym;
  ValueSymbol *valuesym;
  ValueMapSymbol *valuemapsym;
  ContextSymbol *contextsym;
  FamilySymbol *famsym;
  SpecificSymbol *specsym;


/* Line 387 of yacc.c  */
#line 281 "slghparse.tab.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_SLGHPARSE_TAB_HH_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 309 "slghparse.tab.cpp"

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
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2524

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  136
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  71
/* YYNRULES -- Number of rules.  */
#define YYNRULES  335
/* YYNRULES -- Number of states.  */
#define YYNSTATES  705

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   367

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,   135,    43,     2,     2,     2,    38,    11,     2,
     128,   129,    36,    32,   130,    33,     2,    37,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   134,     8,
      17,   127,    18,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   131,     2,   132,     9,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   133,     6,   126,    44,     2,     2,     2,
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
       5,     7,    10,    12,    13,    14,    15,    16,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    34,    35,    39,    40,    41,    42,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    11,    14,    16,    18,    20,
      22,    24,    26,    28,    30,    32,    35,    37,    39,    41,
      44,    50,    56,    62,    65,    72,    75,    79,    82,    86,
      89,    97,   105,   108,   111,   114,   122,   130,   133,   136,
     139,   142,   145,   149,   153,   158,   163,   168,   173,   176,
     187,   193,   198,   200,   203,   212,   217,   223,   229,   235,
     240,   247,   249,   252,   255,   258,   259,   261,   263,   264,
     266,   272,   276,   281,   283,   289,   295,   298,   301,   304,
     307,   310,   313,   316,   319,   322,   325,   328,   330,   333,
     335,   337,   339,   343,   347,   351,   355,   359,   363,   367,
     371,   375,   379,   382,   385,   387,   391,   395,   399,   402,
     404,   407,   409,   411,   415,   419,   423,   427,   431,   435,
     439,   443,   445,   447,   449,   451,   452,   456,   457,   463,
     472,   481,   487,   490,   494,   498,   501,   503,   507,   509,
     514,   520,   524,   529,   530,   533,   538,   545,   550,   556,
     561,   569,   576,   580,   586,   592,   602,   607,   612,   617,
     621,   627,   633,   639,   643,   649,   655,   659,   665,   668,
     674,   680,   682,   684,   687,   691,   695,   699,   703,   707,
     711,   715,   719,   723,   727,   731,   735,   739,   742,   745,
     749,   753,   757,   761,   765,   769,   773,   777,   781,   785,
     789,   792,   796,   800,   804,   808,   812,   816,   820,   824,
     828,   832,   836,   840,   844,   847,   852,   857,   862,   867,
     874,   881,   888,   893,   898,   903,   908,   913,   918,   923,
     928,   935,   940,   944,   951,   953,   958,   963,   970,   975,
     979,   981,   983,   985,   987,   989,   991,   996,   998,  1000,
    1002,  1004,  1006,  1008,  1010,  1012,  1016,  1019,  1024,  1026,
    1028,  1030,  1034,  1038,  1040,  1043,  1048,  1052,  1054,  1056,
    1058,  1060,  1062,  1064,  1066,  1068,  1070,  1072,  1074,  1076,
    1078,  1081,  1085,  1087,  1090,  1092,  1095,  1097,  1100,  1104,
    1107,  1111,  1113,  1115,  1118,  1121,  1125,  1127,  1129,  1132,
    1135,  1139,  1141,  1143,  1145,  1147,  1150,  1153,  1156,  1160,
    1162,  1164,  1166,  1169,  1172,  1173,  1175,  1179,  1180,  1182,
    1186,  1188,  1190,  1192,  1194,  1196,  1198,  1200,  1202,  1204,
    1206,  1208,  1210,  1212,  1214,  1216
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     137,     0,    -1,   140,    -1,   137,   141,    -1,   137,   138,
      -1,   137,   139,    -1,   142,    -1,   144,    -1,   148,    -1,
     150,    -1,   151,    -1,   154,    -1,   155,    -1,   156,    -1,
     157,    -1,     1,     8,    -1,   166,    -1,   158,    -1,   161,
      -1,     1,   126,    -1,    67,    75,   127,    84,     8,    -1,
      67,    75,   127,    85,     8,    -1,    67,    77,   127,   104,
       8,    -1,   143,     8,    -1,    67,    79,   106,   128,   104,
     129,    -1,   143,   146,    -1,    67,    79,   206,    -1,   145,
       8,    -1,    67,    99,   116,    -1,   145,   147,    -1,   106,
     127,   128,   104,   130,   104,   129,    -1,   206,   127,   128,
     104,   130,   104,   129,    -1,   146,    80,    -1,   146,    82,
      -1,   146,    83,    -1,   106,   127,   128,   104,   130,   104,
     129,    -1,   206,   127,   128,   104,   130,   104,   129,    -1,
     147,    80,    -1,   147,    81,    -1,   147,    82,    -1,   147,
      83,    -1,   149,     8,    -1,    67,    70,   106,    -1,    67,
      70,   206,    -1,   149,    71,   127,    72,    -1,   149,    71,
     127,    74,    -1,   149,    86,   127,   104,    -1,   149,    87,
     127,   104,    -1,   149,    73,    -1,    67,   108,    88,   127,
     104,    86,   127,   104,   196,     8,    -1,    67,   108,    88,
     127,    62,    -1,    67,   102,   152,     8,    -1,   153,    -1,
     152,   153,    -1,   106,   127,   116,   131,   104,   130,   104,
     132,    -1,    67,    92,   196,     8,    -1,    68,    90,   200,
     194,     8,    -1,    68,    89,   200,   198,     8,    -1,    68,
      91,   200,   202,     8,    -1,   164,   133,   180,   126,    -1,
      76,   162,   134,   163,   175,   133,    -1,   159,    -1,   160,
     138,    -1,   160,   139,    -1,   160,   126,    -1,    -1,   125,
      -1,   106,    -1,    -1,   170,    -1,    69,   106,   128,   205,
     129,    -1,   133,   180,   126,    -1,   133,   179,   181,   126,
      -1,    78,    -1,   167,    93,   170,   175,   165,    -1,   168,
      93,   170,   175,   165,    -1,   168,   106,    -1,   168,   193,
      -1,   168,   107,    -1,   168,     9,    -1,   167,     9,    -1,
     167,   106,    -1,   167,   193,    -1,   167,   135,    -1,   167,
     107,    -1,   125,   134,    -1,   106,   134,    -1,   134,    -1,
     168,   135,    -1,   105,    -1,   191,    -1,   192,    -1,   128,
     169,   129,    -1,   169,    32,   169,    -1,   169,    33,   169,
      -1,   169,    36,   169,    -1,   169,    31,   169,    -1,   169,
      30,   169,    -1,   169,    12,   169,    -1,   169,     7,   169,
      -1,   169,    10,   169,    -1,   169,    37,   169,    -1,    33,
     169,    -1,    44,   169,    -1,   171,    -1,   170,    11,   170,
      -1,   170,     6,   170,    -1,   170,     8,   170,    -1,   100,
     172,    -1,   172,    -1,   173,   100,    -1,   173,    -1,   174,
      -1,   128,   170,   129,    -1,   191,   127,   169,    -1,   191,
      15,   169,    -1,   191,    17,   169,    -1,   191,    27,   169,
      -1,   191,    18,   169,    -1,   191,    28,   169,    -1,   120,
     127,   169,    -1,   120,    -1,   118,    -1,   191,    -1,   125,
      -1,    -1,   131,   176,   132,    -1,    -1,   176,   114,   127,
     169,     8,    -1,   176,   101,   128,   191,   130,   114,   129,
       8,    -1,   176,   101,   128,   192,   130,   114,   129,     8,
      -1,   176,   120,   127,   169,     8,    -1,   176,   106,    -1,
      31,   106,    30,    -1,    31,   109,    30,    -1,   180,   177,
      -1,   178,    -1,   179,   181,   177,    -1,   181,    -1,   181,
      97,   190,     8,    -1,   181,    97,   184,   188,     8,    -1,
     181,    97,   106,    -1,   181,    97,   184,   106,    -1,    -1,
     181,   182,    -1,   181,    94,   106,     8,    -1,   181,    94,
     106,   134,   104,     8,    -1,   188,   127,   183,     8,    -1,
      94,   106,   127,   183,     8,    -1,   106,   127,   183,     8,
      -1,    94,   106,   134,   104,   127,   183,     8,    -1,   106,
     134,   104,   127,   183,     8,    -1,    94,   192,   127,    -1,
     184,   183,   127,   183,     8,    -1,   111,   128,   204,   129,
       8,    -1,   188,   131,   104,   130,   104,   132,   127,   183,
       8,    -1,   117,   127,   183,     8,    -1,   186,   134,   104,
     127,    -1,   186,   128,   104,   129,    -1,    98,   120,     8,
      -1,    96,   186,   130,   109,     8,    -1,    96,   186,   130,
     106,     8,    -1,    95,   128,   104,   129,     8,    -1,    63,
     185,     8,    -1,    66,   183,    63,   185,     8,    -1,    63,
     131,   183,   132,     8,    -1,    64,   185,     8,    -1,    64,
     131,   183,   132,     8,    -1,    65,     8,    -1,    65,   131,
     183,   132,     8,    -1,   123,   128,   204,   129,     8,    -1,
     189,    -1,   186,    -1,   184,   183,    -1,   128,   183,   129,
      -1,   183,    32,   183,    -1,   183,    33,   183,    -1,   183,
      16,   183,    -1,   183,    15,   183,    -1,   183,    17,   183,
      -1,   183,    28,   183,    -1,   183,    27,   183,    -1,   183,
      18,   183,    -1,   183,    26,   183,    -1,   183,    25,   183,
      -1,   183,    24,   183,    -1,   183,    23,   183,    -1,    33,
     183,    -1,    44,   183,    -1,   183,     9,   183,    -1,   183,
      11,   183,    -1,   183,     6,   183,    -1,   183,    31,   183,
      -1,   183,    30,   183,    -1,   183,    29,   183,    -1,   183,
      36,   183,    -1,   183,    37,   183,    -1,   183,    42,   183,
      -1,   183,    38,   183,    -1,   183,    41,   183,    -1,    43,
     183,    -1,   183,     4,   183,    -1,   183,     5,   183,    -1,
     183,     3,   183,    -1,   183,    14,   183,    -1,   183,    13,
     183,    -1,   183,    22,   183,    -1,   183,    21,   183,    -1,
     183,    20,   183,    -1,   183,    19,   183,    -1,   183,    35,
     183,    -1,   183,    34,   183,    -1,   183,    40,   183,    -1,
     183,    39,   183,    -1,    34,   183,    -1,    52,   128,   183,
     129,    -1,    53,   128,   183,   129,    -1,    48,   128,   183,
     129,    -1,    45,   128,   183,   129,    -1,    46,   128,   183,
     130,   183,   129,    -1,    49,   128,   183,   130,   183,   129,
      -1,    50,   128,   183,   130,   183,   129,    -1,    58,   128,
     183,   129,    -1,    57,   128,   183,   129,    -1,    51,   128,
     183,   129,    -1,    59,   128,   183,   129,    -1,    54,   128,
     183,   129,    -1,    55,   128,   183,   129,    -1,    56,   128,
     183,   129,    -1,    61,   128,   183,   129,    -1,    61,   128,
     183,   130,   183,   129,    -1,   192,   128,   187,   129,    -1,
     192,   134,   104,    -1,   192,   131,   104,   130,   104,   132,
      -1,   117,    -1,   111,   128,   204,   129,    -1,    60,   128,
     204,   129,    -1,    36,   131,   108,   132,   134,   104,    -1,
      36,   131,   108,   132,    -1,    36,   134,   104,    -1,    36,
      -1,   121,    -1,   122,    -1,   104,    -1,    62,    -1,   120,
      -1,   104,   131,   108,   132,    -1,   189,    -1,   106,    -1,
     192,    -1,   187,    -1,   106,    -1,   125,    -1,   104,    -1,
      62,    -1,   104,   134,   104,    -1,    11,   186,    -1,    11,
     134,   104,   186,    -1,   192,    -1,   106,    -1,   125,    -1,
      17,   124,    18,    -1,    17,   106,    18,    -1,   192,    -1,
      11,   186,    -1,    11,   134,   104,   186,    -1,   104,   134,
     104,    -1,   106,    -1,   125,    -1,   112,    -1,   113,    -1,
     114,    -1,   115,    -1,   119,    -1,   116,    -1,   118,    -1,
     120,    -1,   121,    -1,   122,    -1,   103,    -1,   193,   103,
      -1,   131,   195,   132,    -1,   104,    -1,    33,   104,    -1,
     104,    -1,    33,   104,    -1,   106,    -1,   195,   104,    -1,
     195,    33,   104,    -1,   195,   106,    -1,   131,   197,   132,
      -1,   106,    -1,   106,    -1,   197,   106,    -1,   197,   206,
      -1,   131,   199,   132,    -1,   106,    -1,   206,    -1,   199,
     106,    -1,   199,   206,    -1,   131,   201,   132,    -1,   112,
      -1,   114,    -1,   112,    -1,   114,    -1,   201,   112,    -1,
     201,   114,    -1,   201,   106,    -1,   131,   203,   132,    -1,
     116,    -1,   116,    -1,   106,    -1,   203,   116,    -1,   203,
     106,    -1,    -1,   183,    -1,   204,   130,   183,    -1,    -1,
     106,    -1,   205,   130,   106,    -1,   108,    -1,   109,    -1,
     110,    -1,   111,    -1,   123,    -1,   125,    -1,   112,    -1,
     113,    -1,   114,    -1,   115,    -1,   116,    -1,   119,    -1,
     120,    -1,   121,    -1,   122,    -1,   117,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   156,   156,   157,   158,   159,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   172,   173,   174,   175,
     177,   178,   180,   182,   184,   185,   186,   188,   190,   191,
     194,   195,   196,   197,   198,   200,   201,   202,   203,   204,
     205,   207,   209,   210,   211,   212,   213,   214,   215,   217,
     219,   221,   223,   224,   226,   229,   231,   233,   235,   237,
     240,   242,   243,   244,   246,   248,   249,   250,   253,   254,
     257,   259,   260,   261,   263,   264,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   276,   277,   278,   279,   281,
     283,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   300,   301,   302,   303,   305,   306,
     308,   309,   311,   312,   314,   315,   316,   317,   318,   319,
     320,   323,   324,   325,   326,   328,   329,   331,   332,   333,
     334,   335,   336,   338,   339,   341,   343,   344,   346,   347,
     348,   349,   350,   352,   353,   354,   355,   357,   358,   359,
     360,   361,   362,   363,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   373,   374,   375,   376,   377,   378,   379,
     380,   381,   383,   384,   385,   386,   387,   388,   389,   390,
     391,   392,   393,   394,   395,   396,   397,   398,   399,   400,
     401,   402,   403,   404,   405,   406,   407,   408,   409,   410,
     411,   412,   413,   414,   415,   416,   417,   418,   419,   420,
     421,   422,   423,   424,   425,   426,   427,   428,   429,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   449,   450,   451,
     452,   454,   455,   456,   457,   458,   459,   460,   461,   463,
     464,   465,   466,   468,   469,   470,   471,   472,   474,   475,
     476,   478,   479,   481,   482,   483,   484,   485,   486,   488,
     489,   490,   491,   492,   494,   495,   496,   497,   498,   500,
     501,   503,   504,   505,   507,   508,   509,   511,   512,   513,
     516,   517,   519,   520,   521,   523,   525,   526,   527,   528,
     530,   531,   532,   534,   535,   536,   537,   538,   540,   541,
     543,   544,   546,   547,   550,   551,   552,   554,   555,   556,
     558,   559,   560,   561,   562,   563,   564,   565,   566,   567,
     568,   569,   570,   571,   572,   573
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OP_BOOL_OR", "OP_BOOL_XOR",
  "OP_BOOL_AND", "'|'", "OP_OR", "';'", "'^'", "OP_XOR", "'&'", "OP_AND",
  "OP_FNOTEQUAL", "OP_FEQUAL", "OP_NOTEQUAL", "OP_EQUAL", "'<'", "'>'",
  "OP_FGREATEQUAL", "OP_FLESSEQUAL", "OP_FGREAT", "OP_FLESS", "OP_SGREAT",
  "OP_SLESSEQUAL", "OP_SGREATEQUAL", "OP_SLESS", "OP_LESSEQUAL",
  "OP_GREATEQUAL", "OP_SRIGHT", "OP_RIGHT", "OP_LEFT", "'+'", "'-'",
  "OP_FSUB", "OP_FADD", "'*'", "'/'", "'%'", "OP_FDIV", "OP_FMULT",
  "OP_SREM", "OP_SDIV", "'!'", "'~'", "OP_ZEXT", "OP_CARRY", "OP_BORROW",
  "OP_SEXT", "OP_SCARRY", "OP_SBORROW", "OP_NAN", "OP_ABS", "OP_SQRT",
  "OP_CEIL", "OP_FLOOR", "OP_ROUND", "OP_INT2FLOAT", "OP_FLOAT2FLOAT",
  "OP_TRUNC", "OP_CPOOLREF", "OP_NEW", "BADINTEGER", "GOTO_KEY",
  "CALL_KEY", "RETURN_KEY", "IF_KEY", "DEFINE_KEY", "ATTACH_KEY",
  "MACRO_KEY", "SPACE_KEY", "TYPE_KEY", "RAM_KEY", "DEFAULT_KEY",
  "REGISTER_KEY", "ENDIAN_KEY", "WITH_KEY", "ALIGN_KEY", "OP_UNIMPL",
  "TOKEN_KEY", "SIGNED_KEY", "NOFLOW_KEY", "HEX_KEY", "DEC_KEY", "BIG_KEY",
  "LITTLE_KEY", "SIZE_KEY", "WORDSIZE_KEY", "OFFSET_KEY", "NAMES_KEY",
  "VALUES_KEY", "VARIABLES_KEY", "PCODEOP_KEY", "IS_KEY", "LOCAL_KEY",
  "DELAYSLOT_KEY", "CROSSBUILD_KEY", "EXPORT_KEY", "BUILD_KEY",
  "CONTEXT_KEY", "ELLIPSIS_KEY", "GLOBALSET_KEY", "BITRANGE_KEY", "CHAR",
  "INTEGER", "INTB", "STRING", "SYMBOLSTRING", "SPACESYM", "SECTIONSYM",
  "TOKENSYM", "USEROPSYM", "VALUESYM", "VALUEMAPSYM", "CONTEXTSYM",
  "NAMESYM", "VARSYM", "BITSYM", "SPECSYM", "VARLISTSYM", "OPERANDSYM",
  "STARTSYM", "ENDSYM", "MACROSYM", "LABELSYM", "SUBTABLESYM", "'}'",
  "'='", "'('", "')'", "','", "'['", "']'", "'{'", "':'", "' '", "$accept",
  "spec", "definition", "constructorlike", "endiandef", "aligndef",
  "tokendef", "tokenprop", "contextdef", "contextprop", "fielddef",
  "contextfielddef", "spacedef", "spaceprop", "varnodedef", "bitrangedef",
  "bitrangelist", "bitrangesingle", "pcodeopdef", "valueattach",
  "nameattach", "varattach", "macrodef", "withblockstart", "withblockmid",
  "withblock", "id_or_nil", "bitpat_or_nil", "macrostart", "rtlbody",
  "constructor", "constructprint", "subtablestart", "pexpression",
  "pequation", "elleq", "ellrt", "atomic", "constraint", "contextblock",
  "contextlist", "section_def", "rtlfirstsection", "rtlcontinue", "rtl",
  "rtlmid", "statement", "expr", "sizedstar", "jumpdest", "varnode",
  "integervarnode", "lhsvarnode", "label", "exportvarnode", "familysymbol",
  "specificsymbol", "charstring", "intblist", "intbpart", "stringlist",
  "stringpart", "anystringlist", "anystringpart", "valuelist", "valuepart",
  "varlist", "varpart", "paramlist", "oplist", "anysymbol", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   124,   261,    59,    94,
     262,    38,   263,   264,   265,   266,   267,    60,    62,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,    43,    45,   281,   282,    42,    47,    37,   283,
     284,   285,   286,    33,   126,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     342,   343,   344,   345,   346,   347,   348,   349,   350,   351,
     352,   353,   354,   355,   356,   357,   358,   359,   360,   361,
     362,   363,   364,   365,   366,   367,   125,    61,    40,    41,
      44,    91,    93,   123,    58,    32
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   136,   137,   137,   137,   137,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   139,   139,   139,   139,
     140,   140,   141,   142,   143,   143,   143,   144,   145,   145,
     146,   146,   146,   146,   146,   147,   147,   147,   147,   147,
     147,   148,   149,   149,   149,   149,   149,   149,   149,   150,
     150,   151,   152,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   160,   160,   161,   162,   162,   162,   163,   163,
     164,   165,   165,   165,   166,   166,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   168,   168,   168,   168,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   170,   170,   170,   170,   171,   171,
     172,   172,   173,   173,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   175,   175,   176,   176,   176,
     176,   176,   176,   177,   177,   178,   179,   179,   180,   180,
     180,   180,   180,   181,   181,   181,   181,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   184,   184,   184,
     184,   185,   185,   185,   185,   185,   185,   185,   185,   186,
     186,   186,   186,   187,   187,   187,   187,   187,   188,   188,
     188,   189,   189,   190,   190,   190,   190,   190,   190,   191,
     191,   191,   191,   191,   192,   192,   192,   192,   192,   193,
     193,   194,   194,   194,   195,   195,   195,   195,   195,   195,
     196,   196,   197,   197,   197,   198,   199,   199,   199,   199,
     200,   200,   200,   201,   201,   201,   201,   201,   202,   202,
     203,   203,   203,   203,   204,   204,   204,   205,   205,   205,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     2,
       5,     5,     5,     2,     6,     2,     3,     2,     3,     2,
       7,     7,     2,     2,     2,     7,     7,     2,     2,     2,
       2,     2,     3,     3,     4,     4,     4,     4,     2,    10,
       5,     4,     1,     2,     8,     4,     5,     5,     5,     4,
       6,     1,     2,     2,     2,     0,     1,     1,     0,     1,
       5,     3,     4,     1,     5,     5,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     2,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     1,     3,     3,     3,     2,     1,
       2,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     1,     1,     1,     0,     3,     0,     5,     8,
       8,     5,     2,     3,     3,     2,     1,     3,     1,     4,
       5,     3,     4,     0,     2,     4,     6,     4,     5,     4,
       7,     6,     3,     5,     5,     9,     4,     4,     4,     3,
       5,     5,     5,     3,     5,     5,     3,     5,     2,     5,
       5,     1,     1,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     4,     4,     4,     4,     6,
       6,     6,     4,     4,     4,     4,     4,     4,     4,     4,
       6,     4,     3,     6,     1,     4,     4,     6,     4,     3,
       1,     1,     1,     1,     1,     1,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     4,     1,     1,
       1,     3,     3,     1,     2,     4,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     1,     2,     1,     2,     1,     2,     3,     2,
       3,     1,     1,     2,     2,     3,     1,     1,     2,     2,
       3,     1,     1,     1,     1,     2,     2,     2,     3,     1,
       1,     1,     2,     2,     0,     1,     3,     0,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,     2,     0,     1,     0,     0,     0,     0,
      65,     0,     0,    87,     4,     5,     3,     6,     0,     7,
       0,     8,     0,     9,    10,    11,    12,    13,    14,    17,
      61,     0,    18,     0,    16,     0,     0,     0,    15,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    67,    66,     0,    86,    85,    23,     0,   320,   321,
     322,   323,   326,   327,   328,   329,   330,   335,   331,   332,
     333,   334,   324,   325,    25,     0,    27,     0,    29,     0,
      41,     0,    48,     0,     0,     0,    64,    62,    63,   143,
      80,     0,   279,    81,    84,    83,    82,    79,     0,    76,
      78,    88,    77,     0,     0,    42,    43,     0,     0,    26,
     291,     0,     0,    28,     0,     0,    52,     0,   301,   302,
       0,     0,     0,     0,   317,    68,     0,    32,    33,    34,
       0,     0,    37,    38,    39,    40,     0,     0,     0,     0,
       0,   138,     0,   269,   270,   271,   272,   122,   273,   121,
     124,     0,   125,   104,   109,   111,   112,   123,   280,   125,
      20,    21,     0,     0,   292,     0,    55,     0,    51,    53,
       0,   303,   304,     0,     0,     0,     0,   282,     0,     0,
     309,     0,     0,   318,     0,   125,    69,     0,     0,     0,
       0,    44,    45,    46,    47,    59,     0,     0,   240,   254,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   253,
     251,     0,   274,     0,   275,   276,   277,   278,     0,   252,
     144,     0,     0,   250,     0,   171,   249,   108,     0,     0,
       0,     0,     0,   127,     0,   110,     0,     0,     0,     0,
       0,     0,     0,    22,     0,   293,   290,   294,     0,    50,
       0,   307,   305,   306,   300,   296,     0,   297,    57,   283,
       0,   284,   286,     0,    56,   311,   310,     0,    58,    70,
       0,     0,     0,     0,     0,     0,   251,   252,     0,   256,
     249,     0,     0,     0,     0,   244,   243,   248,   245,   241,
     242,     0,     0,   247,     0,     0,   168,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   234,
       0,     0,     0,   172,   249,     0,     0,     0,     0,     0,
       0,   141,   268,     0,     0,   263,     0,     0,     0,     0,
     314,     0,   314,     0,     0,     0,     0,     0,     0,     0,
      89,     0,   120,    90,    91,   113,   106,   107,   105,     0,
      73,   143,    74,   115,   116,   118,   117,   119,   114,    75,
      24,     0,     0,   298,   295,   299,   285,     0,   287,   289,
     281,   313,   312,   308,   319,    60,     0,     0,     0,     0,
       0,   262,   261,     0,   239,     0,     0,   163,     0,   166,
       0,   187,   214,   200,   188,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   314,
       0,   314,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     173,     0,     0,     0,   145,     0,     0,   152,     0,     0,
       0,   264,     0,   142,   260,     0,   258,   139,   159,   255,
       0,     0,   315,     0,     0,     0,     0,     0,     0,     0,
       0,   102,   103,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   132,     0,     0,   126,   136,   143,
       0,     0,     0,   288,     0,     0,     0,     0,   257,   238,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   174,   203,   201,   202,   191,   189,   190,   205,   204,
     178,   177,   179,   182,   209,   208,   207,   206,   186,   185,
     184,   183,   181,   180,   194,   193,   192,   175,   176,   211,
     210,   195,   196,   198,   213,   212,   199,   197,     0,     0,
       0,   232,     0,     0,     0,     0,     0,     0,   266,   140,
     149,     0,     0,     0,   156,     0,     0,   158,   157,   147,
       0,    92,    99,   100,    98,    97,    96,    93,    94,    95,
     101,     0,     0,     0,     0,     0,    71,   135,     0,     0,
      30,    31,    35,    36,     0,   246,   165,   167,   169,   218,
       0,   217,     0,     0,   224,   215,   216,   226,   227,   228,
     223,   222,   225,   236,   229,     0,   235,   164,   231,     0,
     148,   146,     0,   162,   161,   160,   265,     0,   154,   316,
     170,   153,     0,     0,     0,     0,     0,    72,   137,     0,
       0,     0,     0,   237,     0,     0,     0,     0,     0,     0,
     151,     0,     0,     0,   128,   131,   133,   134,    54,    49,
     219,   220,   221,   230,   233,   150,     0,     0,     0,     0,
       0,     0,   155,   129,   130
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,    14,    15,     3,    16,    17,    18,    19,    20,
      74,    78,    21,    22,    23,    24,   115,   116,    25,    26,
      27,    28,    29,    30,    31,    32,    53,   185,    33,   362,
      34,    35,    36,   352,   152,   153,   154,   155,   156,   234,
     359,   617,   508,   509,   140,   141,   220,   482,   322,   292,
     323,   223,   224,   293,   334,   353,   324,    96,   179,   263,
     112,   165,   175,   256,   121,   173,   182,   267,   483,   184,
      75
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -327
static const yytype_int16 yypact[] =
{
     -19,   -23,    10,  -327,   -39,  -327,     4,  1555,   303,    13,
     -57,    -8,    43,  -327,  -327,  -327,  -327,  -327,   370,  -327,
     396,  -327,   301,  -327,  -327,  -327,  -327,  -327,  -327,  -327,
    -327,    22,  -327,     5,  -327,    30,    65,   -67,  -327,  -327,
    2363,    80,  2381,   -73,    33,    93,   201,   -50,   -50,   -50,
     182,  -327,  -327,   181,  -327,  -327,  -327,   205,  -327,  -327,
    -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,
    -327,  -327,  -327,  -327,   300,   207,  -327,   242,   240,   244,
    -327,   250,  -327,   270,   280,  1543,  -327,  -327,  -327,  -327,
    -327,  2228,  -327,  -327,  -327,  -327,   307,  -327,  2228,  -327,
    -327,  -327,   307,   409,   412,  -327,  -327,   320,   304,  -327,
    -327,   322,   423,  -327,   306,     6,  -327,   309,  -327,  -327,
     178,   308,   -20,   -31,   343,  2228,   323,  -327,  -327,  -327,
     325,   327,  -327,  -327,  -327,  -327,   330,   -34,   355,   356,
     337,  1619,  1394,  -327,  -327,  -327,  -327,  -327,  -327,   338,
    -327,  2228,    14,  -327,  -327,   372,  -327,    48,  -327,    14,
    -327,  -327,   461,   373,  -327,  2259,  -327,   354,  -327,  -327,
     -43,  -327,  -327,   -82,  2399,   465,   390,  -327,    75,   488,
    -327,   -75,   489,  -327,     2,   368,   266,   399,   410,   419,
     425,  -327,  -327,  -327,  -327,  -327,    72,    -9,   -13,  -327,
     264,  1395,    28,  1487,  1580,   403,   305,   418,   380,   393,
     -17,   404,  -327,   408,  -327,  -327,  -327,  -327,   413,   -58,
    -327,  1487,   -21,  -327,    64,  -327,   138,  -327,  1461,   190,
    2228,  2228,  2228,  -327,   -74,  -327,  1461,  1461,  1461,  1461,
    1461,  1461,   -74,  -327,   415,  -327,  -327,  -327,   406,  -327,
     456,  -327,  -327,  -327,  -327,  -327,  2284,  -327,  -327,  -327,
     441,  -327,  -327,   -12,  -327,  -327,  -327,   -79,  -327,  -327,
     440,   414,   422,   426,   435,   436,  -327,  -327,   449,  -327,
    -327,   550,   552,   497,   502,  -327,   477,  -327,  -327,  -327,
    -327,  1487,   602,  -327,  1487,   637,  -327,  1487,  1487,  1487,
    1487,  1487,   520,   522,   557,   560,   597,   600,   633,   634,
     642,   644,   679,   682,   684,   719,   722,   724,   759,  -327,
    1487,  1742,  1487,  -327,    36,    -5,   519,   582,   596,   162,
     754,   884,  -327,   346,   915,  -327,   920,   827,  1487,   829,
    1487,  1487,  1487,  1445,   864,   867,  1487,   869,  1461,  1461,
    -327,  1461,  1657,  -327,  -327,  -327,   158,   997,  -327,   302,
    -327,  -327,  -327,  1657,  1657,  1657,  1657,  1657,  1657,  -327,
    -327,   907,   886,  -327,  -327,  -327,  -327,   940,  -327,  -327,
    -327,  -327,  -327,  -327,  -327,  -327,   945,   948,   950,   985,
     305,  -327,  -327,   960,  -327,   986,   324,  -327,   558,  -327,
     598,  -327,  -327,  -327,  -327,  1487,  1487,  1487,  1487,  1487,
    1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,
    1487,  1487,   800,  1487,  1487,  1487,  1487,  1487,  1487,  1487,
    1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,
    1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,
    1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,  1487,   269,
    -327,    98,  1025,  1028,  -327,  1487,  1030,  -327,  1036,   167,
    1066,  -327,  1069,  1167,  -327,  1202,  -327,  -327,  -327,  -327,
    1793,  1086,  2113,    97,  1833,   169,  1487,  1121,  1088,  1873,
    1123,  -327,  -327,   369,  1461,  1461,  1461,  1461,  1461,  1461,
    1461,  1461,  1461,  1127,  -327,  1159,  1164,  -327,  -327,  -327,
     -25,  1166,  1190,  -327,  1205,  1207,  1242,  1245,  -327,  1197,
    1244,  1399,  1407,  1409,   840,   678,   880,   718,   760,   921,
     961,  1001,  1042,  1082,  1122,  1163,  1203,  1243,   195,   638,
     283,  -327,  2152,  2189,  2189,  2223,  2255,  2285,  2400,  2400,
    2400,  2400,  2426,  2426,  2426,  2426,  2426,  2426,  2426,  2426,
    2426,  2426,  2426,  2426,  1518,  1518,  1518,  2293,  2293,  2293,
    2293,  -327,  -327,  -327,  -327,  -327,  -327,  -327,  1444,  1326,
    1358,  -327,  1913,    -3,  1489,  1492,  1494,   305,  -327,  -327,
    -327,  1487,  1495,  1487,  -327,  1496,  1953,  -327,  -327,  -327,
    1406,  -327,  2327,   281,  1459,   172,   172,    35,    35,  -327,
    -327,  1673,  1461,  1461,  1554,   179,  -327,  -327,  1414,   -73,
    -327,  -327,  -327,  -327,  1420,  -327,  -327,  -327,  -327,  -327,
    1487,  -327,  1487,  1487,  -327,  -327,  -327,  -327,  -327,  -327,
    -327,  -327,  -327,  -327,  -327,  1487,  -327,  -327,  -327,  1421,
    -327,  -327,  1487,  -327,  -327,  -327,  -327,  1993,  -327,  2113,
    -327,  -327,  1379,  1397,  1398,   518,  1696,  -327,  -327,  1499,
    1531,  1430,  1556,  -327,  1284,  1324,  1364,  1405,  1431,  2033,
    -327,  1440,  1454,  1455,  -327,  -327,  -327,  -327,  -327,  -327,
    -327,  -327,  -327,  -327,  -327,  -327,  1487,  1441,  1449,  2073,
    1576,  1578,  -327,  -327,  -327
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -327,  -327,  1557,  1561,  -327,  -327,  -327,  -327,  -327,  -327,
    -327,  -327,  -327,  -327,  -327,  -327,  -327,  1472,  -327,  -327,
    -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,  1352,
    -327,  -327,  -327,  -194,   -69,  -327,  1453,  -327,  -327,  -131,
    -327,   982,  -327,  -327,  1236,  1090,  -327,  -195,  -140,  -192,
    -126,  1139,  1268,  -139,  -327,   -91,   -54,  1566,  -327,  -327,
     987,  -327,  -327,  -327,   366,  -327,  -327,  -327,  -326,  -327,
      15
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -268
static const yytype_int16 yytable[] =
{
     157,   221,   225,   464,   360,   651,   615,   157,   321,   295,
       5,     6,    38,   176,   168,   222,   485,   103,   104,   249,
     230,   377,   231,     6,   251,   232,   343,   381,   242,   159,
     252,   265,   253,   110,   157,    79,   296,   382,   191,    90,
     192,   266,   363,   364,   365,   366,   367,   368,     1,    51,
     254,   157,     4,   383,   271,   106,   186,   109,   111,   361,
     157,   250,   118,   236,   119,   237,   238,   333,    52,  -260,
     279,   501,   502,  -260,    97,   239,   240,     7,     8,     9,
     328,   120,   229,   196,   177,   180,    10,   226,    37,    85,
       8,     9,   378,   538,   379,   540,   396,   281,    10,   398,
     181,   616,   400,   401,   402,   403,   404,   344,   260,   196,
     338,   178,   114,   345,  -259,   282,    11,   339,   283,    50,
     380,   284,   465,    91,   652,   422,    54,   460,    11,   466,
      39,   269,   270,    92,   199,    12,    93,    94,    89,   157,
     157,   157,   280,   480,    13,   233,   484,    12,    86,   113,
     326,   489,   280,   335,   491,   492,    13,   493,    98,   297,
     199,   356,   357,   358,   461,    95,   231,   462,    92,   232,
     463,    99,   100,   196,   354,   241,   209,    55,   276,   261,
     247,   262,   354,   354,   354,   354,   354,   354,   212,   257,
     214,   346,   215,   216,   217,   347,   230,   277,   231,   114,
     101,   232,   209,   471,   499,   500,   278,   107,   501,   502,
     524,   525,   526,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   199,   539,   592,   593,   542,   543,
     544,   545,   546,   547,   548,   549,   550,   551,   552,   553,
     554,   555,   556,   557,   558,   559,   560,   561,   562,   563,
     564,   565,   566,   567,   568,   569,   570,   571,   572,   573,
     574,   575,   576,   577,   518,  -258,   209,   578,   276,  -258,
     582,   375,   230,   585,   231,   280,   586,   232,   212,   476,
     214,   197,   215,   216,   217,   669,   197,   277,   670,   117,
     171,   596,   172,   496,   354,   354,   470,   354,   595,   593,
     602,   603,   604,   605,   606,   607,   608,   609,   610,    80,
     124,   497,   498,   499,   500,   125,   196,   501,   502,   355,
     132,   133,   134,   135,   643,   593,   285,   423,   424,   425,
     426,   285,   126,   427,   130,   428,   280,   429,   430,   431,
     432,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   453,   454,   455,   456,   457,   458,   199,   286,   131,
     287,   136,    81,   286,    82,   287,   494,   137,    56,   495,
     127,   496,   128,   129,   288,   289,   290,    83,    84,   288,
     289,   290,    47,    48,    49,   291,   657,   138,   659,   497,
     498,   499,   500,   503,    76,   501,   502,   139,   504,   209,
     158,   276,   646,   593,   122,   123,   505,   160,   665,   666,
     161,   212,   506,   214,   162,   215,   216,   217,   164,   329,
     277,   166,   163,   167,   507,   674,   170,   675,   676,   174,
     354,   354,   354,   354,   354,   354,   354,   354,   354,   183,
     677,   187,   473,   188,   198,   189,   521,   679,   190,   193,
     194,   656,   212,   195,   214,   228,   215,   216,   217,   243,
     248,   474,   235,   258,   221,   225,    57,   244,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,   222,    68,
      69,    70,    71,    72,   259,    73,   264,   268,   601,   233,
     336,   699,    77,   272,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,   273,    68,    69,    70,    71,    72,
     663,    73,   330,   274,   331,   494,   684,   337,   495,   275,
     496,   327,   340,   280,   212,   341,   214,   371,   215,   216,
     217,   342,   372,   332,   370,   376,   384,   385,   497,   498,
     499,   500,   386,   390,   501,   502,   387,   664,   354,   354,
     226,   423,   424,   425,   426,   388,   389,   427,   391,   428,
     392,   429,   430,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   423,   424,   425,   426,   393,   394,   427,   395,   428,
     397,   429,   430,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   423,   424,   425,   426,   399,   467,   427,   405,   428,
     406,   429,   430,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   423,   424,   425,   426,   407,   468,   427,   408,   428,
     522,   429,   430,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   423,   424,   425,   426,   409,   469,   427,   410,   428,
     523,   429,   430,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   411,   412,   423,   424,   425,   426,   644,   645,   427,
     413,   428,   414,   429,   430,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   423,   424,   425,   426,   415,   630,   427,
     416,   428,   417,   429,   430,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   423,   424,   425,   426,   418,   632,   427,
     419,   428,   420,   429,   430,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   423,   424,   425,   426,   421,   472,   427,
     633,   428,  -267,   429,   430,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   477,   423,   424,   425,   426,   478,   541,
     427,   479,   428,   481,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   423,   424,   425,   426,   487,   629,
     427,   488,   428,   490,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   423,   424,   425,   426,   232,   631,
     427,   511,   428,   512,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   513,   423,   424,   425,   426,   514,
     634,   427,   515,   428,   516,   429,   430,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   423,   424,   425,   426,   517,
     635,   427,   519,   428,   520,   429,   430,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   423,   424,   425,   426,   580,
     636,   427,   581,   428,   583,   429,   430,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   584,   423,   424,   425,   426,
     587,   637,   427,   588,   428,  -259,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   423,   424,   425,   426,
     589,   638,   427,   591,   428,   598,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   423,   424,   425,   426,
     597,   639,   427,   600,   428,   611,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   612,   423,   424,   425,
     426,   613,   640,   427,   619,   428,   618,   429,   430,   431,
     432,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   453,   454,   455,   456,   457,   458,   423,   424,   425,
     426,   624,   641,   427,   620,   428,   621,   429,   430,   431,
     432,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   453,   454,   455,   456,   457,   458,   423,   424,   425,
     426,   622,   642,   427,   623,   428,   625,   429,   430,   431,
     432,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   453,   454,   455,   456,   457,   458,   626,   423,   424,
     425,   426,   197,   690,   427,   627,   428,   628,   429,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   423,   424,
     425,   426,   647,   691,   427,   648,   428,   285,   429,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   649,   497,
     498,   499,   500,   692,   348,   501,   502,   653,   196,   286,
     654,   287,   655,   658,   660,   349,   143,   144,   145,   146,
     662,   681,   147,   148,   149,   288,   289,   290,   671,   150,
     298,   299,   151,   198,   673,   678,   294,   682,   683,   686,
     300,   301,   302,   303,   693,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   199,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   687,   688,   694,   689,   196,   350,   696,   697,   698,
     700,   197,   486,   143,   144,   145,   146,   212,   701,   214,
     148,   215,   216,   217,   703,   615,   704,   169,    87,   351,
     198,   209,    88,   276,   369,   227,   668,   510,   318,   614,
     579,   475,   102,   212,   319,   214,   672,   215,   216,   217,
       0,     0,   277,    40,     0,   320,   199,   200,   201,   202,
     203,     0,    42,     0,     0,    40,     0,     0,     0,     0,
     196,     0,    41,     0,    42,    43,   197,     0,     0,     0,
       0,     0,    44,     0,     0,    45,     0,    43,   204,   205,
     206,    46,   208,     0,    44,   198,     0,    45,   209,     0,
     210,     0,     0,    46,   494,   211,     0,   495,     0,   496,
     212,   213,   214,     0,   215,   216,   217,   218,     0,   219,
     667,   199,   200,   201,   202,   203,   325,   497,   498,   499,
     500,     0,     0,   501,   502,     0,   212,     0,   214,     0,
     215,   216,   217,   494,   685,     0,   495,     0,   496,     0,
       0,     0,     0,   204,   205,   206,   207,   208,     0,     0,
       0,     0,     0,   209,     0,   210,   497,   498,   499,   500,
     211,     0,   501,   502,     0,   212,   213,   214,     0,   215,
     216,   217,   218,     0,   219,   423,   424,   425,   426,     0,
       0,   427,     0,   428,     0,   429,   430,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   143,   144,   145,   146,   212,
       0,   214,   148,   215,   216,   217,   423,   424,   425,   426,
       0,   590,   427,     0,   428,   459,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   423,   424,   425,   426,
       0,   594,   427,     0,   428,     0,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   423,   424,   425,   426,
       0,   599,   427,     0,   428,     0,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   423,   424,   425,   426,
       0,   650,   427,     0,   428,     0,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   423,   424,   425,   426,
       0,   661,   427,     0,   428,     0,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   423,   424,   425,   426,
       0,   680,   427,     0,   428,     0,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   423,   424,   425,   426,
       0,   695,   427,     0,   428,     0,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   423,   424,   425,   426,
       0,   702,   427,     0,   428,     0,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   423,   424,   425,   426,
       0,     0,   427,     0,   428,     0,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   424,   425,   426,     0,
       0,   427,     0,   428,     0,   429,   430,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   426,     0,     0,   427,     0,
     428,     0,   429,   430,   431,   432,   433,   434,   435,   436,
     437,   438,   439,   440,   441,   442,   443,   444,   445,   446,
     447,   448,   449,   450,   451,   452,   453,   454,   455,   456,
     457,   458,   427,     0,   428,     0,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   428,     0,   429,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   429,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   142,   452,
     453,   454,   455,   456,   457,   458,     0,   495,     0,   496,
     143,   144,   145,   146,     0,     0,   147,   148,   149,     0,
       0,     0,     0,   150,     0,     0,   151,   497,   498,   499,
     500,     0,     0,   501,   502,   245,     0,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,    69,
      70,    71,    72,     0,    73,     0,     0,     0,     0,     0,
     373,   246,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,    69,    70,    71,    72,     0,    73,
       0,     0,     0,     0,     0,     0,   374,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,  -268,  -268,  -268,  -268,  -268,  -268,  -268,
    -268,  -268,  -268,  -268,  -268,   445,   446,   447,   448,   449,
     450,   451,   452,   453,   454,   455,   456,   457,   458,   105,
       0,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,    69,    70,    71,    72,   108,    73,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,    69,    70,    71,    72,   255,    73,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,    69,
      70,    71,    72,     0,    73
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-327)))

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-268)))

static const yytype_int16 yycheck[] =
{
      91,   141,   141,     8,    78,     8,    31,    98,   203,   201,
       0,     1,     8,    33,     8,   141,   342,    84,    85,    62,
       6,    33,     8,     1,   106,    11,   221,   106,   159,    98,
     112,   106,   114,   106,   125,    20,     8,   116,    72,     9,
      74,   116,   236,   237,   238,   239,   240,   241,    67,   106,
     132,   142,    75,   132,   185,    40,   125,    42,   131,   133,
     151,   104,   112,    15,   114,    17,    18,   207,   125,   127,
     196,    36,    37,   131,     9,    27,    28,    67,    68,    69,
     206,   131,   151,    11,   104,   116,    76,   141,   127,    67,
      68,    69,   104,   419,   106,   421,   291,   106,    76,   294,
     131,   126,   297,   298,   299,   300,   301,   128,    33,    11,
     127,   131,   106,   134,   131,   124,   106,   134,   131,   106,
     132,   134,   127,    93,   127,   320,   134,   322,   106,   134,
     126,   129,   130,   103,    62,   125,   106,   107,   133,   230,
     231,   232,   196,   338,   134,   131,   341,   125,   126,   116,
     204,   346,   206,   207,   348,   349,   134,   351,    93,   131,
      62,   230,   231,   232,   128,   135,     8,   131,   103,    11,
     134,   106,   107,    11,   228,   127,   104,   134,   106,   104,
     165,   106,   236,   237,   238,   239,   240,   241,   116,   174,
     118,   127,   120,   121,   122,   131,     6,   125,     8,   106,
     135,    11,   104,   329,    32,    33,   134,   127,    36,    37,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,    62,   420,   129,   130,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   390,   127,   104,   459,   106,   131,
     465,   256,     6,   106,     8,   329,   109,    11,   116,   333,
     118,    17,   120,   121,   122,   106,    17,   125,   109,    88,
     112,   486,   114,    12,   348,   349,   134,   351,   129,   130,
     494,   495,   496,   497,   498,   499,   500,   501,   502,     8,
     128,    30,    31,    32,    33,   134,    11,    36,    37,   129,
      80,    81,    82,    83,   129,   130,    62,     3,     4,     5,
       6,    62,   127,     9,   127,    11,   390,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    62,   104,   127,
     106,   127,    71,   104,    73,   106,     7,   127,     8,    10,
      80,    12,    82,    83,   120,   121,   122,    86,    87,   120,
     121,   122,    89,    90,    91,   131,   591,   127,   593,    30,
      31,    32,    33,   101,     8,    36,    37,   127,   106,   104,
     103,   106,   129,   130,    48,    49,   114,     8,   612,   613,
       8,   116,   120,   118,   104,   120,   121,   122,   106,    11,
     125,     8,   128,   127,   132,   630,   127,   632,   633,   131,
     494,   495,   496,   497,   498,   499,   500,   501,   502,   106,
     645,   128,   106,   128,    36,   128,   132,   652,   128,   104,
     104,   587,   116,   126,   118,   127,   120,   121,   122,     8,
     116,   125,   100,     8,   614,   614,   106,   104,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   614,   119,
     120,   121,   122,   123,   104,   125,     8,     8,   129,   131,
     120,   696,   106,   104,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   104,   119,   120,   121,   122,   123,
     611,   125,   104,   104,   106,     7,     8,   134,    10,   104,
      12,   128,   128,   587,   116,   127,   118,   131,   120,   121,
     122,   128,    86,   125,   129,   104,   106,   133,    30,    31,
      32,    33,   130,   104,    36,    37,   130,   611,   612,   613,
     614,     3,     4,     5,     6,   130,   130,     9,    18,    11,
      18,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     3,     4,     5,     6,   108,   104,     9,   131,    11,
       8,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     3,     4,     5,     6,     8,   127,     9,   128,    11,
     128,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     3,     4,     5,     6,   128,   104,     9,   128,    11,
     132,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     3,     4,     5,     6,   128,   130,     9,   128,    11,
     132,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,   128,   128,     3,     4,     5,     6,   129,   130,     9,
     128,    11,   128,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     3,     4,     5,     6,   128,   130,     9,
     128,    11,   128,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     3,     4,     5,     6,   128,   130,     9,
     128,    11,   128,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     3,     4,     5,     6,   128,   134,     9,
     130,    11,     8,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     8,     3,     4,     5,     6,     8,   129,
       9,   104,    11,   104,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     3,     4,     5,     6,   104,   129,
       9,   104,    11,   104,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     3,     4,     5,     6,    11,   129,
       9,   104,    11,   127,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,   104,     3,     4,     5,     6,   104,
     129,     9,   104,    11,   104,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     3,     4,     5,     6,   104,
     129,     9,   132,    11,   108,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     3,     4,     5,     6,   104,
     129,     9,   104,    11,   104,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,   129,     3,     4,     5,     6,
     104,   129,     9,   104,    11,     8,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
       8,   129,     9,   127,    11,   127,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
     129,   129,     9,   130,    11,   128,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,   127,     3,     4,     5,
       6,   127,   129,     9,   104,    11,   130,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     3,     4,     5,
       6,   134,   129,     9,   129,    11,   129,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     3,     4,     5,
       6,   129,   129,     9,   129,    11,   132,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     8,     3,     4,
       5,     6,    17,   129,     9,     8,    11,     8,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,     3,     4,
       5,     6,     8,   129,     9,   129,    11,    62,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,   130,    30,
      31,    32,    33,   129,    33,    36,    37,     8,    11,   104,
       8,   106,     8,     8,     8,    44,   112,   113,   114,   115,
     104,   132,   118,   119,   120,   120,   121,   122,   104,   125,
      33,    34,   128,    36,   104,   104,   131,   130,   130,    30,
      43,    44,    45,    46,   129,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    30,   132,   132,     8,    11,   105,   127,   114,   114,
     129,    17,   127,   112,   113,   114,   115,   116,   129,   118,
     119,   120,   121,   122,     8,    31,     8,   115,    31,   128,
      36,   104,    31,   106,   242,   142,   614,   361,   111,   509,
     461,   333,    36,   116,   117,   118,   619,   120,   121,   122,
      -1,    -1,   125,    70,    -1,   128,    62,    63,    64,    65,
      66,    -1,    79,    -1,    -1,    70,    -1,    -1,    -1,    -1,
      11,    -1,    77,    -1,    79,    92,    17,    -1,    -1,    -1,
      -1,    -1,    99,    -1,    -1,   102,    -1,    92,    94,    95,
      96,   108,    98,    -1,    99,    36,    -1,   102,   104,    -1,
     106,    -1,    -1,   108,     7,   111,    -1,    10,    -1,    12,
     116,   117,   118,    -1,   120,   121,   122,   123,    -1,   125,
     126,    62,    63,    64,    65,    66,   106,    30,    31,    32,
      33,    -1,    -1,    36,    37,    -1,   116,    -1,   118,    -1,
     120,   121,   122,     7,     8,    -1,    10,    -1,    12,    -1,
      -1,    -1,    -1,    94,    95,    96,    97,    98,    -1,    -1,
      -1,    -1,    -1,   104,    -1,   106,    30,    31,    32,    33,
     111,    -1,    36,    37,    -1,   116,   117,   118,    -1,   120,
     121,   122,   123,    -1,   125,     3,     4,     5,     6,    -1,
      -1,     9,    -1,    11,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,   112,   113,   114,   115,   116,
      -1,   118,   119,   120,   121,   122,     3,     4,     5,     6,
      -1,     8,     9,    -1,    11,    63,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
      -1,     8,     9,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
      -1,     8,     9,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
      -1,     8,     9,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
      -1,     8,     9,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
      -1,     8,     9,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
      -1,     8,     9,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
      -1,     8,     9,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     3,     4,     5,     6,
      -1,    -1,     9,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     4,     5,     6,    -1,
      -1,     9,    -1,    11,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     6,    -1,    -1,     9,    -1,
      11,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     9,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,   100,    36,
      37,    38,    39,    40,    41,    42,    -1,    10,    -1,    12,
     112,   113,   114,   115,    -1,    -1,   118,   119,   120,    -1,
      -1,    -1,    -1,   125,    -1,    -1,   128,    30,    31,    32,
      33,    -1,    -1,    36,    37,   106,    -1,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,    -1,   119,   120,
     121,   122,   123,    -1,   125,    -1,    -1,    -1,    -1,    -1,
     106,   132,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,    -1,   119,   120,   121,   122,   123,    -1,   125,
      -1,    -1,    -1,    -1,    -1,    -1,   132,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,   106,
      -1,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,    -1,   119,   120,   121,   122,   123,   106,   125,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,    -1,
     119,   120,   121,   122,   123,   106,   125,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,    -1,   119,   120,
     121,   122,   123,    -1,   125
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    67,   137,   140,    75,     0,     1,    67,    68,    69,
      76,   106,   125,   134,   138,   139,   141,   142,   143,   144,
     145,   148,   149,   150,   151,   154,   155,   156,   157,   158,
     159,   160,   161,   164,   166,   167,   168,   127,     8,   126,
      70,    77,    79,    92,    99,   102,   108,    89,    90,    91,
     106,   106,   125,   162,   134,   134,     8,   106,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   119,   120,
     121,   122,   123,   125,   146,   206,     8,   106,   147,   206,
       8,    71,    73,    86,    87,    67,   126,   138,   139,   133,
       9,    93,   103,   106,   107,   135,   193,     9,    93,   106,
     107,   135,   193,    84,    85,   106,   206,   127,   106,   206,
     106,   131,   196,   116,   106,   152,   153,    88,   112,   114,
     131,   200,   200,   200,   128,   134,   127,    80,    82,    83,
     127,   127,    80,    81,    82,    83,   127,   127,   127,   127,
     180,   181,   100,   112,   113,   114,   115,   118,   119,   120,
     125,   128,   170,   171,   172,   173,   174,   191,   103,   170,
       8,     8,   104,   128,   106,   197,     8,   127,     8,   153,
     127,   112,   114,   201,   131,   198,    33,   104,   131,   194,
     116,   131,   202,   106,   205,   163,   170,   128,   128,   128,
     128,    72,    74,   104,   104,   126,    11,    17,    36,    62,
      63,    64,    65,    66,    94,    95,    96,    97,    98,   104,
     106,   111,   116,   117,   118,   120,   121,   122,   123,   125,
     182,   184,   186,   187,   188,   189,   192,   172,   127,   170,
       6,     8,    11,   131,   175,   100,    15,    17,    18,    27,
      28,   127,   175,     8,   104,   106,   132,   206,   116,    62,
     104,   106,   112,   114,   132,   106,   199,   206,     8,   104,
      33,   104,   106,   195,     8,   106,   116,   203,     8,   129,
     130,   175,   104,   104,   104,   104,   106,   125,   134,   186,
     192,   106,   124,   131,   134,    62,   104,   106,   120,   121,
     122,   131,   185,   189,   131,   185,     8,   131,    33,    34,
      43,    44,    45,    46,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,   111,   117,
     128,   183,   184,   186,   192,   106,   192,   128,   186,    11,
     104,   106,   125,   184,   190,   192,   120,   134,   127,   134,
     128,   127,   128,   183,   128,   134,   127,   131,    33,    44,
     105,   128,   169,   191,   192,   129,   170,   170,   170,   176,
      78,   133,   165,   169,   169,   169,   169,   169,   169,   165,
     129,   131,    86,   106,   132,   206,   104,    33,   104,   106,
     132,   106,   116,   132,   106,   133,   130,   130,   130,   130,
     104,    18,    18,   108,   104,   131,   183,     8,   183,     8,
     183,   183,   183,   183,   183,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   183,     3,     4,     5,     6,     9,    11,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    63,
     183,   128,   131,   134,     8,   127,   134,   127,   104,   130,
     134,   186,   134,   106,   125,   188,   192,     8,     8,   104,
     183,   104,   183,   204,   183,   204,   127,   104,   104,   183,
     104,   169,   169,   169,     7,    10,    12,    30,    31,    32,
      33,    36,    37,   101,   106,   114,   120,   132,   178,   179,
     180,   104,   127,   104,   104,   104,   104,   104,   186,   132,
     108,   132,   132,   132,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   204,   183,
     204,   129,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   185,   187,
     104,   104,   183,   104,   129,   106,   109,   104,   104,     8,
       8,   127,   129,   130,     8,   129,   183,   129,   127,     8,
     130,   129,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   128,   127,   127,   181,    31,   126,   177,   130,   104,
     129,   129,   129,   129,   134,   132,     8,     8,     8,   129,
     130,   129,   130,   130,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   130,   129,     8,   129,   130,
       8,     8,   127,     8,     8,     8,   186,   183,     8,   183,
       8,     8,   104,   191,   192,   169,   169,   126,   177,   106,
     109,   104,   196,   104,   183,   183,   183,   183,   104,   183,
       8,   132,   130,   130,     8,     8,    30,    30,   132,     8,
     129,   129,   129,   129,   132,     8,   127,   114,   114,   183,
     129,   129,     8,     8,     8
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
        case 19:
/* Line 1792 of yacc.c  */
#line 175 "slghparse.y"
    { slgh->resetConstructors(); }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 177 "slghparse.y"
    { slgh->setEndian(1); }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 178 "slghparse.y"
    { slgh->setEndian(0); }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 180 "slghparse.y"
    { slgh->setAlignment(*(yyvsp[(4) - (5)].i)); delete (yyvsp[(4) - (5)].i); }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 182 "slghparse.y"
    {}
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 184 "slghparse.y"
    { (yyval.tokensym) = slgh->defineToken((yyvsp[(3) - (6)].str),(yyvsp[(5) - (6)].i)); }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 185 "slghparse.y"
    { (yyval.tokensym) = (yyvsp[(1) - (2)].tokensym); slgh->addTokenField((yyvsp[(1) - (2)].tokensym),(yyvsp[(2) - (2)].fieldqual)); }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 186 "slghparse.y"
    { string errmsg=(yyvsp[(3) - (3)].anysym)->getName()+": redefined as a token"; yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 188 "slghparse.y"
    {}
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 190 "slghparse.y"
    { (yyval.varsym) = (yyvsp[(3) - (3)].varsym); }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 191 "slghparse.y"
    { (yyval.varsym) = (yyvsp[(1) - (2)].varsym); if (!slgh->addContextField( (yyvsp[(1) - (2)].varsym), (yyvsp[(2) - (2)].fieldqual) ))
                                            { yyerror("All context definitions must come before constructors"); YYERROR; } }
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 194 "slghparse.y"
    { (yyval.fieldqual) = new FieldQuality((yyvsp[(1) - (7)].str),(yyvsp[(4) - (7)].i),(yyvsp[(6) - (7)].i)); }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 195 "slghparse.y"
    { delete (yyvsp[(4) - (7)].i); delete (yyvsp[(6) - (7)].i); string errmsg = (yyvsp[(1) - (7)].anysym)->getName()+": redefined as field"; yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 196 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->signext = true; }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 197 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->hex = true; }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 198 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->hex = false; }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 200 "slghparse.y"
    { (yyval.fieldqual) = new FieldQuality((yyvsp[(1) - (7)].str),(yyvsp[(4) - (7)].i),(yyvsp[(6) - (7)].i)); }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 201 "slghparse.y"
    { delete (yyvsp[(4) - (7)].i); delete (yyvsp[(6) - (7)].i); string errmsg = (yyvsp[(1) - (7)].anysym)->getName()+": redefined as field"; yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 202 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->signext = true; }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 203 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->flow = false; }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 204 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->hex = true; }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 205 "slghparse.y"
    { (yyval.fieldqual) = (yyvsp[(1) - (2)].fieldqual); (yyval.fieldqual)->hex = false; }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 207 "slghparse.y"
    { slgh->newSpace((yyvsp[(1) - (2)].spacequal)); }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 209 "slghparse.y"
    { (yyval.spacequal) = new SpaceQuality(*(yyvsp[(3) - (3)].str)); delete (yyvsp[(3) - (3)].str); }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 210 "slghparse.y"
    { string errmsg = (yyvsp[(3) - (3)].anysym)->getName()+": redefined as space"; yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 211 "slghparse.y"
    { (yyval.spacequal) = (yyvsp[(1) - (4)].spacequal); (yyval.spacequal)->type = SpaceQuality::ramtype; }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 212 "slghparse.y"
    { (yyval.spacequal) = (yyvsp[(1) - (4)].spacequal); (yyval.spacequal)->type = SpaceQuality::registertype; }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 213 "slghparse.y"
    { (yyval.spacequal) = (yyvsp[(1) - (4)].spacequal); (yyval.spacequal)->size = *(yyvsp[(4) - (4)].i); delete (yyvsp[(4) - (4)].i); }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 214 "slghparse.y"
    { (yyval.spacequal) = (yyvsp[(1) - (4)].spacequal); (yyval.spacequal)->wordsize = *(yyvsp[(4) - (4)].i); delete (yyvsp[(4) - (4)].i); }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 215 "slghparse.y"
    { (yyval.spacequal) = (yyvsp[(1) - (2)].spacequal); (yyval.spacequal)->isdefault = true; }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 217 "slghparse.y"
    {
               slgh->defineVarnodes((yyvsp[(2) - (10)].spacesym),(yyvsp[(5) - (10)].i),(yyvsp[(8) - (10)].i),(yyvsp[(9) - (10)].strlist)); }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 219 "slghparse.y"
    { yyerror("Parsed integer is too big (overflow)"); YYERROR; }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 226 "slghparse.y"
    {
               slgh->defineBitrange((yyvsp[(1) - (8)].str),(yyvsp[(3) - (8)].varsym),(uint4)*(yyvsp[(5) - (8)].i),(uint4)*(yyvsp[(7) - (8)].i)); delete (yyvsp[(5) - (8)].i); delete (yyvsp[(7) - (8)].i); }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 229 "slghparse.y"
    { slgh->addUserOp((yyvsp[(3) - (4)].strlist)); }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 231 "slghparse.y"
    { slgh->attachValues((yyvsp[(3) - (5)].symlist),(yyvsp[(4) - (5)].biglist)); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 233 "slghparse.y"
    { slgh->attachNames((yyvsp[(3) - (5)].symlist),(yyvsp[(4) - (5)].strlist)); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 235 "slghparse.y"
    { slgh->attachVarnodes((yyvsp[(3) - (5)].symlist),(yyvsp[(4) - (5)].symlist)); }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 237 "slghparse.y"
    { slgh->buildMacro((yyvsp[(1) - (4)].macrosym),(yyvsp[(3) - (4)].sem)); }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 240 "slghparse.y"
    {  slgh->pushWith((yyvsp[(2) - (6)].subtablesym),(yyvsp[(4) - (6)].pateq),(yyvsp[(5) - (6)].contop)); }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 246 "slghparse.y"
    { slgh->popWith(); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 248 "slghparse.y"
    { (yyval.subtablesym) = (SubtableSymbol *)0; }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 249 "slghparse.y"
    { (yyval.subtablesym) = (yyvsp[(1) - (1)].subtablesym); }
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 250 "slghparse.y"
    { (yyval.subtablesym) = slgh->newTable((yyvsp[(1) - (1)].str)); }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 253 "slghparse.y"
    { (yyval.pateq) = (PatternEquation *)0; }
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 254 "slghparse.y"
    { (yyval.pateq) = (yyvsp[(1) - (1)].pateq); }
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 257 "slghparse.y"
    { (yyval.macrosym) = slgh->createMacro((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].strlist)); }
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 259 "slghparse.y"
    { (yyval.sectionstart) = slgh->standaloneSection((yyvsp[(2) - (3)].sem)); }
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 260 "slghparse.y"
    { (yyval.sectionstart) = slgh->finalNamedSection((yyvsp[(2) - (4)].sectionstart),(yyvsp[(3) - (4)].sem)); }
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 261 "slghparse.y"
    { (yyval.sectionstart) = (SectionVector *)0; }
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 263 "slghparse.y"
    { slgh->buildConstructor((yyvsp[(1) - (5)].construct),(yyvsp[(3) - (5)].pateq),(yyvsp[(4) - (5)].contop),(yyvsp[(5) - (5)].sectionstart)); }
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 264 "slghparse.y"
    { slgh->buildConstructor((yyvsp[(1) - (5)].construct),(yyvsp[(3) - (5)].pateq),(yyvsp[(4) - (5)].contop),(yyvsp[(5) - (5)].sectionstart)); }
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 266 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); (yyval.construct)->addSyntax(*(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); }
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 267 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); (yyval.construct)->addSyntax(*(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); }
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 268 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); if (slgh->isInRoot((yyvsp[(1) - (2)].construct))) { (yyval.construct)->addSyntax(*(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); } else slgh->newOperand((yyvsp[(1) - (2)].construct),(yyvsp[(2) - (2)].str)); }
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 269 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); if (!slgh->isInRoot((yyvsp[(1) - (2)].construct))) { yyerror("Unexpected '^' at start of print pieces");  YYERROR; } }
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 270 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); }
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 271 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); (yyval.construct)->addSyntax(*(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); }
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 272 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); (yyval.construct)->addSyntax(*(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); }
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 273 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); (yyval.construct)->addSyntax(string(" ")); }
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 274 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); slgh->newOperand((yyvsp[(1) - (2)].construct),(yyvsp[(2) - (2)].str)); }
    break;

  case 85:
/* Line 1792 of yacc.c  */
#line 276 "slghparse.y"
    { (yyval.construct) = slgh->createConstructor((yyvsp[(1) - (2)].subtablesym)); }
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 277 "slghparse.y"
    { SubtableSymbol *sym=slgh->newTable((yyvsp[(1) - (2)].str)); (yyval.construct) = slgh->createConstructor(sym); }
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 278 "slghparse.y"
    { (yyval.construct) = slgh->createConstructor((SubtableSymbol *)0); }
    break;

  case 88:
/* Line 1792 of yacc.c  */
#line 279 "slghparse.y"
    { (yyval.construct) = (yyvsp[(1) - (2)].construct); }
    break;

  case 89:
/* Line 1792 of yacc.c  */
#line 281 "slghparse.y"
    { (yyval.patexp) = new ConstantValue(*(yyvsp[(1) - (1)].big)); delete (yyvsp[(1) - (1)].big); }
    break;

  case 90:
/* Line 1792 of yacc.c  */
#line 283 "slghparse.y"
    { if ((actionon==1)&&((yyvsp[(1) - (1)].famsym)->getType() != SleighSymbol::context_symbol))
                                             { string errmsg="Global symbol "+(yyvsp[(1) - (1)].famsym)->getName(); errmsg += " is not allowed in action expression"; yyerror(errmsg.c_str()); } (yyval.patexp) = (yyvsp[(1) - (1)].famsym)->getPatternValue(); }
    break;

  case 91:
/* Line 1792 of yacc.c  */
#line 286 "slghparse.y"
    { (yyval.patexp) = (yyvsp[(1) - (1)].specsym)->getPatternExpression(); }
    break;

  case 92:
/* Line 1792 of yacc.c  */
#line 287 "slghparse.y"
    { (yyval.patexp) = (yyvsp[(2) - (3)].patexp); }
    break;

  case 93:
/* Line 1792 of yacc.c  */
#line 288 "slghparse.y"
    { (yyval.patexp) = new PlusExpression((yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 94:
/* Line 1792 of yacc.c  */
#line 289 "slghparse.y"
    { (yyval.patexp) = new SubExpression((yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 95:
/* Line 1792 of yacc.c  */
#line 290 "slghparse.y"
    { (yyval.patexp) = new MultExpression((yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 96:
/* Line 1792 of yacc.c  */
#line 291 "slghparse.y"
    { (yyval.patexp) = new LeftShiftExpression((yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 97:
/* Line 1792 of yacc.c  */
#line 292 "slghparse.y"
    { (yyval.patexp) = new RightShiftExpression((yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 98:
/* Line 1792 of yacc.c  */
#line 293 "slghparse.y"
    { (yyval.patexp) = new AndExpression((yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 99:
/* Line 1792 of yacc.c  */
#line 294 "slghparse.y"
    { (yyval.patexp) = new OrExpression((yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 100:
/* Line 1792 of yacc.c  */
#line 295 "slghparse.y"
    { (yyval.patexp) = new XorExpression((yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 101:
/* Line 1792 of yacc.c  */
#line 296 "slghparse.y"
    { (yyval.patexp) = new DivExpression((yyvsp[(1) - (3)].patexp),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 102:
/* Line 1792 of yacc.c  */
#line 297 "slghparse.y"
    { (yyval.patexp) = new MinusExpression((yyvsp[(2) - (2)].patexp)); }
    break;

  case 103:
/* Line 1792 of yacc.c  */
#line 298 "slghparse.y"
    { (yyval.patexp) = new NotExpression((yyvsp[(2) - (2)].patexp)); }
    break;

  case 105:
/* Line 1792 of yacc.c  */
#line 301 "slghparse.y"
    { (yyval.pateq) = new EquationAnd((yyvsp[(1) - (3)].pateq),(yyvsp[(3) - (3)].pateq)); }
    break;

  case 106:
/* Line 1792 of yacc.c  */
#line 302 "slghparse.y"
    { (yyval.pateq) = new EquationOr((yyvsp[(1) - (3)].pateq),(yyvsp[(3) - (3)].pateq)); }
    break;

  case 107:
/* Line 1792 of yacc.c  */
#line 303 "slghparse.y"
    { (yyval.pateq) = new EquationCat((yyvsp[(1) - (3)].pateq),(yyvsp[(3) - (3)].pateq)); }
    break;

  case 108:
/* Line 1792 of yacc.c  */
#line 305 "slghparse.y"
    { (yyval.pateq) = new EquationLeftEllipsis((yyvsp[(2) - (2)].pateq)); }
    break;

  case 110:
/* Line 1792 of yacc.c  */
#line 308 "slghparse.y"
    { (yyval.pateq) = new EquationRightEllipsis((yyvsp[(1) - (2)].pateq)); }
    break;

  case 113:
/* Line 1792 of yacc.c  */
#line 312 "slghparse.y"
    { (yyval.pateq) = (yyvsp[(2) - (3)].pateq); }
    break;

  case 114:
/* Line 1792 of yacc.c  */
#line 314 "slghparse.y"
    { (yyval.pateq) = new EqualEquation((yyvsp[(1) - (3)].famsym)->getPatternValue(),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 115:
/* Line 1792 of yacc.c  */
#line 315 "slghparse.y"
    { (yyval.pateq) = new NotEqualEquation((yyvsp[(1) - (3)].famsym)->getPatternValue(),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 116:
/* Line 1792 of yacc.c  */
#line 316 "slghparse.y"
    { (yyval.pateq) = new LessEquation((yyvsp[(1) - (3)].famsym)->getPatternValue(),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 117:
/* Line 1792 of yacc.c  */
#line 317 "slghparse.y"
    { (yyval.pateq) = new LessEqualEquation((yyvsp[(1) - (3)].famsym)->getPatternValue(),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 118:
/* Line 1792 of yacc.c  */
#line 318 "slghparse.y"
    { (yyval.pateq) = new GreaterEquation((yyvsp[(1) - (3)].famsym)->getPatternValue(),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 119:
/* Line 1792 of yacc.c  */
#line 319 "slghparse.y"
    { (yyval.pateq) = new GreaterEqualEquation((yyvsp[(1) - (3)].famsym)->getPatternValue(),(yyvsp[(3) - (3)].patexp)); }
    break;

  case 120:
/* Line 1792 of yacc.c  */
#line 320 "slghparse.y"
    { (yyval.pateq) = slgh->constrainOperand((yyvsp[(1) - (3)].operandsym),(yyvsp[(3) - (3)].patexp)); 
                                          if ((yyval.pateq) == (PatternEquation *)0) 
                                            { string errmsg="Constraining currently undefined operand "+(yyvsp[(1) - (3)].operandsym)->getName(); yyerror(errmsg.c_str()); } }
    break;

  case 121:
/* Line 1792 of yacc.c  */
#line 323 "slghparse.y"
    { (yyval.pateq) = new OperandEquation((yyvsp[(1) - (1)].operandsym)->getIndex()); slgh->selfDefine((yyvsp[(1) - (1)].operandsym)); }
    break;

  case 122:
/* Line 1792 of yacc.c  */
#line 324 "slghparse.y"
    { (yyval.pateq) = new UnconstrainedEquation((yyvsp[(1) - (1)].specsym)->getPatternExpression()); }
    break;

  case 123:
/* Line 1792 of yacc.c  */
#line 325 "slghparse.y"
    { (yyval.pateq) = slgh->defineInvisibleOperand((yyvsp[(1) - (1)].famsym)); }
    break;

  case 124:
/* Line 1792 of yacc.c  */
#line 326 "slghparse.y"
    { (yyval.pateq) = slgh->defineInvisibleOperand((yyvsp[(1) - (1)].subtablesym)); }
    break;

  case 125:
/* Line 1792 of yacc.c  */
#line 328 "slghparse.y"
    { (yyval.contop) = (vector<ContextChange *> *)0; }
    break;

  case 126:
/* Line 1792 of yacc.c  */
#line 329 "slghparse.y"
    { (yyval.contop) = (yyvsp[(2) - (3)].contop); }
    break;

  case 127:
/* Line 1792 of yacc.c  */
#line 331 "slghparse.y"
    { (yyval.contop) = new vector<ContextChange *>; }
    break;

  case 128:
/* Line 1792 of yacc.c  */
#line 332 "slghparse.y"
    { (yyval.contop) = (yyvsp[(1) - (5)].contop); if (!slgh->contextMod((yyvsp[(1) - (5)].contop),(yyvsp[(2) - (5)].contextsym),(yyvsp[(4) - (5)].patexp))) { string errmsg="Cannot use 'inst_next' to set context variable: "+(yyvsp[(2) - (5)].contextsym)->getName(); yyerror(errmsg.c_str()); YYERROR; } }
    break;

  case 129:
/* Line 1792 of yacc.c  */
#line 333 "slghparse.y"
    { (yyval.contop) = (yyvsp[(1) - (8)].contop); slgh->contextSet((yyvsp[(1) - (8)].contop),(yyvsp[(4) - (8)].famsym),(yyvsp[(6) - (8)].contextsym)); }
    break;

  case 130:
/* Line 1792 of yacc.c  */
#line 334 "slghparse.y"
    { (yyval.contop) = (yyvsp[(1) - (8)].contop); slgh->contextSet((yyvsp[(1) - (8)].contop),(yyvsp[(4) - (8)].specsym),(yyvsp[(6) - (8)].contextsym)); }
    break;

  case 131:
/* Line 1792 of yacc.c  */
#line 335 "slghparse.y"
    { (yyval.contop) = (yyvsp[(1) - (5)].contop); slgh->defineOperand((yyvsp[(2) - (5)].operandsym),(yyvsp[(4) - (5)].patexp)); }
    break;

  case 132:
/* Line 1792 of yacc.c  */
#line 336 "slghparse.y"
    { string errmsg="Expecting context symbol, not "+*(yyvsp[(2) - (2)].str); delete (yyvsp[(2) - (2)].str); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 133:
/* Line 1792 of yacc.c  */
#line 338 "slghparse.y"
    { (yyval.sectionsym) = slgh->newSectionSymbol( *(yyvsp[(2) - (3)].str) ); delete (yyvsp[(2) - (3)].str); }
    break;

  case 134:
/* Line 1792 of yacc.c  */
#line 339 "slghparse.y"
    { (yyval.sectionsym) = (yyvsp[(2) - (3)].sectionsym); }
    break;

  case 135:
/* Line 1792 of yacc.c  */
#line 341 "slghparse.y"
    { (yyval.sectionstart) = slgh->firstNamedSection((yyvsp[(1) - (2)].sem),(yyvsp[(2) - (2)].sectionsym)); }
    break;

  case 136:
/* Line 1792 of yacc.c  */
#line 343 "slghparse.y"
    { (yyval.sectionstart) = (yyvsp[(1) - (1)].sectionstart); }
    break;

  case 137:
/* Line 1792 of yacc.c  */
#line 344 "slghparse.y"
    { (yyval.sectionstart) = slgh->nextNamedSection((yyvsp[(1) - (3)].sectionstart),(yyvsp[(2) - (3)].sem),(yyvsp[(3) - (3)].sectionsym)); }
    break;

  case 138:
/* Line 1792 of yacc.c  */
#line 346 "slghparse.y"
    { (yyval.sem) = (yyvsp[(1) - (1)].sem); if ((yyval.sem)->getOpvec().empty() && ((yyval.sem)->getResult() == (HandleTpl *)0)) slgh->recordNop(); }
    break;

  case 139:
/* Line 1792 of yacc.c  */
#line 347 "slghparse.y"
    { (yyval.sem) = slgh->setResultVarnode((yyvsp[(1) - (4)].sem),(yyvsp[(3) - (4)].varnode)); }
    break;

  case 140:
/* Line 1792 of yacc.c  */
#line 348 "slghparse.y"
    { (yyval.sem) = slgh->setResultStarVarnode((yyvsp[(1) - (5)].sem),(yyvsp[(3) - (5)].starqual),(yyvsp[(4) - (5)].varnode)); }
    break;

  case 141:
/* Line 1792 of yacc.c  */
#line 349 "slghparse.y"
    { string errmsg="Unknown export varnode: "+*(yyvsp[(3) - (3)].str); delete (yyvsp[(3) - (3)].str); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 142:
/* Line 1792 of yacc.c  */
#line 350 "slghparse.y"
    { string errmsg="Unknown pointer varnode: "+*(yyvsp[(4) - (4)].str); delete (yyvsp[(3) - (4)].starqual); delete (yyvsp[(4) - (4)].str); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 143:
/* Line 1792 of yacc.c  */
#line 352 "slghparse.y"
    { (yyval.sem) = new ConstructTpl(); }
    break;

  case 144:
/* Line 1792 of yacc.c  */
#line 353 "slghparse.y"
    { (yyval.sem) = (yyvsp[(1) - (2)].sem); if (!(yyval.sem)->addOpList(*(yyvsp[(2) - (2)].stmt))) { delete (yyvsp[(2) - (2)].stmt); yyerror("Multiple delayslot declarations"); YYERROR; } delete (yyvsp[(2) - (2)].stmt); }
    break;

  case 145:
/* Line 1792 of yacc.c  */
#line 354 "slghparse.y"
    { (yyval.sem) = (yyvsp[(1) - (4)].sem); slgh->pcode.newLocalDefinition((yyvsp[(3) - (4)].str)); }
    break;

  case 146:
/* Line 1792 of yacc.c  */
#line 355 "slghparse.y"
    { (yyval.sem) = (yyvsp[(1) - (6)].sem); slgh->pcode.newLocalDefinition((yyvsp[(3) - (6)].str),*(yyvsp[(5) - (6)].i)); delete (yyvsp[(5) - (6)].i); }
    break;

  case 147:
/* Line 1792 of yacc.c  */
#line 357 "slghparse.y"
    { (yyvsp[(3) - (4)].tree)->setOutput((yyvsp[(1) - (4)].varnode)); (yyval.stmt) = ExprTree::toVector((yyvsp[(3) - (4)].tree)); }
    break;

  case 148:
/* Line 1792 of yacc.c  */
#line 358 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.newOutput(true,(yyvsp[(4) - (5)].tree),(yyvsp[(2) - (5)].str)); }
    break;

  case 149:
/* Line 1792 of yacc.c  */
#line 359 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.newOutput(false,(yyvsp[(3) - (4)].tree),(yyvsp[(1) - (4)].str)); }
    break;

  case 150:
/* Line 1792 of yacc.c  */
#line 360 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.newOutput(true,(yyvsp[(6) - (7)].tree),(yyvsp[(2) - (7)].str),*(yyvsp[(4) - (7)].i)); delete (yyvsp[(4) - (7)].i); }
    break;

  case 151:
/* Line 1792 of yacc.c  */
#line 361 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.newOutput(true,(yyvsp[(5) - (6)].tree),(yyvsp[(1) - (6)].str),*(yyvsp[(3) - (6)].i)); delete (yyvsp[(3) - (6)].i); }
    break;

  case 152:
/* Line 1792 of yacc.c  */
#line 362 "slghparse.y"
    { (yyval.stmt) = (vector<OpTpl *> *)0; string errmsg = "Redefinition of symbol: "+(yyvsp[(2) - (3)].specsym)->getName(); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 153:
/* Line 1792 of yacc.c  */
#line 363 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.createStore((yyvsp[(1) - (5)].starqual),(yyvsp[(2) - (5)].tree),(yyvsp[(4) - (5)].tree)); }
    break;

  case 154:
/* Line 1792 of yacc.c  */
#line 364 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.createUserOpNoOut((yyvsp[(1) - (5)].useropsym),(yyvsp[(3) - (5)].param)); }
    break;

  case 155:
/* Line 1792 of yacc.c  */
#line 365 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.assignBitRange((yyvsp[(1) - (9)].varnode),(uint4)*(yyvsp[(3) - (9)].i),(uint4)*(yyvsp[(5) - (9)].i),(yyvsp[(8) - (9)].tree)); delete (yyvsp[(3) - (9)].i), delete (yyvsp[(5) - (9)].i); }
    break;

  case 156:
/* Line 1792 of yacc.c  */
#line 366 "slghparse.y"
    { (yyval.stmt)=slgh->pcode.assignBitRange((yyvsp[(1) - (4)].bitsym)->getParentSymbol()->getVarnode(),(yyvsp[(1) - (4)].bitsym)->getBitOffset(),(yyvsp[(1) - (4)].bitsym)->numBits(),(yyvsp[(3) - (4)].tree)); }
    break;

  case 157:
/* Line 1792 of yacc.c  */
#line 367 "slghparse.y"
    { delete (yyvsp[(1) - (4)].varnode); delete (yyvsp[(3) - (4)].i); yyerror("Illegal truncation on left-hand side of assignment"); YYERROR; }
    break;

  case 158:
/* Line 1792 of yacc.c  */
#line 368 "slghparse.y"
    { delete (yyvsp[(1) - (4)].varnode); delete (yyvsp[(3) - (4)].i); yyerror("Illegal subpiece on left-hand side of assignment"); YYERROR; }
    break;

  case 159:
/* Line 1792 of yacc.c  */
#line 369 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.createOpConst(BUILD,(yyvsp[(2) - (3)].operandsym)->getIndex()); }
    break;

  case 160:
/* Line 1792 of yacc.c  */
#line 370 "slghparse.y"
    { (yyval.stmt) = slgh->createCrossBuild((yyvsp[(2) - (5)].varnode),(yyvsp[(4) - (5)].sectionsym)); }
    break;

  case 161:
/* Line 1792 of yacc.c  */
#line 371 "slghparse.y"
    { (yyval.stmt) = slgh->createCrossBuild((yyvsp[(2) - (5)].varnode),slgh->newSectionSymbol(*(yyvsp[(4) - (5)].str))); delete (yyvsp[(4) - (5)].str); }
    break;

  case 162:
/* Line 1792 of yacc.c  */
#line 372 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.createOpConst(DELAY_SLOT,*(yyvsp[(3) - (5)].i)); delete (yyvsp[(3) - (5)].i); }
    break;

  case 163:
/* Line 1792 of yacc.c  */
#line 373 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.createOpNoOut(CPUI_BRANCH,new ExprTree((yyvsp[(2) - (3)].varnode))); }
    break;

  case 164:
/* Line 1792 of yacc.c  */
#line 374 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.createOpNoOut(CPUI_CBRANCH,new ExprTree((yyvsp[(4) - (5)].varnode)),(yyvsp[(2) - (5)].tree)); }
    break;

  case 165:
/* Line 1792 of yacc.c  */
#line 375 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.createOpNoOut(CPUI_BRANCHIND,(yyvsp[(3) - (5)].tree)); }
    break;

  case 166:
/* Line 1792 of yacc.c  */
#line 376 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.createOpNoOut(CPUI_CALL,new ExprTree((yyvsp[(2) - (3)].varnode))); }
    break;

  case 167:
/* Line 1792 of yacc.c  */
#line 377 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.createOpNoOut(CPUI_CALLIND,(yyvsp[(3) - (5)].tree)); }
    break;

  case 168:
/* Line 1792 of yacc.c  */
#line 378 "slghparse.y"
    { yyerror("Must specify an indirect parameter for return"); YYERROR; }
    break;

  case 169:
/* Line 1792 of yacc.c  */
#line 379 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.createOpNoOut(CPUI_RETURN,(yyvsp[(3) - (5)].tree)); }
    break;

  case 170:
/* Line 1792 of yacc.c  */
#line 380 "slghparse.y"
    { (yyval.stmt) = slgh->createMacroUse((yyvsp[(1) - (5)].macrosym),(yyvsp[(3) - (5)].param)); }
    break;

  case 171:
/* Line 1792 of yacc.c  */
#line 381 "slghparse.y"
    { (yyval.stmt) = slgh->pcode.placeLabel( (yyvsp[(1) - (1)].labelsym) ); }
    break;

  case 172:
/* Line 1792 of yacc.c  */
#line 383 "slghparse.y"
    { (yyval.tree) = new ExprTree((yyvsp[(1) - (1)].varnode)); }
    break;

  case 173:
/* Line 1792 of yacc.c  */
#line 384 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createLoad((yyvsp[(1) - (2)].starqual),(yyvsp[(2) - (2)].tree)); }
    break;

  case 174:
/* Line 1792 of yacc.c  */
#line 385 "slghparse.y"
    { (yyval.tree) = (yyvsp[(2) - (3)].tree); }
    break;

  case 175:
/* Line 1792 of yacc.c  */
#line 386 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_ADD,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 176:
/* Line 1792 of yacc.c  */
#line 387 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SUB,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 177:
/* Line 1792 of yacc.c  */
#line 388 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_EQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 178:
/* Line 1792 of yacc.c  */
#line 389 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_NOTEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 179:
/* Line 1792 of yacc.c  */
#line 390 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_LESS,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 180:
/* Line 1792 of yacc.c  */
#line 391 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_LESSEQUAL,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); }
    break;

  case 181:
/* Line 1792 of yacc.c  */
#line 392 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_LESSEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 182:
/* Line 1792 of yacc.c  */
#line 393 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_LESS,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); }
    break;

  case 183:
/* Line 1792 of yacc.c  */
#line 394 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SLESS,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 184:
/* Line 1792 of yacc.c  */
#line 395 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SLESSEQUAL,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); }
    break;

  case 185:
/* Line 1792 of yacc.c  */
#line 396 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SLESSEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 186:
/* Line 1792 of yacc.c  */
#line 397 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SLESS,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); }
    break;

  case 187:
/* Line 1792 of yacc.c  */
#line 398 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_2COMP,(yyvsp[(2) - (2)].tree)); }
    break;

  case 188:
/* Line 1792 of yacc.c  */
#line 399 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_NEGATE,(yyvsp[(2) - (2)].tree)); }
    break;

  case 189:
/* Line 1792 of yacc.c  */
#line 400 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_XOR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 190:
/* Line 1792 of yacc.c  */
#line 401 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_AND,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 191:
/* Line 1792 of yacc.c  */
#line 402 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_OR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 192:
/* Line 1792 of yacc.c  */
#line 403 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_LEFT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 193:
/* Line 1792 of yacc.c  */
#line 404 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_RIGHT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 194:
/* Line 1792 of yacc.c  */
#line 405 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SRIGHT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 195:
/* Line 1792 of yacc.c  */
#line 406 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_MULT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 196:
/* Line 1792 of yacc.c  */
#line 407 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_DIV,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 197:
/* Line 1792 of yacc.c  */
#line 408 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SDIV,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 198:
/* Line 1792 of yacc.c  */
#line 409 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_REM,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 199:
/* Line 1792 of yacc.c  */
#line 410 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SREM,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 200:
/* Line 1792 of yacc.c  */
#line 411 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_BOOL_NEGATE,(yyvsp[(2) - (2)].tree)); }
    break;

  case 201:
/* Line 1792 of yacc.c  */
#line 412 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_BOOL_XOR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 202:
/* Line 1792 of yacc.c  */
#line 413 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_BOOL_AND,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 203:
/* Line 1792 of yacc.c  */
#line 414 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_BOOL_OR,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 204:
/* Line 1792 of yacc.c  */
#line 415 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_EQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 205:
/* Line 1792 of yacc.c  */
#line 416 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_NOTEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 206:
/* Line 1792 of yacc.c  */
#line 417 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_LESS,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 207:
/* Line 1792 of yacc.c  */
#line 418 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_LESS,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); }
    break;

  case 208:
/* Line 1792 of yacc.c  */
#line 419 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_LESSEQUAL,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 209:
/* Line 1792 of yacc.c  */
#line 420 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_LESSEQUAL,(yyvsp[(3) - (3)].tree),(yyvsp[(1) - (3)].tree)); }
    break;

  case 210:
/* Line 1792 of yacc.c  */
#line 421 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_ADD,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 211:
/* Line 1792 of yacc.c  */
#line 422 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_SUB,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 212:
/* Line 1792 of yacc.c  */
#line 423 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_MULT,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 213:
/* Line 1792 of yacc.c  */
#line 424 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_DIV,(yyvsp[(1) - (3)].tree),(yyvsp[(3) - (3)].tree)); }
    break;

  case 214:
/* Line 1792 of yacc.c  */
#line 425 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_NEG,(yyvsp[(2) - (2)].tree)); }
    break;

  case 215:
/* Line 1792 of yacc.c  */
#line 426 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_ABS,(yyvsp[(3) - (4)].tree)); }
    break;

  case 216:
/* Line 1792 of yacc.c  */
#line 427 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_SQRT,(yyvsp[(3) - (4)].tree)); }
    break;

  case 217:
/* Line 1792 of yacc.c  */
#line 428 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SEXT,(yyvsp[(3) - (4)].tree)); }
    break;

  case 218:
/* Line 1792 of yacc.c  */
#line 429 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_ZEXT,(yyvsp[(3) - (4)].tree)); }
    break;

  case 219:
/* Line 1792 of yacc.c  */
#line 430 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_CARRY,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); }
    break;

  case 220:
/* Line 1792 of yacc.c  */
#line 431 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SCARRY,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); }
    break;

  case 221:
/* Line 1792 of yacc.c  */
#line 432 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_INT_SBORROW,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); }
    break;

  case 222:
/* Line 1792 of yacc.c  */
#line 433 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_FLOAT2FLOAT,(yyvsp[(3) - (4)].tree)); }
    break;

  case 223:
/* Line 1792 of yacc.c  */
#line 434 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_INT2FLOAT,(yyvsp[(3) - (4)].tree)); }
    break;

  case 224:
/* Line 1792 of yacc.c  */
#line 435 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_NAN,(yyvsp[(3) - (4)].tree)); }
    break;

  case 225:
/* Line 1792 of yacc.c  */
#line 436 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_TRUNC,(yyvsp[(3) - (4)].tree)); }
    break;

  case 226:
/* Line 1792 of yacc.c  */
#line 437 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_CEIL,(yyvsp[(3) - (4)].tree)); }
    break;

  case 227:
/* Line 1792 of yacc.c  */
#line 438 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_FLOOR,(yyvsp[(3) - (4)].tree)); }
    break;

  case 228:
/* Line 1792 of yacc.c  */
#line 439 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_FLOAT_ROUND,(yyvsp[(3) - (4)].tree)); }
    break;

  case 229:
/* Line 1792 of yacc.c  */
#line 440 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_NEW,(yyvsp[(3) - (4)].tree)); }
    break;

  case 230:
/* Line 1792 of yacc.c  */
#line 441 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_NEW,(yyvsp[(3) - (6)].tree),(yyvsp[(5) - (6)].tree)); }
    break;

  case 231:
/* Line 1792 of yacc.c  */
#line 442 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createOp(CPUI_SUBPIECE,new ExprTree((yyvsp[(1) - (4)].specsym)->getVarnode()),new ExprTree((yyvsp[(3) - (4)].varnode))); }
    break;

  case 232:
/* Line 1792 of yacc.c  */
#line 443 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createBitRange((yyvsp[(1) - (3)].specsym),0,(uint4)(*(yyvsp[(3) - (3)].i) * 8)); delete (yyvsp[(3) - (3)].i); }
    break;

  case 233:
/* Line 1792 of yacc.c  */
#line 444 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createBitRange((yyvsp[(1) - (6)].specsym),(uint4)*(yyvsp[(3) - (6)].i),(uint4)*(yyvsp[(5) - (6)].i)); delete (yyvsp[(3) - (6)].i), delete (yyvsp[(5) - (6)].i); }
    break;

  case 234:
/* Line 1792 of yacc.c  */
#line 445 "slghparse.y"
    { (yyval.tree)=slgh->pcode.createBitRange((yyvsp[(1) - (1)].bitsym)->getParentSymbol(),(yyvsp[(1) - (1)].bitsym)->getBitOffset(),(yyvsp[(1) - (1)].bitsym)->numBits()); }
    break;

  case 235:
/* Line 1792 of yacc.c  */
#line 446 "slghparse.y"
    { (yyval.tree) = slgh->pcode.createUserOp((yyvsp[(1) - (4)].useropsym),(yyvsp[(3) - (4)].param)); }
    break;

  case 236:
/* Line 1792 of yacc.c  */
#line 447 "slghparse.y"
    { if ((*(yyvsp[(3) - (4)].param)).size() < 2) { string errmsg = "Must at least two inputs to cpool"; yyerror(errmsg.c_str()); YYERROR; } (yyval.tree) = slgh->pcode.createVariadic(CPUI_CPOOLREF,(yyvsp[(3) - (4)].param)); }
    break;

  case 237:
/* Line 1792 of yacc.c  */
#line 449 "slghparse.y"
    { (yyval.starqual) = new StarQuality; (yyval.starqual)->size = *(yyvsp[(6) - (6)].i); delete (yyvsp[(6) - (6)].i); (yyval.starqual)->id=ConstTpl((yyvsp[(3) - (6)].spacesym)->getSpace()); }
    break;

  case 238:
/* Line 1792 of yacc.c  */
#line 450 "slghparse.y"
    { (yyval.starqual) = new StarQuality; (yyval.starqual)->size = 0; (yyval.starqual)->id=ConstTpl((yyvsp[(3) - (4)].spacesym)->getSpace()); }
    break;

  case 239:
/* Line 1792 of yacc.c  */
#line 451 "slghparse.y"
    { (yyval.starqual) = new StarQuality; (yyval.starqual)->size = *(yyvsp[(3) - (3)].i); delete (yyvsp[(3) - (3)].i); (yyval.starqual)->id=ConstTpl(slgh->getDefaultSpace()); }
    break;

  case 240:
/* Line 1792 of yacc.c  */
#line 452 "slghparse.y"
    { (yyval.starqual) = new StarQuality; (yyval.starqual)->size = 0; (yyval.starqual)->id=ConstTpl(slgh->getDefaultSpace()); }
    break;

  case 241:
/* Line 1792 of yacc.c  */
#line 454 "slghparse.y"
    { VarnodeTpl *sym = (yyvsp[(1) - (1)].startsym)->getVarnode(); (yyval.varnode) = new VarnodeTpl(ConstTpl(ConstTpl::j_curspace),sym->getOffset(),ConstTpl(ConstTpl::j_curspace_size)); delete sym; }
    break;

  case 242:
/* Line 1792 of yacc.c  */
#line 455 "slghparse.y"
    { VarnodeTpl *sym = (yyvsp[(1) - (1)].endsym)->getVarnode(); (yyval.varnode) = new VarnodeTpl(ConstTpl(ConstTpl::j_curspace),sym->getOffset(),ConstTpl(ConstTpl::j_curspace_size)); delete sym; }
    break;

  case 243:
/* Line 1792 of yacc.c  */
#line 456 "slghparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(ConstTpl::j_curspace),ConstTpl(ConstTpl::real,*(yyvsp[(1) - (1)].i)),ConstTpl(ConstTpl::j_curspace_size)); delete (yyvsp[(1) - (1)].i); }
    break;

  case 244:
/* Line 1792 of yacc.c  */
#line 457 "slghparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(ConstTpl::j_curspace),ConstTpl(ConstTpl::real,0),ConstTpl(ConstTpl::j_curspace_size)); yyerror("Parsed integer is too big (overflow)"); }
    break;

  case 245:
/* Line 1792 of yacc.c  */
#line 458 "slghparse.y"
    { (yyval.varnode) = (yyvsp[(1) - (1)].operandsym)->getVarnode(); (yyvsp[(1) - (1)].operandsym)->setCodeAddress(); }
    break;

  case 246:
/* Line 1792 of yacc.c  */
#line 459 "slghparse.y"
    { AddrSpace *spc = (yyvsp[(3) - (4)].spacesym)->getSpace(); (yyval.varnode) = new VarnodeTpl(ConstTpl(spc),ConstTpl(ConstTpl::real,*(yyvsp[(1) - (4)].i)),ConstTpl(ConstTpl::real,spc->getAddrSize())); delete (yyvsp[(1) - (4)].i); }
    break;

  case 247:
/* Line 1792 of yacc.c  */
#line 460 "slghparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(slgh->getConstantSpace()),ConstTpl(ConstTpl::j_relative,(yyvsp[(1) - (1)].labelsym)->getIndex()),ConstTpl(ConstTpl::real,sizeof(uintm))); (yyvsp[(1) - (1)].labelsym)->incrementRefCount(); }
    break;

  case 248:
/* Line 1792 of yacc.c  */
#line 461 "slghparse.y"
    { string errmsg = "Unknown jump destination: "+*(yyvsp[(1) - (1)].str); delete (yyvsp[(1) - (1)].str); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 249:
/* Line 1792 of yacc.c  */
#line 463 "slghparse.y"
    { (yyval.varnode) = (yyvsp[(1) - (1)].specsym)->getVarnode(); }
    break;

  case 250:
/* Line 1792 of yacc.c  */
#line 464 "slghparse.y"
    { (yyval.varnode) = (yyvsp[(1) - (1)].varnode); }
    break;

  case 251:
/* Line 1792 of yacc.c  */
#line 465 "slghparse.y"
    { string errmsg = "Unknown varnode parameter: "+*(yyvsp[(1) - (1)].str); delete (yyvsp[(1) - (1)].str); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 252:
/* Line 1792 of yacc.c  */
#line 466 "slghparse.y"
    { string errmsg = "Subtable not attached to operand: "+(yyvsp[(1) - (1)].subtablesym)->getName(); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 253:
/* Line 1792 of yacc.c  */
#line 468 "slghparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(slgh->getConstantSpace()),ConstTpl(ConstTpl::real,*(yyvsp[(1) - (1)].i)),ConstTpl(ConstTpl::real,0)); delete (yyvsp[(1) - (1)].i); }
    break;

  case 254:
/* Line 1792 of yacc.c  */
#line 469 "slghparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(slgh->getConstantSpace()),ConstTpl(ConstTpl::real,0),ConstTpl(ConstTpl::real,0)); yyerror("Parsed integer is too big (overflow)"); }
    break;

  case 255:
/* Line 1792 of yacc.c  */
#line 470 "slghparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(slgh->getConstantSpace()),ConstTpl(ConstTpl::real,*(yyvsp[(1) - (3)].i)),ConstTpl(ConstTpl::real,*(yyvsp[(3) - (3)].i))); delete (yyvsp[(1) - (3)].i); delete (yyvsp[(3) - (3)].i); }
    break;

  case 256:
/* Line 1792 of yacc.c  */
#line 471 "slghparse.y"
    { (yyval.varnode) = slgh->pcode.addressOf((yyvsp[(2) - (2)].varnode),0); }
    break;

  case 257:
/* Line 1792 of yacc.c  */
#line 472 "slghparse.y"
    { (yyval.varnode) = slgh->pcode.addressOf((yyvsp[(4) - (4)].varnode),*(yyvsp[(3) - (4)].i)); delete (yyvsp[(3) - (4)].i); }
    break;

  case 258:
/* Line 1792 of yacc.c  */
#line 474 "slghparse.y"
    { (yyval.varnode) = (yyvsp[(1) - (1)].specsym)->getVarnode(); }
    break;

  case 259:
/* Line 1792 of yacc.c  */
#line 475 "slghparse.y"
    { string errmsg = "Unknown assignment varnode: "+*(yyvsp[(1) - (1)].str); delete (yyvsp[(1) - (1)].str); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 260:
/* Line 1792 of yacc.c  */
#line 476 "slghparse.y"
    { string errmsg = "Subtable not attached to operand: "+(yyvsp[(1) - (1)].subtablesym)->getName(); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 261:
/* Line 1792 of yacc.c  */
#line 478 "slghparse.y"
    { (yyval.labelsym) = (yyvsp[(2) - (3)].labelsym); }
    break;

  case 262:
/* Line 1792 of yacc.c  */
#line 479 "slghparse.y"
    { (yyval.labelsym) = slgh->pcode.defineLabel( (yyvsp[(2) - (3)].str) ); }
    break;

  case 263:
/* Line 1792 of yacc.c  */
#line 481 "slghparse.y"
    { (yyval.varnode) = (yyvsp[(1) - (1)].specsym)->getVarnode(); }
    break;

  case 264:
/* Line 1792 of yacc.c  */
#line 482 "slghparse.y"
    { (yyval.varnode) = slgh->pcode.addressOf((yyvsp[(2) - (2)].varnode),0); }
    break;

  case 265:
/* Line 1792 of yacc.c  */
#line 483 "slghparse.y"
    { (yyval.varnode) = slgh->pcode.addressOf((yyvsp[(4) - (4)].varnode),*(yyvsp[(3) - (4)].i)); delete (yyvsp[(3) - (4)].i); }
    break;

  case 266:
/* Line 1792 of yacc.c  */
#line 484 "slghparse.y"
    { (yyval.varnode) = new VarnodeTpl(ConstTpl(slgh->getConstantSpace()),ConstTpl(ConstTpl::real,*(yyvsp[(1) - (3)].i)),ConstTpl(ConstTpl::real,*(yyvsp[(3) - (3)].i))); delete (yyvsp[(1) - (3)].i); delete (yyvsp[(3) - (3)].i); }
    break;

  case 267:
/* Line 1792 of yacc.c  */
#line 485 "slghparse.y"
    { string errmsg="Unknown export varnode: "+*(yyvsp[(1) - (1)].str); delete (yyvsp[(1) - (1)].str); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 268:
/* Line 1792 of yacc.c  */
#line 486 "slghparse.y"
    { string errmsg = "Subtable not attached to operand: "+(yyvsp[(1) - (1)].subtablesym)->getName(); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 269:
/* Line 1792 of yacc.c  */
#line 488 "slghparse.y"
    { (yyval.famsym) = (yyvsp[(1) - (1)].valuesym); }
    break;

  case 270:
/* Line 1792 of yacc.c  */
#line 489 "slghparse.y"
    { (yyval.famsym) = (yyvsp[(1) - (1)].valuemapsym); }
    break;

  case 271:
/* Line 1792 of yacc.c  */
#line 490 "slghparse.y"
    { (yyval.famsym) = (yyvsp[(1) - (1)].contextsym); }
    break;

  case 272:
/* Line 1792 of yacc.c  */
#line 491 "slghparse.y"
    { (yyval.famsym) = (yyvsp[(1) - (1)].namesym); }
    break;

  case 273:
/* Line 1792 of yacc.c  */
#line 492 "slghparse.y"
    { (yyval.famsym) = (yyvsp[(1) - (1)].varlistsym); }
    break;

  case 274:
/* Line 1792 of yacc.c  */
#line 494 "slghparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].varsym); }
    break;

  case 275:
/* Line 1792 of yacc.c  */
#line 495 "slghparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].specsym); }
    break;

  case 276:
/* Line 1792 of yacc.c  */
#line 496 "slghparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].operandsym); }
    break;

  case 277:
/* Line 1792 of yacc.c  */
#line 497 "slghparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].startsym); }
    break;

  case 278:
/* Line 1792 of yacc.c  */
#line 498 "slghparse.y"
    { (yyval.specsym) = (yyvsp[(1) - (1)].endsym); }
    break;

  case 279:
/* Line 1792 of yacc.c  */
#line 500 "slghparse.y"
    { (yyval.str) = new string; (*(yyval.str)) += (yyvsp[(1) - (1)].ch); }
    break;

  case 280:
/* Line 1792 of yacc.c  */
#line 501 "slghparse.y"
    { (yyval.str) = (yyvsp[(1) - (2)].str); (*(yyval.str)) += (yyvsp[(2) - (2)].ch); }
    break;

  case 281:
/* Line 1792 of yacc.c  */
#line 503 "slghparse.y"
    { (yyval.biglist) = (yyvsp[(2) - (3)].biglist); }
    break;

  case 282:
/* Line 1792 of yacc.c  */
#line 504 "slghparse.y"
    { (yyval.biglist) = new vector<intb>; (yyval.biglist)->push_back(intb(*(yyvsp[(1) - (1)].i))); delete (yyvsp[(1) - (1)].i); }
    break;

  case 283:
/* Line 1792 of yacc.c  */
#line 505 "slghparse.y"
    { (yyval.biglist) = new vector<intb>; (yyval.biglist)->push_back(-intb(*(yyvsp[(2) - (2)].i))); delete (yyvsp[(2) - (2)].i); }
    break;

  case 284:
/* Line 1792 of yacc.c  */
#line 507 "slghparse.y"
    { (yyval.biglist) = new vector<intb>; (yyval.biglist)->push_back(intb(*(yyvsp[(1) - (1)].i))); delete (yyvsp[(1) - (1)].i); }
    break;

  case 285:
/* Line 1792 of yacc.c  */
#line 508 "slghparse.y"
    { (yyval.biglist) = new vector<intb>; (yyval.biglist)->push_back(-intb(*(yyvsp[(2) - (2)].i))); delete (yyvsp[(2) - (2)].i); }
    break;

  case 286:
/* Line 1792 of yacc.c  */
#line 509 "slghparse.y"
    { if (*(yyvsp[(1) - (1)].str)!="_") { string errmsg = "Expecting integer but saw: "+*(yyvsp[(1) - (1)].str); delete (yyvsp[(1) - (1)].str); yyerror(errmsg.c_str()); YYERROR; }
                                  (yyval.biglist) = new vector<intb>; (yyval.biglist)->push_back((intb)0xBADBEEF); delete (yyvsp[(1) - (1)].str); }
    break;

  case 287:
/* Line 1792 of yacc.c  */
#line 511 "slghparse.y"
    { (yyval.biglist) = (yyvsp[(1) - (2)].biglist); (yyval.biglist)->push_back(intb(*(yyvsp[(2) - (2)].i))); delete (yyvsp[(2) - (2)].i); }
    break;

  case 288:
/* Line 1792 of yacc.c  */
#line 512 "slghparse.y"
    { (yyval.biglist) = (yyvsp[(1) - (3)].biglist); (yyval.biglist)->push_back(-intb(*(yyvsp[(3) - (3)].i))); delete (yyvsp[(3) - (3)].i); }
    break;

  case 289:
/* Line 1792 of yacc.c  */
#line 513 "slghparse.y"
    { if (*(yyvsp[(2) - (2)].str)!="_") { string errmsg = "Expecting integer but saw: "+*(yyvsp[(2) - (2)].str); delete (yyvsp[(2) - (2)].str); yyerror(errmsg.c_str()); YYERROR; }
                                  (yyval.biglist) = (yyvsp[(1) - (2)].biglist); (yyval.biglist)->push_back((intb)0xBADBEEF); delete (yyvsp[(2) - (2)].str); }
    break;

  case 290:
/* Line 1792 of yacc.c  */
#line 516 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(2) - (3)].strlist); }
    break;

  case 291:
/* Line 1792 of yacc.c  */
#line 517 "slghparse.y"
    { (yyval.strlist) = new vector<string>; (yyval.strlist)->push_back(*(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); }
    break;

  case 292:
/* Line 1792 of yacc.c  */
#line 519 "slghparse.y"
    { (yyval.strlist) = new vector<string>; (yyval.strlist)->push_back( *(yyvsp[(1) - (1)].str) ); delete (yyvsp[(1) - (1)].str); }
    break;

  case 293:
/* Line 1792 of yacc.c  */
#line 520 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(1) - (2)].strlist); (yyval.strlist)->push_back(*(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); }
    break;

  case 294:
/* Line 1792 of yacc.c  */
#line 521 "slghparse.y"
    { string errmsg = (yyvsp[(2) - (2)].anysym)->getName()+": redefined"; yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 295:
/* Line 1792 of yacc.c  */
#line 523 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(2) - (3)].strlist); }
    break;

  case 296:
/* Line 1792 of yacc.c  */
#line 525 "slghparse.y"
    { (yyval.strlist) = new vector<string>; (yyval.strlist)->push_back( *(yyvsp[(1) - (1)].str) ); delete (yyvsp[(1) - (1)].str); }
    break;

  case 297:
/* Line 1792 of yacc.c  */
#line 526 "slghparse.y"
    { (yyval.strlist) = new vector<string>; (yyval.strlist)->push_back( (yyvsp[(1) - (1)].anysym)->getName() ); }
    break;

  case 298:
/* Line 1792 of yacc.c  */
#line 527 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(1) - (2)].strlist); (yyval.strlist)->push_back(*(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); }
    break;

  case 299:
/* Line 1792 of yacc.c  */
#line 528 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(1) - (2)].strlist); (yyval.strlist)->push_back((yyvsp[(2) - (2)].anysym)->getName()); }
    break;

  case 300:
/* Line 1792 of yacc.c  */
#line 530 "slghparse.y"
    { (yyval.symlist) = (yyvsp[(2) - (3)].symlist); }
    break;

  case 301:
/* Line 1792 of yacc.c  */
#line 531 "slghparse.y"
    { (yyval.symlist) = new vector<SleighSymbol *>; (yyval.symlist)->push_back((yyvsp[(1) - (1)].valuesym)); }
    break;

  case 302:
/* Line 1792 of yacc.c  */
#line 532 "slghparse.y"
    { (yyval.symlist) = new vector<SleighSymbol *>; (yyval.symlist)->push_back((yyvsp[(1) - (1)].contextsym)); }
    break;

  case 303:
/* Line 1792 of yacc.c  */
#line 534 "slghparse.y"
    { (yyval.symlist) = new vector<SleighSymbol *>; (yyval.symlist)->push_back( (yyvsp[(1) - (1)].valuesym) ); }
    break;

  case 304:
/* Line 1792 of yacc.c  */
#line 535 "slghparse.y"
    { (yyval.symlist) = new vector<SleighSymbol *>; (yyval.symlist)->push_back((yyvsp[(1) - (1)].contextsym)); }
    break;

  case 305:
/* Line 1792 of yacc.c  */
#line 536 "slghparse.y"
    { (yyval.symlist) = (yyvsp[(1) - (2)].symlist); (yyval.symlist)->push_back((yyvsp[(2) - (2)].valuesym)); }
    break;

  case 306:
/* Line 1792 of yacc.c  */
#line 537 "slghparse.y"
    { (yyval.symlist) = (yyvsp[(1) - (2)].symlist); (yyval.symlist)->push_back((yyvsp[(2) - (2)].contextsym)); }
    break;

  case 307:
/* Line 1792 of yacc.c  */
#line 538 "slghparse.y"
    { string errmsg = *(yyvsp[(2) - (2)].str)+": is not a value pattern"; delete (yyvsp[(2) - (2)].str); yyerror(errmsg.c_str()); YYERROR; }
    break;

  case 308:
/* Line 1792 of yacc.c  */
#line 540 "slghparse.y"
    { (yyval.symlist) = (yyvsp[(2) - (3)].symlist); }
    break;

  case 309:
/* Line 1792 of yacc.c  */
#line 541 "slghparse.y"
    { (yyval.symlist) = new vector<SleighSymbol *>; (yyval.symlist)->push_back((yyvsp[(1) - (1)].varsym)); }
    break;

  case 310:
/* Line 1792 of yacc.c  */
#line 543 "slghparse.y"
    { (yyval.symlist) = new vector<SleighSymbol *>; (yyval.symlist)->push_back((yyvsp[(1) - (1)].varsym)); }
    break;

  case 311:
/* Line 1792 of yacc.c  */
#line 544 "slghparse.y"
    { if (*(yyvsp[(1) - (1)].str)!="_") { string errmsg = *(yyvsp[(1) - (1)].str)+": is not a varnode symbol"; delete (yyvsp[(1) - (1)].str); yyerror(errmsg.c_str()); YYERROR; }
				  (yyval.symlist) = new vector<SleighSymbol *>; (yyval.symlist)->push_back((SleighSymbol *)0); delete (yyvsp[(1) - (1)].str); }
    break;

  case 312:
/* Line 1792 of yacc.c  */
#line 546 "slghparse.y"
    { (yyval.symlist) = (yyvsp[(1) - (2)].symlist); (yyval.symlist)->push_back((yyvsp[(2) - (2)].varsym)); }
    break;

  case 313:
/* Line 1792 of yacc.c  */
#line 547 "slghparse.y"
    { if (*(yyvsp[(2) - (2)].str)!="_") { string errmsg = *(yyvsp[(2) - (2)].str)+": is not a varnode symbol"; delete (yyvsp[(2) - (2)].str); yyerror(errmsg.c_str()); YYERROR; }
                                  (yyval.symlist) = (yyvsp[(1) - (2)].symlist); (yyval.symlist)->push_back((SleighSymbol *)0); delete (yyvsp[(2) - (2)].str); }
    break;

  case 314:
/* Line 1792 of yacc.c  */
#line 550 "slghparse.y"
    { (yyval.param) = new vector<ExprTree *>; }
    break;

  case 315:
/* Line 1792 of yacc.c  */
#line 551 "slghparse.y"
    { (yyval.param) = new vector<ExprTree *>; (yyval.param)->push_back((yyvsp[(1) - (1)].tree)); }
    break;

  case 316:
/* Line 1792 of yacc.c  */
#line 552 "slghparse.y"
    { (yyval.param) = (yyvsp[(1) - (3)].param); (yyval.param)->push_back((yyvsp[(3) - (3)].tree)); }
    break;

  case 317:
/* Line 1792 of yacc.c  */
#line 554 "slghparse.y"
    { (yyval.strlist) = new vector<string>; }
    break;

  case 318:
/* Line 1792 of yacc.c  */
#line 555 "slghparse.y"
    { (yyval.strlist) = new vector<string>; (yyval.strlist)->push_back(*(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); }
    break;

  case 319:
/* Line 1792 of yacc.c  */
#line 556 "slghparse.y"
    { (yyval.strlist) = (yyvsp[(1) - (3)].strlist); (yyval.strlist)->push_back(*(yyvsp[(3) - (3)].str)); delete (yyvsp[(3) - (3)].str); }
    break;

  case 320:
/* Line 1792 of yacc.c  */
#line 558 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].spacesym); }
    break;

  case 321:
/* Line 1792 of yacc.c  */
#line 559 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].sectionsym); }
    break;

  case 322:
/* Line 1792 of yacc.c  */
#line 560 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].tokensym); }
    break;

  case 323:
/* Line 1792 of yacc.c  */
#line 561 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].useropsym); }
    break;

  case 324:
/* Line 1792 of yacc.c  */
#line 562 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].macrosym); }
    break;

  case 325:
/* Line 1792 of yacc.c  */
#line 563 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].subtablesym); }
    break;

  case 326:
/* Line 1792 of yacc.c  */
#line 564 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].valuesym); }
    break;

  case 327:
/* Line 1792 of yacc.c  */
#line 565 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].valuemapsym); }
    break;

  case 328:
/* Line 1792 of yacc.c  */
#line 566 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].contextsym); }
    break;

  case 329:
/* Line 1792 of yacc.c  */
#line 567 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].namesym); }
    break;

  case 330:
/* Line 1792 of yacc.c  */
#line 568 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].varsym); }
    break;

  case 331:
/* Line 1792 of yacc.c  */
#line 569 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].varlistsym); }
    break;

  case 332:
/* Line 1792 of yacc.c  */
#line 570 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].operandsym); }
    break;

  case 333:
/* Line 1792 of yacc.c  */
#line 571 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].startsym); }
    break;

  case 334:
/* Line 1792 of yacc.c  */
#line 572 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].endsym); }
    break;

  case 335:
/* Line 1792 of yacc.c  */
#line 573 "slghparse.y"
    { (yyval.anysym) = (yyvsp[(1) - (1)].bitsym); }
    break;


/* Line 1792 of yacc.c  */
#line 4377 "slghparse.tab.cpp"
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
#line 575 "slghparse.y"


int yyerror(const char *s)

{
  slgh->reportError(s,true);
  return 0;
}
