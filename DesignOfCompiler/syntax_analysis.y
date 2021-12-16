%{
#include <cstdio>
#include <iostream>
#include <vector>
#include <string.h>
#include "node.h"
using namespace std;
#define YYSTYPE node*

int num = 0;
FILE *opraw, *optree;
extern FILE * yyin;
extern int yylineno;
extern "C" int yylex();

node* newnode(char *str){
    node* tmp = new node;
    tmp->text = strdup(str);
    return tmp;

}

void setchild(node *fa, node *ch){
    (fa->ch).push_back(ch);
}

void outtree(node *cur, node *fa, int faid){
    int id = ++num;
    fprintf(optree, "Node%d[label=\"%s\"]\n", id, cur->text);
    if(fa != NULL)
        fprintf(optree, "Node%d -> Node%d\n", faid, id);
    fprintf(optree, "\n");
    for(auto i: cur -> ch){
        outtree(i, cur, id);
    }
}

void yyerror(const char *str){
    fprintf(stderr,"Error | Line: %d\n%s\n",yylineno,str);
}

%}
%define parse.error verbose
%token IF THEN WHILE DO READ WRITE CALL BEGIN_ END CONST VAR PROCEDURE ODD INTEGER FLOAT CHAR INTEGERARRAY FOR TO DOWNTO REPEAT UNTIL
%token ID CONSTANT PLUS MINUS MULTIPLY DIVIDE EQ GT LT GE LE ASSIGN NEQ
%token DOTS COMMA DOT SEMICOLON COLON L0 R0 L1 R1 L2 R2

%start program

%%

program : subprogram DOT
          {
              $$ = newnode("<程序>");
              setchild($$, $1);
              setchild($$, $2);
              fprintf(opraw, "<程序> -> <分程序>.\n");
              fprintf(optree, "digraph pic{\n");
              outtree($$, NULL, 0);
              fprintf(optree, "}\n");
          }
        ;

subprogram : const_cv var_cv proc_cv statement
             {
                 $$ = newnode("<分程序>");
                 setchild($$, $1);
                 setchild($$, $2);
                 setchild($$, $3);
                 setchild($$, $4);
                 fprintf(opraw, "<分程序> -> <常量说明部分> <变量说明部分> <过程说明部分> <语句>\n");
             }
           ;

const_cv : {
                $$ = newnode("<常量说明部分>");
                node *tmp = newnode("<空>");
                setchild($$, tmp);
                fprintf(opraw, "<常量说明部分> -> <空>\n");
           }
         | CONST const_def SEMICOLON
           {
               $$ = newnode("<常量说明部分>");
               setchild($$, $1);
               setchild($$, $2);
               setchild($$, $3);
               fprintf(opraw, "<常量说明部分> -> const <常量定义>;\n");
           }
         ;

const_def : simple_const_def
            {
                $$ = newnode("<常量定义>");
                setchild($$, $1);
                fprintf(opraw, "<常量定义> -> <普通常量定义>\n");
            } 
          | simple_const_def COMMA const_def
            {
                $$ = newnode("<常量定义>");
                setchild($$, $1);
                setchild($$, $2);
                setchild($$, $3);
                fprintf(opraw, "<常量定义> -> <普通常量定义> , <常量定义>\n");
            }
          ;

simple_const_def : ID EQ CONSTANT
                 {
                     $$ = newnode("<普通常量定义>");
                     setchild($$, $1);
                     setchild($$, $2);
                     setchild($$, $3);
                     fprintf(opraw, "<普通常量定义> -> <标识符> = <无符号整数>\n");
                 }
                 ;


explist : expression
          {
              $$ = newnode("<表达式列表>");
              setchild($$, $1);
              fprintf(opraw, "<表达式列表> -> <表达式>\n");
          }
        | expression COMMA explist
          {
              $$ = newnode("<表达式列表>");
              setchild($$, $1);
              setchild($$, $2);
              setchild($$, $3);
              fprintf(opraw, "<表达式列表> -> <表达式>, <表达式列表>\n");
          }
        ;


