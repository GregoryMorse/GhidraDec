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
/* Line 2058 of yacc.c  */
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


/* Line 2058 of yacc.c  */
#line 86 "grammar.tab.h"
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
