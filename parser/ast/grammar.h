/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_EGOR_CLIONPROJECTS_LLP_PARSER_AST_GRAMMAR_H_INCLUDED
# define YY_YY_HOME_EGOR_CLIONPROJECTS_LLP_PARSER_AST_GRAMMAR_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    T_END = 0,                     /* "end"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_CREATE_TABLE = 258,          /* "CREATE_TABLE"  */
    T_DROP_TABLE = 259,            /* "DROP_TABLE"  */
    T_FOR = 260,                   /* "FOR"  */
    T_FILTER = 261,                /* "FILTER"  */
    T_RETURN = 262,                /* "RETURN"  */
    T_REMOVE = 263,                /* "REMOVE"  */
    T_INSERT = 264,                /* "INSERT"  */
    T_UPDATE = 265,                /* "UPDATE"  */
    T_IN = 266,                    /* "IN"  */
    T_QUOTED_STRING = 267,         /* "q-string"  */
    T_NULL = 268,                  /* "null"  */
    T_TRUE = 269,                  /* "true"  */
    T_FALSE = 270,                 /* "false"  */
    T_STRING = 271,                /* "string"  */
    T_INTEGER = 272,               /* "int"  */
    T_DOUBLE = 273,                /* "double"  */
    T_EQ = 274,                    /* "=="  */
    T_NE = 275,                    /* "!="  */
    T_LT = 276,                    /* "<"  */
    T_GT = 277,                    /* ">"  */
    T_LE = 278,                    /* "<="  */
    T_GE = 279,                    /* ">="  */
    T_COMMA = 280,                 /* ","  */
    T_OPEN = 281,                  /* "("  */
    T_CLOSE = 282,                 /* ")"  */
    T_OBJECT_OPEN = 283,           /* "{"  */
    T_OBJECT_CLOSE = 284,          /* "}"  */
    T_COLON = 285,                 /* ":"  */
    T_NOT = 286,                   /* "not operator"  */
    T_AND = 287,                   /* "and operator"  */
    T_OR = 288,                    /* "or operator"  */
    T_CONTAINS = 289               /* "contains function"  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 15 "/home/egor/CLionProjects/llp/parser/ast/grammar.y"

    struct AstNode* node;
    double d_val;
    int i_val;
    char* s_val;
    bool b_val;

#line 106 "/home/egor/CLionProjects/llp/parser/ast/grammar.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_HOME_EGOR_CLIONPROJECTS_LLP_PARSER_AST_GRAMMAR_H_INCLUDED  */