expression : termlist
             {
                $$ = newnode("<表达式>");
                setchild($$, $1);
                fprintf(opraw, "<表达式> -> <项列表>\n");
             }
           | plusop termlist
             {
                $$ = newnode("<表达式>");
                setchild($$, $1);
                setchild($$, $2);
                fprintf(opraw, "<表达式> -> <加法运算符> <项列表>\n");
             }
           ;

termlist : term
           {
               $$ = newnode("<项列表>");
               setchild($$, $1); 
               fprintf(opraw, "<项列表> -> <项>\n");
           }
         | term plusop termlist
           {
               $$ = newnode("<项列表>");
               setchild($$, $1);
               setchild($$, $2);
               setchild($$, $3);
               fprintf(opraw, "<项列表> -> <项> <加法运算符> <项列表>\n");
           }  
         ;

term : factor
       {
           $$ = newnode("<项>");
           setchild($$, $1);
           fprintf(opraw, "<项> -> <因子>\n");
       }
     | factor multop term
       {
           $$ = newnode("<项>");
           setchild($$, $1);
           setchild($$, $2);
           setchild($$, $3);
           fprintf(opraw, "<项> -> <因子> <乘法运算符> <项>\n");
       }
     ;

factor : ID
         {
             $$ = newnode("因子");
             setchild($$, $1);
             fprintf(opraw, "<因子> -> <标识符>\n");
         }
       | CONSTANT
         {
             $$ = newnode("因子");
             setchild($$, $1);
             fprintf(opraw, "<因子> -> <无符号整数>\n");
         }
       | L0 expression R0
         {
             $$ = newnode("因子");
             setchild($$, $1);
             setchild($$, $2);
             setchild($$, $3);
             fprintf(opraw, "<因子> -> ( <表达式> )\n");
         }
       ;

plusop : PLUS
         {
             $$ = newnode("加法运算符");
             setchild($$, $1);
             fprintf(opraw, "<加法运算符> -> +\n");
         }
       | MINUS
         {
             $$ = newnode("加法运算符");
             setchild($$, $1);
             fprintf(opraw, "<加法运算符> -> -\n");
         }
       ;

multop : MULTIPLY
         {
             $$ = newnode("乘法运算符");
             setchild($$, $1);
             fprintf(opraw, "<乘法运算符> -> *\n");
         }
       | DIVIDE
         {
             $$ = newnode("乘法运算符");
             setchild($$, $1);
             fprintf(opraw, "<乘法运算符> -> /\n");
         }
       ;  

var_cv : {
             $$ = newnode("<变量说明部分>");
             node *tmp = newnode("<空>");
             setchild($$, tmp);
             fprintf(opraw, "<变量说明部分> -> <空>\n");
         }
       | VAR var_def SEMICOLON
         {
             $$ = newnode("<变量说明部分>");
             setchild($$, $1);
             setchild($$, $2);
             setchild($$, $3);
             fprintf(opraw, "<变量说明部分> -> VAR <变量定义> ;\n");
         }
       ;

var_def : ID
          {
              $$ = newnode("<变量定义>");
              setchild($$, $1);
              fprintf(opraw, "<变量定义> -> <标识符> \n");

          }
        | ID COMMA var_def
          {
              $$ = newnode("<变量定义>");
              setchild($$, $1);
              setchild($$, $2);
              setchild($$, $3);
              fprintf(opraw, "<变量定义> -> <标识符>, <变量定义> \n");
          }
        ;

