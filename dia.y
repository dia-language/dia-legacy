%require "3.8"

%{
#include <stdio.h>
#include <stdint.h>

// Pre-defined functions
#include "dia.h"

#undef DIA_DEBUG
#define DIA_DEBUG(...) (DIA_VERBOSE_LEVEL < 1 ? : fprintf(stderr, "[DIA:dia.y] " __VA_ARGS__))

extern uint8_t DIA_VERBOSE_LEVEL;
extern char* yytext;
extern int yylex();
extern void yyerror(const char*);
%}


/* The lines start with %token will generate .tab.h file
 * it will possible to use it with bison
 */
%union {
  int i;
  double f;
  char* str;
}

%token <i> DIA_INTEGER
%token <f> DIA_DOUBLE
%token <i> DIA_HEXADECIMAL
%token <str> DIA_STRING

%token DIA_MAIN_FUNC          "main"
%token DIA_ALLOC              "="

%token DIA_BIND               "."
%token DIA_NEXT               ";"

%token DIA_OPEN_PARENTHESIS   "("
%token DIA_CLOSE_PARENTHESIS  ")"
%token DIA_OPEN_BRACKET       "["
%token DIA_CLOSE_BRACKET      "]"

%token <str> DIA_IDENTIFIER

%%

/* enjoy = "Enjoy the flexivity of the Dia language!"
 *
 * main = puts("Hello World!");
 *        enjoy.puts;
 *
 */

dia: custom_func dia                       { DIA_DEBUG("Welcome to the Dia World! main function with custom functions detected!\n"); }
   | DIA_MAIN_FUNC DIA_ALLOC dia_expr      { dia_main(); DIA_DEBUG("Welcome to the Dia World! main function detected!\n"); }
   ;

custom_func:
           | DIA_IDENTIFIER DIA_ALLOC dia_expr custom_func   { DIA_DEBUG("Dia: custom function, which name is '%s'\n", $1); }
           ;

dia_expr: dia_expr DIA_BIND dia_expr                 { DIA_DEBUG("Reserved for DIA_BIND\n"); }
        | dia_expr DIA_NEXT dia_expr                 { DIA_DEBUG("Reserved for DIA_NEXT\n"); }
        | dia_expr DIA_NEXT                          { DIA_DEBUG("Semicolon Terminated\n"); }
        | dia_function
        ;

dia_function: DIA_IDENTIFIER DIA_OPEN_PARENTHESIS dia_parameters DIA_CLOSE_PARENTHESIS    { DIA_DEBUG("Function Name: %s\n", $1); }
            | DIA_IDENTIFIER                           { DIA_DEBUG("Function Name: %s\n", $1); }
            | token
            ;

dia_if: "if" DIA_OPEN_PARENTHESIS dia_expr DIA_CLOSE_PARENTHESIS dia_else;

dia_else: "else" dia_if
        | "else" DIA_OPEN_PARENTHESIS dia_expr DIA_CLOSE_PARENTHESIS
        ;

dia_parameters: dia_function                        { DIA_DEBUG("Function Parameter\n"); }
              | dia_function "," dia_parameters
              ;

token: DIA_STRING         { $<str>$ = dia_string($1); }
     | DIA_INTEGER        { $<i>$ = dia_integer($1); }
     | DIA_DOUBLE         { $<f>$ = dia_double($1); }
     ;


%%

void yyerror(const char *str) {
  puts("diac: There was an error while parsing the code");
  printf("[DIA:Error] %s\n", str);

  return;
}

