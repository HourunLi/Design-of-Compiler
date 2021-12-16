%{
#include <cstdio>
#include <iostream>
#include <vector>
#include <utility>
#include <stdlib.h>
#include <string.h>
using namespace std;

int yrow = 1;

extern "C" int yylex();
#define CONSTANT_T 1
#define VARIABLE_T 2
#define INTEGER_T 1
#define FLOAT_T 2
#define CHAR_T 3
#define INTEGERARRAY_T 4
#define PROG_T 5
#define PLUS_T 6
#define MINUS_T 7
#define MULTIPLY_T 8
#define DIVIDE_T 9
#define EQ_T 1
#define NEQ_T 2
#define GT_T 3
#define GE_T 4
#define LT_T 5
#define LE_T 6

FILE *out;
#define MAX_INSTRUCTIONS 100000000
#define MAX_SYMBOL_TABLE_ENTRIES 10000
#define MAX_LEVEL 5
#define PROG_OCCUPIED 3
#define YYSTYPE SuperNode

struct Instructions{
    char *f;
    int l;
    int a;
}code[MAX_INSTRUCTIONS];

struct Symbol{
    char* name;
    int typeH;
    int typeS;
    int level;
    int offset;
    int size;
    int codeAddr;
    int valInteger;
}symbolTable[MAX_SYMBOL_TABLE_ENTRIES];

struct Node{
    int *ptr;
    Node *pre, *next;
};

struct SuperNode{
    char* name;
    int typeH; /*the first tyoe*/
    int typeS; /*the second type*/
    int codeAddr;/*the location of the first code of a procedure*/
    int valInteger; /*the value of variable of constant*/
    Node *list; /*the nextlist*/
};

int curIns;
int curLevel, curSymbolTable, curDisplayTable;
int curDataStackSize;

int dataStackSize[MAX_LEVEL];
Symbol *funcStack[MAX_LEVEL];

int yyerror(const char *x){
    for(int i = 0; i < curIns; i++){
        fprintf(stderr, "%s %d %d\n", code[i].f, code[i].l, code[i].a);
    }
	fprintf(stderr, "row %d: %s\n", yrow, x);
	exit(0);
}

/*generate the code*/
void gen(char *x, int l, int a){
    code[curIns++] = (Instructions){strdup(x),l,a};
    /*make the f in lower case*/
    for(int i = 0; code[curIns - 1].f[i] != 0; i++){
        if(code[curIns - 1].f[i] <= 'Z')
            code[curIns - 1].f[i] += 'a' - 'A';
    }
}

Symbol *registSymbol(char *name, int typeH, int typeS, int val){
    /*only the variable will occupy the stack*/
    if(typeH == VARIABLE_T)
        curDataStackSize++;
    Symbol tmp;
    tmp.name = strdup(name);
    tmp.typeH = typeH;
    tmp.typeS = typeS;
    tmp.level = curLevel;
    tmp.size = 1;
    if(typeH == VARIABLE_T){
        /*if it is the first variable or the local first variable in a procedure*/
        if(curSymbolTable == 0 || symbolTable[curSymbolTable - 1].level != curLevel || symbolTable[curSymbolTable - 1].typeH != VARIABLE_T)
            tmp.offset = PROG_OCCUPIED;
        else
            tmp.offset = symbolTable[curSymbolTable - 1].offset + symbolTable[curSymbolTable - 1].size;
    }else if(typeH == CONSTANT_T){
        tmp.offset = 0;
        tmp.valInteger = val;
    }
    symbolTable[curSymbolTable++] = tmp;
    return &symbolTable[curSymbolTable - 1];
}

Symbol *lookupSymbol(char *name){
    for(int i = curSymbolTable - 1; i >= 0; i--)
        if(strcmp(name, symbolTable[i].name) == 0)
        return &symbolTable[i];
    return NULL;
}

void deleteSymbol(){
    while(symbolTable[curSymbolTable - 1].level == curLevel)
        curSymbolTable --;
}

Symbol* lodid(char *name){
    //fprintf(stderr, "%s\n", name);
    Symbol *tmp = lookupSymbol(name);
    if(tmp == NULL){
        fprintf(stderr, "404 %s\n", name);
        exit(0); 
    }
    if(tmp -> typeH == VARIABLE_T)
        gen("LOD", curLevel - tmp->level, tmp->offset);
    else{
        /*If the var is a constant var*/
        gen("LIT", 0, tmp->valInteger);
    }
    return tmp;
}

void stoid(char *name){
    Symbol *tmp = lookupSymbol(name);
    if(tmp == NULL){
        fprintf(stderr, "404 %s\n", name);
        exit(0);
    }gen("STO", curLevel - tmp->level, tmp->offset);
}
/*There will be a cycle*/
struct Node* linkList(Node *ptr1, Node *ptr2){
    if(ptr1 == NULL) return ptr2;
    if(ptr2 == NULL) return ptr1;
    Node *ptr1n = ptr1 -> next;
    Node *ptr2n = ptr2 -> next;
    ptr1 -> next = ptr2n;
    ptr2n -> pre = ptr1;
    ptr2 -> next = ptr1n;
    ptr1n -> pre = ptr2;
    return ptr1;
}

struct Node* makeList(int *ptr){
    Node *res = (Node *)malloc(sizeof(Node));
    res -> ptr = ptr;
    res -> pre = res -> next = res;
    return res;
}

struct Node* insList(Node *list, int *ptr){
    Node *list2 = makeList(ptr);
    return linkList(list, list2);
}

void backPatch(Node *list, int x){
    if(list == NULL) return;
    Node *tmp = list;
    do{
        *(tmp->ptr) = x;
        tmp = tmp->next;
    }while(tmp != list);
}

void ERROR(int type, char *s){
    if(type == 0){ // ID not found
        fprintf(stderr, "row %d: Identifier %s is not registered.\n", yrow, s);
    }else if(type == 1){ //try to modify constant
        fprintf(stderr, "row %d: Identifier %s is a constant/function_address which shouldn't be modified.\n", yrow, s);
    }else if(type == 2){ //array with index
        fprintf(stderr, "row %d: Identifier %s is not an array.\n", yrow, s);
    }else if(type == 3){ //index is not an integer
        fprintf(stderr, "row %d: index is not an array.\n", yrow);
    }else if(type == 4){ //odd
        fprintf(stderr, "row %d: the object followed by odd should be an integer.\n", yrow);
    }else if(type == 5){ // call statement
        fprintf(stderr, "row %d: %s is not a program name.\n", yrow, s);
    }
    exit(0);
}

%}
%define parse.error verbose
%token IF THEN WHILE DO READ WRITE CALL BEGIN_ END CONST VAR PROCEDURE ODD INTEGER FLOAT CHAR INTEGERARRAY FOR TO DOWNTO REPEAT UNTIL
%token ID CONSTANT PLUS MINUS MULTIPLY DIVIDE EQ GT LT GE LE ASSIGN NEQ
%token DOTS COMMA DOT SEMICOLON COLON L0 R0 L1 R1 L2 R2