proc_cv : {
              $$ = newnode("<过程说明部分>");
              node *tmp = newnode("<空>");
              setchild($$, tmp);
              fprintf(opraw, "<过程说明部分> -> <空>\n");
          } 
        | proc_head subprogram SEMICOLON proc_cv 
          {
              $$ = newnode("<过程说明部分");
              setchild($$, $1);
              setchild($$, $2);
              setchild($$, $3);
              setchild($$, $4);
              fprintf(opraw, "<过程说明部分> -> <过程首部> <分程序> ; <过程说明部分>\n");
          }
        ;

proc_head : PROCEDURE ID SEMICOLON
            {
                $$ = newnode("<过程首部>");
                setchild($$, $1);
                setchild($$, $2);
                setchild($$, $3);
                fprintf(opraw, "<过程首部> -> PROCEDURE <标识符>;\n");
            }
          ;

statement : assignsta
            {
                $$ = newnode("<语句>");
                setchild($$, $1);
                fprintf(opraw, "<语句> -> <赋值语句>\n");
            }
          | compsta
            {
                $$ = newnode("<语句>");
                setchild($$, $1);
                fprintf(opraw, "<语句> -> <复合语句>\n");
            }
          | ifsta
            {
                $$ = newnode("<语句>");
                setchild($$, $1);
                fprintf(opraw, "<语句> -> <条件语句>\n");
            }
          | whilesta
            {
                $$ = newnode("<语句>");
                setchild($$, $1);
                fprintf(opraw, "<语句> -> <while语句>\n");
            }
          | forsta
            {
                $$ = newnode("<语句>");
                setchild($$, $1);
                fprintf(opraw, "<语句> -> <for语句>\n");
            }
          | progcallsta
            {
                $$ = newnode("<语句>");
                setchild($$, $1);
                fprintf(opraw, "<语句> -> <过程调用语句>\n");
            }
          | readsta
            {
                $$ = newnode("<语句>");
                setchild($$, $1);
                fprintf(opraw, "<语句> -> <读语句>\n");
            }
          | writesta
            {
                $$ = newnode("<语句>");
                setchild($$, $1);
                fprintf(opraw, "<语句> -> <写语句>\n");
            }
          |
            {
                $$ = newnode("<语句>");
                node *tmp = newnode("<空>");
                setchild($$, tmp);
                fprintf(opraw, "<语句> -> <空>\n");        
            }
          ;

assignsta : ID ASSIGN expression
            {
                $$ = newnode("<赋值语句>");
                setchild($$, $1);
                setchild($$, $2);
                setchild($$, $3);
                fprintf(opraw, "<赋值语句> -> <标识符> := <表达式>\n");
            }
          ;

compsta : BEGIN_ statementlist END
          {
              $$ = newnode("<复合语句>");
              setchild($$, $1);
              setchild($$, $2);
              setchild($$, $3);
              fprintf(opraw, "<复合语句> -> BEGIN <语句列表> END\n");
          }
        ;

statementlist : statement
                {
                    $$ = newnode("<语句列表>");
                    setchild($$, $1);
                    fprintf(opraw, "<语句列表> -> <语句>\n");
                }
              | statement SEMICOLON statementlist
                {
                    $$ = newnode("<语句列表>");
                    setchild($$, $1);
                    setchild($$, $2);
                    setchild($$, $3);
                    fprintf(opraw, "<语句列表> -> <语句> ; <语句列表>\n");
                }
              ;

ifsta : IF condition THEN statement
      {
          $$ = newnode("<条件语句>");
          setchild($$, $1);
          setchild($$, $2);
          setchild($$, $3);
          setchild($$, $4);
          fprintf(opraw, "<条件语句> -> IF <条件> THEN <语句>\n");
      }
      ;

condition : expression relop expression
            {
                $$ = newnode("<条件>");
                setchild($$, $1);
                setchild($$, $2);
                setchild($$, $3);
                fprintf(opraw, "<条件> -> <表达式> <关系运算符> <表达式>\n");
            }
          | ODD expression
            {
                $$ = newnode("<条件>");
                setchild($$, $1);
                setchild($$, $2);
                fprintf(opraw, "<条件> -> ODD <表达式>\n");
            }
          ;

