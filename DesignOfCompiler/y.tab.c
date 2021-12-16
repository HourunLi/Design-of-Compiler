/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "semantic_analysis.y" /* yacc.c:339  */

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


#line 271 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
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

/* Copy the second part of user declarations.  */

#line 413 "y.tab.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
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
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

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
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   103

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  49
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  71
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  128

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   303

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   224,   224,   224,   239,   245,   239,   261,   262,   265,
     266,   269,   275,   279,   288,   292,   303,   307,   318,   323,
     328,   334,   338,   344,   348,   354,   355,   358,   362,   368,
     371,   371,   381,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   398,   410,   416,   420,   427,   433,   446,   454,
     458,   462,   466,   470,   474,   480,   488,   490,   492,   488,
     508,   510,   512,   508,   526,   536,   542,   551,   562,   567,
     572,   579
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IF", "THEN", "WHILE", "DO", "READ",
  "WRITE", "CALL", "BEGIN_", "END", "CONST", "VAR", "PROCEDURE", "ODD",
  "INTEGER", "FLOAT", "CHAR", "INTEGERARRAY", "FOR", "TO", "DOWNTO",
  "REPEAT", "UNTIL", "ID", "CONSTANT", "PLUS", "MINUS", "MULTIPLY",
  "DIVIDE", "EQ", "GT", "LT", "GE", "LE", "ASSIGN", "NEQ", "DOTS", "COMMA",
  "DOT", "SEMICOLON", "COLON", "L0", "R0", "L1", "R1", "L2", "R2",
  "$accept", "program", "$@1", "subprogram", "$@2", "$@3", "const_cv",
  "const_def", "simple_const_def", "expression", "termlist", "term",
  "factor", "plusop", "multop", "var_cv", "var_def", "proc_cv", "$@4",
  "proc_head", "statement", "assignsta", "compsta", "statementlist",
  "ifsta", "condition", "relop", "whilesta", "forsta", "$@5", "$@6", "$@7",
  "$@8", "$@9", "$@10", "progcallsta", "readsta", "RDLIST", "writesta",
  "WTLIST", "checkpoint1", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303
};
# endif

#define YYPACT_NINF -86

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-86)))

#define YYTABLE_NINF -61

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -86,     4,    17,   -86,   -15,     8,    31,    22,    13,    16,
     -86,    37,   -86,    30,   -86,   -15,   -86,     6,   -86,   -86,
     -86,    38,   -86,    50,   -86,    41,    32,   -86,    26,   -13,
     -86,    25,    34,    44,    32,    48,    39,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,    17,   -86,    33,   -86,
     -86,   -86,   -86,    33,   -14,    -2,    21,   -86,   -19,    70,
     -13,    53,    54,   -86,   -86,   -10,    45,    33,    42,   -86,
      36,   -86,   -86,   -86,   -86,   -86,   -86,    33,   -19,   -86,
     -86,   -19,    -2,    32,    76,   -86,   -28,   -86,    -1,   -86,
     -86,    33,   -86,   -86,   -86,   -86,    21,   -86,   -86,    32,
      59,   -86,    60,   -86,    32,    64,   -86,   -86,   -86,   -86,
      66,    67,   -86,   -86,   -86,   -86,    33,    33,   -86,   -86,
     -86,   -86,    82,    86,    32,    32,   -86,   -86
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     7,     1,     0,     0,    25,     0,     0,     9,
       3,     0,     4,     0,     8,     0,    27,     0,    29,    11,
      10,     0,    26,     5,    28,     0,    41,    30,     0,     0,
      71,     0,     0,     0,    41,     0,     0,     6,    33,    34,
      35,    36,    37,    38,    39,    40,     7,    32,     0,    18,
      19,    21,    22,     0,     0,    12,    14,    16,     0,     0,
       0,     0,     0,    64,    44,     0,     0,     0,     0,    48,
       0,    49,    51,    52,    53,    54,    50,     0,     0,    23,
      24,     0,    13,    41,     0,    66,     0,    69,     0,    43,
      71,     0,    42,    31,    20,    47,    15,    17,    46,    41,
       0,    65,     0,    68,    41,    56,    55,    67,    70,    45,
       0,     0,    71,    71,    57,    61,     0,     0,    71,    71,
      58,    62,     0,     0,    41,    41,    59,    63
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -86,   -86,   -86,    47,   -86,   -86,   -86,    79,   -86,   -45,
      40,    18,    14,   -46,   -86,   -86,   -86,   -86,   -86,   -86,
     -34,   -86,   -86,   -86,   -86,    43,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,
     -85
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,     5,    18,    26,     6,     8,     9,    54,
      55,    56,    57,    58,    81,    12,    17,    23,    46,    27,
      37,    38,    39,    65,    40,    59,    77,    41,    42,   110,
     116,   122,   111,   117,   123,    43,    44,    86,    45,    88,
      60
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      64,    89,    48,    69,     3,   104,    49,    50,    70,    78,
       7,   100,    49,    50,    51,    52,   101,    71,    72,    73,
      74,    75,    92,    76,    53,    51,    52,   114,   115,     4,
      53,    90,    95,   120,   121,    29,    78,    30,   102,    31,
      32,    33,    34,   103,    11,    21,   105,    22,    10,    98,
      79,    80,    35,    13,    14,    15,    19,    36,    49,    50,
      51,    52,    16,    24,    25,   106,    28,    47,    61,    63,
     109,   118,   119,    66,    83,    67,    53,    62,    85,    87,
      94,    91,    99,    93,   107,   108,   -60,   112,   124,   113,
     126,   127,   125,    68,    20,    97,    96,     0,    82,     0,
       0,     0,     0,    84
};

