%{
#include <stdlib.h>
extern "C" int yylex();
%}
%option noyywrap

digit_ch   [0-9]
alpha_ch   [A-Za-z]
delimiter_ch    (\.\.\.)|[\,\.\:\;\(\)\[\]\{\}]
blank_ch    (" ")|(\\t)|(\\r)|(\\n)|(\\a)|(\\b)|(\\f)|(\\v)|(\\0)|(\\\\)
special_ch  [\!\@\#\$\%\^\&\"\'\~\|\_]
operator_ch    [\+\-\*\/\=\<\>]|(\>\=)|(\<=)|(\:\=)|(\!\=)
keyword if|then|while|do|read|write|call|begin|end|const|var|procedure|odd|integer|float|char|integerarray|for|to|downto|repeat|until
identifier  ({alpha_ch}|\_)({alpha_ch}|{digit_ch}|\_)*
integer {digit_ch}+
float   {integer}(\.{integer})?([E|e][+|-]?{integer})?
character   (\'({digit_ch}|{alpha_ch}|[\,\.\:\;\(\)\[\]\{\}]|{blank_ch}|{special_ch}|[\+\-\*\/\=\<\>])\')|(#{integer})
constant {integer}

%%
{keyword} {
    if(!strcmp(yytext, "if")) return IF;
    if(!strcmp(yytext, "then")) return THEN;
    if(!strcmp(yytext, "while")) return WHILE;
    if(!strcmp(yytext, "do")) return DO;
    if(!strcmp(yytext, "read")) return READ;
    if(!strcmp(yytext, "write")) return WRITE;
    if(!strcmp(yytext, "call")) return CALL;
    if(!strcmp(yytext, "begin")) return BEGIN_;
    if(!strcmp(yytext, "end")) return END;
    if(!strcmp(yytext, "const")) return CONST;
    if(!strcmp(yytext, "var")) return VAR;
    if(!strcmp(yytext, "procedure")) return PROCEDURE;
    if(!strcmp(yytext, "odd")) return ODD;
    if(!strcmp(yytext, "integer")) return INTEGER;
    if(!strcmp(yytext, "integerarray")) return INTEGERARRAY;
    if(!strcmp(yytext, "for")) return FOR;
    if(!strcmp(yytext, "to")) return TO;
    if(!strcmp(yytext, "downto")) return DOWNTO;
    if(!strcmp(yytext, "repeat")) return REPEAT;
    if(!strcmp(yytext, "until")) return UNTIL;
}

{identifier} {
    yylval.name = strdup(yytext);
    return ID;
}

{integer} {
    int t = atoi(yytext);
    yylval.valInteger = t;
    return CONSTANT;
}

{operator_ch} {
    if(!strcmp(yytext, "+")) return PLUS;
    if(!strcmp(yytext, "-")) return MINUS;
    if(!strcmp(yytext, "*")) return MULTIPLY;
    if(!strcmp(yytext, "/")) return DIVIDE;
    if(!strcmp(yytext, "=")) return EQ;
    if(!strcmp(yytext, ">")) return GT;
    if(!strcmp(yytext, "<")) return LT;
    if(!strcmp(yytext, ">=")) return GE;
    if(!strcmp(yytext, "<=")) return LE;
    if(!strcmp(yytext, ":=")) return ASSIGN;
    if(!strcmp(yytext, "#")) return NEQ;
}

{delimiter_ch} {
    if(!strcmp(yytext, "...")) return DOTS;
    if(!strcmp(yytext, ",")) return COMMA;
    if(!strcmp(yytext, ".")) return DOT;
    if(!strcmp(yytext, ";")) return SEMICOLON;
    if(!strcmp(yytext, ",")) return COMMA;
    if(!strcmp(yytext, "(")) return L0;
    if(!strcmp(yytext, ")")) return R0;
    if(!strcmp(yytext, "[")) return L1;
    if(!strcmp(yytext, "]")) return R1;
    if(!strcmp(yytext, "{")) return L2;
    if(!strcmp(yytext, "}")) return R2;
}

[" "\t\r] ;
[\n] {yrow++;}
. ;

%%