/*the name of identifier*/
%type<name> ID
/*the type of expression*/
%type<typeS> plusop multop expression termlist term factor relop
/*the value of a caontant variable*/
%type<valInteger> CONSTANT 
/*the nextlist, used for backpatch*/
%type<list> statement assignsta compsta statementlist ifsta condition whilesta forsta progcallsta readsta writesta
/*the anchor, used for backpatch*/
%type<codeAddr> checkpoint1

%start program

%%
program : { 
            curIns = 0;
            curLevel = 0;
            curSymbolTable = 0;
            curDataStackSize = PROG_OCCUPIED;
            gen("JMP", 0, 0);
          }
          subprogram DOT{
            for(int i = 0; i < curIns; i++){
                /*output the Pcode*/
              fprintf(out, "%s %d %d\n", code[i].f, code[i].l, code[i].a);
            }
          }
        ;

subprogram : const_cv var_cv {
            /*Figure out the DataStackSize when compiling*/
                dataStackSize[curLevel] = curDataStackSize;
                /*for(int i = 0; i < curSymbolTable; i++){
                    fprintf(stderr, "%s %d\n", symbolTable[i].name, symbolTable[i].offset);
                }*/
             } proc_cv {
                if(curLevel > 0)
                /*the first ins of the current procedure*/
                    funcStack[curLevel - 1] -> codeAddr = curIns;
                else
                /*directly enter the master procedure*/
                    code[0].a = curIns;
                gen("INT", 0, dataStackSize[curLevel]);
             } statement {
                backPatch($6, curIns);
                gen("OPR", 0, 0);
                curDataStackSize = PROG_OCCUPIED;
                deleteSymbol();
             }
           ;