relop : EQ
        {
            $$ = newnode("<关系运算符>");
            setchild($$, $1);
            fprintf(opraw, "<关系运算符> -> =\n");
        }
      | NEQ
        {
            $$ = newnode("<关系运算符>");
            setchild($$, $1);
            fprintf(opraw, "<关系运算符> -> #\n");
        }
      | GT
        {
            $$ = newnode("<关系运算符>");
            setchild($$, $1);
            fprintf(opraw, "<关系运算符> -> >\n");
        }
      | LT
        {
            $$ = newnode("<关系运算符>");
            setchild($$, $1);
            fprintf(opraw, "<关系运算符> -> <\n");
        }
      | GE
        {
            $$ = newnode("<关系运算符>");
            setchild($$, $1);
            fprintf(opraw, "<关系运算符> -> >=\n");
        }
      | LE
        {
            $$ = newnode("<关系运算符>");
            setchild($$, $1);
            fprintf(opraw, "<关系运算符> -> <=\n");
        }
      ;

whilesta : WHILE condition DO statement
           {
                $$ = newnode("<while语句>");
                setchild($$, $1);
                setchild($$, $2);
                setchild($$, $3);
                setchild($$, $4);
                fprintf(opraw, "<while语句> -> WHILE <条件> DO <语句>\n");
           }
         ;

forsta : FOR ID ASSIGN expression TO expression DO statement
         {
             $$ = newnode("<for语句>");
             setchild($$, $1);
             setchild($$, $2);
             setchild($$, $3);
             setchild($$, $4);
             setchild($$, $5);
             setchild($$, $6);
             setchild($$, $7);
             setchild($$, $8);
             fprintf(opraw, "<for语句> -> FOR <标识符> := <表达式> TO <表达式> DO <语句>\n");
         }
       | FOR ID ASSIGN expression DOWNTO expression DO statement
         {
             $$ = newnode("<for语句>");
             setchild($$, $1);
             setchild($$, $2);
             setchild($$, $3);
             setchild($$, $4);
             setchild($$, $5);
             setchild($$, $6);
             setchild($$, $7);
             setchild($$, $8);
             fprintf(opraw, "<for语句> -> FOR <标识符> := <表达式> DOWNTO <表达式> DO <语句>\n");
         }
       ;

progcallsta : CALL ID
            {
                $$ = newnode("<过程调用语句>");
                setchild($$, $1);
                setchild($$, $2);
                fprintf(opraw, "<过程调用语句> -> CALL <标识符>");
            }
            ;

readsta : READ L0 IDLIST R0
          {
              $$ = newnode("<读语句>");
              setchild($$, $1);
              setchild($$, $2);
              setchild($$, $3);
              setchild($$, $4);
              fprintf(opraw, "<读语句> -> READ ( <标识符列表> )\n");
          }
        ;

writesta : WRITE L0 explist R0
           {
               $$ = newnode("<写语句>");
               setchild($$, $1);
               setchild($$, $2);
               setchild($$, $3);
               setchild($$, $4);
               fprintf(opraw, "<写语句> -> WRITE ( <表达式列表> )\n");
           }
        ;


IDLIST : ID
         {
             $$ = newnode("<标识符列表>");
             setchild($$, $1);
             fprintf(opraw, "<标识符列表> -> <标识符>\n");
         }
       | ID COMMA IDLIST
         {
             $$ = newnode("<标识符列表>");
             setchild($$, $1);
             setchild($$, $2);
             setchild($$, $3);
             fprintf(opraw, "<标识符列表> -> <标识符> , <标识符列表>\n");
         }
       ;
%%

int main(int argc, char **argv){
    yyin = fopen(argv[1], "r");
    opraw = fopen(argv[2], "w");
    optree = fopen(argv[3], "w");
    yyparse();
    return 0;
}