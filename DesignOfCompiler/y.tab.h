/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IF = 258,
    THEN = 259,
    WHILE = 260,
    DO = 261,
    READ = 262,
    WRITE = 263,
    CALL = 264,
    BEGIN_ = 265,
    END = 266,
    CONST = 267,
    VAR = 268,
    PROCEDURE = 269,
    ODD = 270,
    INTEGER = 271,
    FLOAT = 272,
    CHAR = 273,
    INTEGERARRAY = 274,
    FOR = 275,
    TO = 276,
    DOWNTO = 277,
    REPEAT = 278,
    UNTIL = 279,
    ID = 280,
    CONSTANT = 281,
    PLUS = 282,
    MINUS = 283,
    MULTIPLY = 284,
    DIVIDE = 285,
    EQ = 286,
    GT = 287,
    LT = 288,
    GE = 289,
    LE = 290,
    ASSIGN = 291,
    NEQ = 292,
    DOTS = 293,
    COMMA = 294,
    DOT = 295,
    SEMICOLON = 296,
    COLON = 297,
    L0 = 298,
    R0 = 299,
    L1 = 300,
    R1 = 301,
    L2 = 302,
    R2 = 303
  };
#endif
/* Tokens.  */
#define IF 258
#define THEN 259
#define WHILE 260
#define DO 261
#define READ 262
#define WRITE 263
#define CALL 264
#define BEGIN_ 265
#define END 266
#define CONST 267
#define VAR 268
#define PROCEDURE 269
#define ODD 270
#define INTEGER 271
#define FLOAT 272
#define CHAR 273
#define INTEGERARRAY 274
#define FOR 275
#define TO 276
#define DOWNTO 277
#define REPEAT 278
#define UNTIL 279
#define ID 280
#define CONSTANT 281
#define PLUS 282
#define MINUS 283
#define MULTIPLY 284
#define DIVIDE 285
#define EQ 286
#define GT 287
#define LT 288
#define GE 289
#define LE 290
#define ASSIGN 291
#define NEQ 292
#define DOTS 293
#define COMMA 294
#define DOT 295
#define SEMICOLON 296
#define COLON 297
#define L0 298
#define R0 299
#define L1 300
#define R1 301
#define L2 302
#define R2 303

/* Value type.  */


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