const_cv : 
         | CONST const_def SEMICOLON
         ;

const_def : simple_const_def
          | simple_const_def COMMA const_def
          ;

simple_const_def : ID EQ CONSTANT
                 {
                    registSymbol($1, CONSTANT_T, INTEGER_T, $3);
                 }
                 ;

expression : termlist
             {
                $$ = $1;
             }
           | plusop termlist
             {
                $$ = $1;
                if($1 == MINUS_T){
                    gen("OPR", 0, 1);
                }
             }
           ;

termlist : term
           {
               $$ = $1;
           }
         | termlist plusop term
           {
              $$ = $1;
              if($2 == PLUS_T){
                  gen("OPR", 0, 2);
              }else if($2 == MINUS_T){
                  gen("OPR", 0, 3);
              }
           }  
         ;

term : factor
       {
           $$ = $1;
       }
     | term multop factor
       {
            $$ = $1;
            if($2 == MULTIPLY_T){
                gen("OPR", 0, 4);
            }else if($2 == DIVIDE_T){
                gen("OPR", 0, 5);
            }
       }
     ;

factor : ID
         {
           lodid($1);
           $$ = lookupSymbol($1) -> typeS;
         }
       | CONSTANT
         {
            $$ = INTEGER_T;
            gen("LIT", 0, $1);
         }
       | L0 expression R0
         {
            $$ = $2;
         }
       ;

plusop : PLUS
         {
             $$ = PLUS_T;
         }
       | MINUS
         {
             $$ = MINUS_T;
         }
       ;

multop : MULTIPLY
         {
             $$ = MULTIPLY_T;
         }
       | DIVIDE
         {
             $$ = DIVIDE_T;
         }
       ;  

var_cv : 
       | VAR var_def SEMICOLON
       ;

var_def : ID
          {
            registSymbol($1, VARIABLE_T, INTEGER_T, 0);
          }
        | var_def COMMA ID
          {
            registSymbol($3, VARIABLE_T, INTEGER_T, 0);
          }
        ;

proc_cv : {
              
          } 
        | proc_cv proc_head{
            curLevel++;
        } subprogram SEMICOLON
          {
            deleteSymbol();
            curLevel--;
          }
        ;


proc_head : PROCEDURE ID SEMICOLON
            {
                funcStack[curLevel] = registSymbol($2, CONSTANT_T, PROG_T, 0);
            }
          ;

statement : assignsta {$$ = $1;}
          | compsta{$$ = $1;}
          | ifsta{$$ = $1;}
          | whilesta{$$ = $1;}
          | forsta{$$ = $1;}
          | progcallsta{$$ = $1;}
          | readsta{$$ = $1;}
          | writesta{$$ = $1;}
	      |        { }
          ;

assignsta : ID ASSIGN expression
            {
                Symbol *tmp = lookupSymbol($1);
                if(tmp == NULL)
                  ERROR(0, $1);
                if(tmp -> typeH != VARIABLE_T)
                  ERROR(1, $1);
                stoid($1);
            }
          ;


compsta : BEGIN_ statementlist END
          {
              $$ = $2;
          }
        ;

statementlist : statement
                {
                    backPatch($1, curIns);
                }
              | statementlist SEMICOLON checkpoint1 statement 
                {
                    backPatch($1, $3);
                    $$ = $4;
                }
              ;

ifsta : IF condition THEN statement
      {
          $$ = linkList($2, $4);
      }
      ;