static const yytype_int8 yycheck[] =
{
      34,    11,    15,    48,     0,    90,    25,    26,    53,    55,
      25,    39,    25,    26,    27,    28,    44,    31,    32,    33,
      34,    35,    67,    37,    43,    27,    28,   112,   113,    12,
      43,    41,    77,   118,   119,     3,    82,     5,    39,     7,
       8,     9,    10,    44,    13,    39,    91,    41,    40,    83,
      29,    30,    20,    31,    41,    39,    26,    25,    25,    26,
      27,    28,    25,    25,    14,    99,    25,    41,    43,    25,
     104,   116,   117,    25,     4,    36,    43,    43,    25,    25,
      44,    36,     6,    41,    25,    25,    22,    21,     6,    22,
     124,   125,     6,    46,    15,    81,    78,    -1,    58,    -1,
      -1,    -1,    -1,    60
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    50,    51,     0,    12,    52,    55,    25,    56,    57,
      40,    13,    64,    31,    41,    39,    25,    65,    53,    26,
      56,    39,    41,    66,    25,    14,    54,    68,    25,     3,
       5,     7,     8,     9,    10,    20,    25,    69,    70,    71,
      73,    76,    77,    84,    85,    87,    67,    41,    15,    25,
      26,    27,    28,    43,    58,    59,    60,    61,    62,    74,
      89,    43,    43,    25,    69,    72,    25,    36,    52,    58,
      58,    31,    32,    33,    34,    35,    37,    75,    62,    29,
      30,    63,    59,     4,    74,    25,    86,    25,    88,    11,
      41,    36,    58,    41,    44,    58,    60,    61,    69,     6,
      39,    44,    39,    44,    89,    58,    69,    25,    25,    69,
      78,    81,    21,    22,    89,    89,    79,    82,    58,    58,
      89,    89,    80,    83,     6,     6,    69,    69
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    49,    51,    50,    53,    54,    52,    55,    55,    56,
      56,    57,    58,    58,    59,    59,    60,    60,    61,    61,
      61,    62,    62,    63,    63,    64,    64,    65,    65,    66,
      67,    66,    68,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    70,    71,    72,    72,    73,    74,    74,    75,
      75,    75,    75,    75,    75,    76,    78,    79,    80,    77,
      81,    82,    83,    77,    84,    85,    86,    86,    87,    88,
      88,    89
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     3,     0,     0,     6,     0,     3,     1,
       3,     3,     1,     2,     1,     3,     1,     3,     1,     1,
       3,     1,     1,     1,     1,     0,     3,     1,     3,     0,
       0,     5,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     3,     3,     1,     4,     4,     3,     2,     1,
       1,     1,     1,     1,     1,     5,     0,     0,     0,    13,
       0,     0,     0,    13,     2,     4,     1,     3,     4,     1,
       3,     0
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
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
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
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

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

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
      yychar = yylex ();
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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 224 "semantic_analysis.y" /* yacc.c:1646  */
    { 
            curIns = 0;
            curLevel = 0;
            curSymbolTable = 0;
            curDataStackSize = PROG_OCCUPIED;
            gen("JMP", 0, 0);
          }
#line 1589 "y.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 231 "semantic_analysis.y" /* yacc.c:1646  */
    {
            for(int i = 0; i < curIns; i++){
                /*output the Pcode*/
              fprintf(out, "%s %d %d\n", code[i].f, code[i].l, code[i].a);
            }
          }
#line 1600 "y.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 239 "semantic_analysis.y" /* yacc.c:1646  */
    {
            /*Figure out the DataStackSize when compiling*/
                dataStackSize[curLevel] = curDataStackSize;
                /*for(int i = 0; i < curSymbolTable; i++){
                    fprintf(stderr, "%s %d\n", symbolTable[i].name, symbolTable[i].offset);
                }*/
             }
#line 1612 "y.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 245 "semantic_analysis.y" /* yacc.c:1646  */
    {
                if(curLevel > 0)
                /*the first ins of the current procedure*/
                    funcStack[curLevel - 1] -> codeAddr = curIns;
                else
                /*directly enter the master procedure*/
                    code[0].a = curIns;
                gen("INT", 0, dataStackSize[curLevel]);
             }
#line 1626 "y.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 253 "semantic_analysis.y" /* yacc.c:1646  */
    {
                backPatch((yyvsp[0].list), curIns);
                gen("OPR", 0, 0);
                curDataStackSize = PROG_OCCUPIED;
                deleteSymbol();
             }
#line 1637 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 270 "semantic_analysis.y" /* yacc.c:1646  */
    {
                    registSymbol((yyvsp[-2].name), CONSTANT_T, INTEGER_T, (yyvsp[0].valInteger));
                 }
#line 1645 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 276 "semantic_analysis.y" /* yacc.c:1646  */
    {
                (yyval.typeS) = (yyvsp[0].typeS);
             }
#line 1653 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 280 "semantic_analysis.y" /* yacc.c:1646  */
    {
                (yyval.typeS) = (yyvsp[-1].typeS);
                if((yyvsp[-1].typeS) == MINUS_T){
                    gen("OPR", 0, 1);
                }
             }
#line 1664 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 289 "semantic_analysis.y" /* yacc.c:1646  */
    {
               (yyval.typeS) = (yyvsp[0].typeS);
           }
#line 1672 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 293 "semantic_analysis.y" /* yacc.c:1646  */
    {
              (yyval.typeS) = (yyvsp[-2].typeS);
              if((yyvsp[-1].typeS) == PLUS_T){
                  gen("OPR", 0, 2);
              }else if((yyvsp[-1].typeS) == MINUS_T){
                  gen("OPR", 0, 3);
              }
           }
#line 1685 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 304 "semantic_analysis.y" /* yacc.c:1646  */
    {
           (yyval.typeS) = (yyvsp[0].typeS);
       }
#line 1693 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 308 "semantic_analysis.y" /* yacc.c:1646  */
    {
            (yyval.typeS) = (yyvsp[-2].typeS);
            if((yyvsp[-1].typeS) == MULTIPLY_T){
                gen("OPR", 0, 4);
            }else if((yyvsp[-1].typeS) == DIVIDE_T){
                gen("OPR", 0, 5);
            }
       }
#line 1706 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 319 "semantic_analysis.y" /* yacc.c:1646  */
    {
           lodid((yyvsp[0].name));
           (yyval.typeS) = lookupSymbol((yyvsp[0].name)) -> typeS;
         }
#line 1715 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 324 "semantic_analysis.y" /* yacc.c:1646  */
    {
            (yyval.typeS) = INTEGER_T;
            gen("LIT", 0, (yyvsp[0].valInteger));
         }
#line 1724 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 329 "semantic_analysis.y" /* yacc.c:1646  */
    {
            (yyval.typeS) = (yyvsp[-1].typeS);
         }
#line 1732 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 335 "semantic_analysis.y" /* yacc.c:1646  */
    {
             (yyval.typeS) = PLUS_T;
         }
#line 1740 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 339 "semantic_analysis.y" /* yacc.c:1646  */
    {
             (yyval.typeS) = MINUS_T;
         }
#line 1748 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 345 "semantic_analysis.y" /* yacc.c:1646  */
    {
             (yyval.typeS) = MULTIPLY_T;
         }
#line 1756 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 349 "semantic_analysis.y" /* yacc.c:1646  */
    {
             (yyval.typeS) = DIVIDE_T;
         }
#line 1764 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 359 "semantic_analysis.y" /* yacc.c:1646  */
    {
            registSymbol((yyvsp[0].name), VARIABLE_T, INTEGER_T, 0);
          }
#line 1772 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 363 "semantic_analysis.y" /* yacc.c:1646  */
    {
            registSymbol((yyvsp[0].name), VARIABLE_T, INTEGER_T, 0);
          }
#line 1780 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 368 "semantic_analysis.y" /* yacc.c:1646  */
    {
              
          }
#line 1788 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 371 "semantic_analysis.y" /* yacc.c:1646  */
    {
            curLevel++;
        }
#line 1796 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 374 "semantic_analysis.y" /* yacc.c:1646  */
    {
            deleteSymbol();
            curLevel--;
          }
#line 1805 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 382 "semantic_analysis.y" /* yacc.c:1646  */
    {
                funcStack[curLevel] = registSymbol((yyvsp[-1].name), CONSTANT_T, PROG_T, 0);
            }
#line 1813 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 387 "semantic_analysis.y" /* yacc.c:1646  */
    {(yyval.list) = (yyvsp[0].list);}
#line 1819 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 388 "semantic_analysis.y" /* yacc.c:1646  */
    {(yyval.list) = (yyvsp[0].list);}
#line 1825 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 389 "semantic_analysis.y" /* yacc.c:1646  */
    {(yyval.list) = (yyvsp[0].list);}
#line 1831 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 390 "semantic_analysis.y" /* yacc.c:1646  */
    {(yyval.list) = (yyvsp[0].list);}
#line 1837 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 391 "semantic_analysis.y" /* yacc.c:1646  */
    {(yyval.list) = (yyvsp[0].list);}
#line 1843 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 392 "semantic_analysis.y" /* yacc.c:1646  */
    {(yyval.list) = (yyvsp[0].list);}
#line 1849 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 393 "semantic_analysis.y" /* yacc.c:1646  */
    {(yyval.list) = (yyvsp[0].list);}
#line 1855 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 394 "semantic_analysis.y" /* yacc.c:1646  */
    {(yyval.list) = (yyvsp[0].list);}
#line 1861 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 395 "semantic_analysis.y" /* yacc.c:1646  */
    { }
#line 1867 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 399 "semantic_analysis.y" /* yacc.c:1646  */
    {
                Symbol *tmp = lookupSymbol((yyvsp[-2].name));
                if(tmp == NULL)
                  ERROR(0, (yyvsp[-2].name));
                if(tmp -> typeH != VARIABLE_T)
                  ERROR(1, (yyvsp[-2].name));
                stoid((yyvsp[-2].name));
            }
#line 1880 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 411 "semantic_analysis.y" /* yacc.c:1646  */
    {
              (yyval.list) = (yyvsp[-1].list);
          }
#line 1888 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 417 "semantic_analysis.y" /* yacc.c:1646  */
    {
                    backPatch((yyvsp[0].list), curIns);
                }
#line 1896 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 421 "semantic_analysis.y" /* yacc.c:1646  */
    {
                    backPatch((yyvsp[-3].list), (yyvsp[-1].codeAddr));
                    (yyval.list) = (yyvsp[0].list);
                }
#line 1905 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 428 "semantic_analysis.y" /* yacc.c:1646  */
    {
          (yyval.list) = linkList((yyvsp[-2].list), (yyvsp[0].list));
      }
#line 1913 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 434 "semantic_analysis.y" /* yacc.c:1646  */
    {
                switch (yyvsp[-1].typeS){
                    case EQ_T: gen("OPR", 0, 8);break;
                    case NEQ_T: gen("OPR", 0, 9);break;
                    case GT_T: gen("OPR", 0, 12);break;
                    case GE_T: gen("OPR", 0, 11);break;
                    case LT_T: gen("OPR", 0, 10);break;
                    case LE_T: gen("OPR", 0, 13);break;
                }
                (yyval.list) = makeList(&code[curIns].a);
                gen("JPC", 0, 0);
            }
#line 1930 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 447 "semantic_analysis.y" /* yacc.c:1646  */
    {
                gen("OPR", 0, 6);
                (yyval.list) = makeList(&code[curIns].a);
                gen("JPC", 0, 0);
            }
#line 1940 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 455 "semantic_analysis.y" /* yacc.c:1646  */
    {
            (yyval.typeS) = EQ_T;
        }
#line 1948 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 459 "semantic_analysis.y" /* yacc.c:1646  */
    {
            (yyval.typeS) = NEQ_T;
        }
#line 1956 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 463 "semantic_analysis.y" /* yacc.c:1646  */
    {
            (yyval.typeS) = GT_T;
        }
#line 1964 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 467 "semantic_analysis.y" /* yacc.c:1646  */
    {
            (yyval.typeS) = LT_T;
        }
#line 1972 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 471 "semantic_analysis.y" /* yacc.c:1646  */
    {
            (yyval.typeS) = GE_T;
        }
#line 1980 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 475 "semantic_analysis.y" /* yacc.c:1646  */
    {
            (yyval.typeS) = LE_T;
        }
#line 1988 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 481 "semantic_analysis.y" /* yacc.c:1646  */
    {
                backPatch((yyvsp[0].list), (yyvsp[-3].codeAddr));
                (yyval.list) = (yyvsp[-2].list);
                gen("JMP", 0, (yyvsp[-3].codeAddr));
           }
#line 1998 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 488 "semantic_analysis.y" /* yacc.c:1646  */
    {
                stoid((yyvsp[-2].name));
          }
#line 2006 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 490 "semantic_analysis.y" /* yacc.c:1646  */
    {
                lodid((yyvsp[-5].name));
          }
#line 2014 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 492 "semantic_analysis.y" /* yacc.c:1646  */
    {
              /*check boundary*/
                gen("OPR", 0, 10);
                gen("JPC", 0, 0);
          }
#line 2024 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 496 "semantic_analysis.y" /* yacc.c:1646  */
    {
              /*the next code of statement is modifying the value of ID*/
                backPatch((yyvsp[0].list), curIns);
                lodid((yyvsp[-11].name));
                gen("LIT", 0, 1);
                gen("OPR", 0, 2);
                stoid((yyvsp[-11].name));
              	/*goto the first chckpoint1 and check the boundary*/
                gen("JMP", 0, (yyvsp[-6].codeAddr));
              	/*the nextlist of forsta equals to the nextlist of falselist of expression*/
                (yyval.list) = makeList(&code[((yyvsp[-3].codeAddr))+1].a);
          }
#line 2041 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 508 "semantic_analysis.y" /* yacc.c:1646  */
    {
                stoid((yyvsp[-2].name));
          }
#line 2049 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 510 "semantic_analysis.y" /* yacc.c:1646  */
    {
                lodid((yyvsp[-5].name));
          }
#line 2057 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 512 "semantic_analysis.y" /* yacc.c:1646  */
    {
                gen("OPR", 0, 12);
                gen("JPC", 0, 0);
          }
#line 2066 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 515 "semantic_analysis.y" /* yacc.c:1646  */
    {
                backPatch((yyvsp[0].list), curIns);
                lodid((yyvsp[-11].name));
                gen("LIT", 0, 1);
                gen("OPR", 0, 3);
                stoid((yyvsp[-11].name));
                gen("JMP", 0, (yyvsp[-6].codeAddr));
                (yyval.list) = makeList(&code[((yyvsp[-3].codeAddr))+1].a);
          }
#line 2080 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 527 "semantic_analysis.y" /* yacc.c:1646  */
    {
                Symbol *tmp = lookupSymbol((yyvsp[0].name));
                if(tmp -> typeS != PROG_T){
                  ERROR(5, (yyvsp[0].name));
                }
                gen("CAL", curLevel - tmp->level, tmp->codeAddr);
            }
#line 2092 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 537 "semantic_analysis.y" /* yacc.c:1646  */
    {
              
          }
#line 2100 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 542 "semantic_analysis.y" /* yacc.c:1646  */
    {
            Symbol *tmp = lookupSymbol((yyvsp[0].name));
            if(tmp == NULL)
                ERROR(0, (yyvsp[0].name));
            if(tmp -> typeH != VARIABLE_T)
                ERROR(1, (yyvsp[0].name));
            gen("OPR", 0, 16);
            stoid((yyvsp[0].name));
        }
#line 2114 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 551 "semantic_analysis.y" /* yacc.c:1646  */
    {
            Symbol *tmp = lookupSymbol((yyvsp[0].name));
            if(tmp == NULL)
                ERROR(0, (yyvsp[0].name));
            if(tmp -> typeH != VARIABLE_T)
                ERROR(1, (yyvsp[0].name));
            gen("OPR", 0, 16);
            stoid((yyvsp[0].name));
        }
#line 2128 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 562 "semantic_analysis.y" /* yacc.c:1646  */
    {
            gen("OPR", 0, 15);
          }
#line 2136 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 568 "semantic_analysis.y" /* yacc.c:1646  */
    {
           Symbol *tmp = lodid((yyvsp[0].name));
           gen("OPR", 0, 14);
         }
#line 2145 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 573 "semantic_analysis.y" /* yacc.c:1646  */
    {
           Symbol *tmp = lodid((yyvsp[0].name));
           gen("OPR", 0, 14);
         }
#line 2154 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 579 "semantic_analysis.y" /* yacc.c:1646  */
    {
              (yyval.codeAddr) = curIns;
            }
#line 2162 "y.tab.c" /* yacc.c:1646  */
    break;


#line 2166 "y.tab.c" /* yacc.c:1646  */
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

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

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
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 583 "semantic_analysis.y" /* yacc.c:1906  */

#include "LA.cpp"
int main(int argc, char **argv){
    yyin = fopen(argv[1], "r");
    out = fopen(argv[2], "w");
    yyparse();
    return 0;
}
