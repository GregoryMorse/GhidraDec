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

#ifndef YY_PCODE_D_SOURCE_REPOS_GHIDRADEC_DECOMPILE_PCODEPARSE_TAB_HPP_INCLUDED
# define YY_PCODE_D_SOURCE_REPOS_GHIDRADEC_DECOMPILE_PCODEPARSE_TAB_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int pcodedebug;
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
     OP_FNOTEQUAL = 261,
     OP_FEQUAL = 262,
     OP_NOTEQUAL = 263,
     OP_EQUAL = 264,
     OP_FGREATEQUAL = 265,
     OP_FLESSEQUAL = 266,
     OP_FGREAT = 267,
     OP_FLESS = 268,
     OP_SGREAT = 269,
     OP_SLESSEQUAL = 270,
     OP_SGREATEQUAL = 271,
     OP_SLESS = 272,
     OP_LESSEQUAL = 273,
     OP_GREATEQUAL = 274,
     OP_SRIGHT = 275,
     OP_RIGHT = 276,
     OP_LEFT = 277,
     OP_FSUB = 278,
     OP_FADD = 279,
     OP_FDIV = 280,
     OP_FMULT = 281,
     OP_SREM = 282,
     OP_SDIV = 283,
     OP_ZEXT = 284,
     OP_CARRY = 285,
     OP_BORROW = 286,
     OP_SEXT = 287,
     OP_SCARRY = 288,
     OP_SBORROW = 289,
     OP_NAN = 290,
     OP_ABS = 291,
     OP_SQRT = 292,
     OP_CEIL = 293,
     OP_FLOOR = 294,
     OP_ROUND = 295,
     OP_INT2FLOAT = 296,
     OP_FLOAT2FLOAT = 297,
     OP_TRUNC = 298,
     OP_NEW = 299,
     BADINTEGER = 300,
     GOTO_KEY = 301,
     CALL_KEY = 302,
     RETURN_KEY = 303,
     IF_KEY = 304,
     ENDOFSTREAM = 305,
     LOCAL_KEY = 306,
     INTEGER = 307,
     STRING = 308,
     SPACESYM = 309,
     USEROPSYM = 310,
     VARSYM = 311,
     OPERANDSYM = 312,
     STARTSYM = 313,
     ENDSYM = 314,
     LABELSYM = 315
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 27 "D:/Source/Repos/GhidraDec/decompile/pcodeparse.y"

  uintb *i;
  string *str;
  vector<ExprTree *> *param;
  StarQuality *starqual;
  VarnodeTpl *varnode;
  ExprTree *tree;
  vector<OpTpl *> *stmt;
  ConstructTpl *sem;

  SpaceSymbol *spacesym;
  UserOpSymbol *useropsym;
  LabelSymbol *labelsym;
  StartSymbol *startsym;
  EndSymbol *endsym;
  OperandSymbol *operandsym;
  VarnodeSymbol *varsym;
  SpecificSymbol *specsym;


/* Line 2058 of yacc.c  */
#line 138 "D:/Source/Repos/GhidraDec/decompile/pcodeparse.tab.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE pcodelval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int pcodeparse (void *YYPARSE_PARAM);
#else
int pcodeparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int pcodeparse (void);
#else
int pcodeparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_PCODE_D_SOURCE_REPOS_GHIDRADEC_DECOMPILE_PCODEPARSE_TAB_HPP_INCLUDED  */