condition : expression relop expression
            {
                switch $2{
                    case EQ_T: gen("OPR", 0, 8);break;
                    case NEQ_T: gen("OPR", 0, 9);break;
                    case GT_T: gen("OPR", 0, 12);break;
                    case GE_T: gen("OPR", 0, 11);break;
                    case LT_T: gen("OPR", 0, 10);break;
                    case LE_T: gen("OPR", 0, 13);break;
                }
                $$ = makeList(&code[curIns].a);
                gen("JPC", 0, 0);
            }
          | ODD expression
            {
                gen("OPR", 0, 6);
                $$ = makeList(&code[curIns].a);
                gen("JPC", 0, 0);
            }
          ;

relop : EQ
        {
            $$ = EQ_T;
        }
      | NEQ
        {
            $$ = NEQ_T;
        }
      | GT
        {
            $$ = GT_T;
        }
      | LT
        {
            $$ = LT_T;
        }
      | GE
        {
            $$ = GE_T;
        }
      | LE
        {
            $$ = LE_T;
        }
      ;

whilesta : WHILE checkpoint1 condition DO statement 
           {
                backPatch($5, $2);
                $$ = $3;
                gen("JMP", 0, $2);
           }
         ;

forsta : FOR ID ASSIGN expression{
                stoid($2);
          } TO checkpoint1{
                lodid($2);
          } expression checkpoint1{
              /*check boundary*/
                gen("OPR", 0, 10);
                gen("JPC", 0, 0);
          } DO statement {
              /*the next code of statement is modifying the value of ID*/
                backPatch($13, curIns);
                lodid($2);
                gen("LIT", 0, 1);
                gen("OPR", 0, 2);
                stoid($2);
              	/*goto the first chckpoint1 and check the boundary*/
                gen("JMP", 0, $7);
              	/*the nextlist of forsta equals to the nextlist of falselist of expression*/
                $$ = makeList(&code[($10)+1].a);
          }
       | FOR ID ASSIGN expression{
                stoid($2);
          } DOWNTO checkpoint1{
                lodid($2);
          } expression checkpoint1{
                gen("OPR", 0, 12);
                gen("JPC", 0, 0);
          } DO statement {
                backPatch($13, curIns);
                lodid($2);
                gen("LIT", 0, 1);
                gen("OPR", 0, 3);
                stoid($2);
                gen("JMP", 0, $7);
                $$ = makeList(&code[($10)+1].a);
          }
       ;
         
progcallsta : CALL ID
            {
                Symbol *tmp = lookupSymbol($2);
                if(tmp -> typeS != PROG_T){
                  ERROR(5, $2);
                }
                gen("CAL", curLevel - tmp->level, tmp->codeAddr);
            }
            ;

readsta : READ L0 RDLIST R0
          {
              
          }
        ;

RDLIST : ID{
            Symbol *tmp = lookupSymbol($1);
            if(tmp == NULL)
                ERROR(0, $1);
            if(tmp -> typeH != VARIABLE_T)
                ERROR(1, $1);
            gen("OPR", 0, 16);
            stoid($1);
        }
       | RDLIST COMMA ID {
            Symbol *tmp = lookupSymbol($3);
            if(tmp == NULL)
                ERROR(0, $3);
            if(tmp -> typeH != VARIABLE_T)
                ERROR(1, $3);
            gen("OPR", 0, 16);
            stoid($3);
        }
      ;

writesta : WRITE L0 WTLIST R0{
            gen("OPR", 0, 15);
          }
        ;

WTLIST : ID
         {
           Symbol *tmp = lodid($1);
           gen("OPR", 0, 14);
         }
       | WTLIST COMMA ID
         {
           Symbol *tmp = lodid($3);
           gen("OPR", 0, 14);
         }
        ;

checkpoint1 :{
              $$ = curIns;
            }
          
%%
#include "LA.cpp"
int main(int argc, char **argv){
    yyin = fopen(argv[1], "r");
    out = fopen(argv[2], "w");
    yyparse();
    return 0;
}