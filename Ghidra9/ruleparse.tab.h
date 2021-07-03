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
/* Line 2058 of yacc.c  */
#line 28 "ruleparse.y"

  char ch;
  string *str;
  int8 *big;
  int4 id;
  OpCode opcode;
  vector<OpCode> *opcodelist;
  ConstraintGroup *group;
  RHSConstant *rhsconst;


/* Line 2058 of yacc.c  */
#line 150 "ruleparse.tab.h"
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
