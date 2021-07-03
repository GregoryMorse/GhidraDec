/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
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
/* Line 2058 of yacc.c  */
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


/* Line 2058 of yacc.c  */
#line 214 "slghparse.tab.hh"
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
