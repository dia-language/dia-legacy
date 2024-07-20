%require "3.8"

%{
#include <stdio.h>
#include <stdint.h>

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

%code requires {
#include "dia.h"
#include "dia_calculation.h"
}

%union {
  int i;
  double f;
  char* str;

  dia_node* node;
}

%token <node> DIA_INTEGER
%token <node> DIA_DOUBLE
%token <node> DIA_HEXADECIMAL
%token <node> DIA_STRING

%token DIA_MAIN_FUNC          "main"
%token DIA_ALLOC              "="

/* Calculation */
%token DIA_PLUS               "+"
%token DIA_MINUS              "-"
%token DIA_MUL                "*"
%token DIA_DIV                "/"
%token DIA_MOD                "%"

%token DIA_LOGICAL_AND
%token DIA_LOGICAL_OR
%token DIA_LOGICAL_NOT

%token DIA_EQUAL              "=="
%token DIA_GREATER_EQUAL      ">="
%token DIA_GREATER            ">"
%token DIA_LESS_EQUAL         "<="
%token DIA_LESS               "<"

%token DIA_BIT_AND            "&"
%token DIA_BIT_XOR
%token DIA_BIT_OR             "|"
%token DIA_BIT_NOT            "~"

/* Calculation: End */

%token DIA_COMMA              ","
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

dia: custom_func dia                      { DIA_DEBUG("Welcome to the Dia World! main function with custom functions detected!\n"); }
   | DIA_MAIN_FUNC DIA_ALLOC dia_expr     { dia_main($<node>3); }
   ;

custom_func:
           | DIA_IDENTIFIER DIA_ALLOC dia_expr custom_func   { DIA_DEBUG("Dia: custom function, which name is '%s'\n", $1); }
           ;

dia_expr: dia_expr DIA_BIND dia_expr
          {
            dia_node* _previous = $<node>1;
            dia_node* _next = $<node>3;

            dia_node* _tmp = _next->next_parameter;
            _next->next_parameter = _previous;
            _previous->next_parameter = _tmp;

            DIA_DEBUG("DIA_BIND: To weave function parameter to the adjacent(next) function.\n");

            dia_debug_function_descriptor(_next);
            $<node>$ = _next;
          }
        | dia_expr DIA_NEXT dia_expr
          {
            $<node>1->next_function = $<node>3;
            DIA_DEBUG("Reserved for DIA_NEXT\n");
            $<node>$ = $<node>1;
          }
        | dia_expr DIA_NEXT
          {
            DIA_DEBUG("Semicolon Terminated\n");
            $<node>$ = $<node>1;
          }
        | dia_function
        ;

dia_function: DIA_IDENTIFIER DIA_OPEN_PARENTHESIS dia_parameters DIA_CLOSE_PARENTHESIS
              {
                dia_node* _node = (dia_node*)malloc(sizeof(dia_node));

                _node->name = strdup($1);
                _node->next_parameter = $<node>3;

                dia_debug_function_descriptor(_node);

                $<node>$ = _node;
              }
            | DIA_IDENTIFIER
              {
                dia_node* _node = (dia_node*)malloc(sizeof(dia_node));
                _node->name = strdup($1);

                dia_debug_function_descriptor(_node);

                $<node>$ = _node;
              }
            | dia_calculation
            | token
            ;

dia_if: "if" DIA_OPEN_PARENTHESIS dia_expr DIA_CLOSE_PARENTHESIS dia_else
      ;

dia_else: "else" dia_if
        | "else" DIA_OPEN_PARENTHESIS dia_expr DIA_CLOSE_PARENTHESIS
        ;

dia_parameters: dia_function
                {
                  DIA_DEBUG("+- Function Parameter: %s\n", $<node>1->name);

                  $<node>$ = $<node>1;
                }
              | dia_parameters DIA_COMMA dia_function
                {
                  dia_node* __node = $<node>1;
                  for (; __node->next_parameter != NULL; __node = __node->next_parameter)
                    DIA_DEBUG("__node->name : %s\n", __node->name);
                  __node->next_parameter = $<node>3;

                  DIA_DEBUG("+- Function Parameter: %s ... %s\n", $<node>1->name, $<node>3->name);
                  $<node>$ = $<node>1;
                }
              ;

/* Calculation */
dia_calculation: dia_arithmetic
               | dia_logical
               | dia_comparison
               | dia_bitwise
               ;

dia_arithmetic: token DIA_PLUS token      { $<node>$ = dia_plus($<node>1, $<node>3); }
              | token DIA_MINUS token     { $<node>$ = dia_minus($<node>1, $<node>3); }
              | token DIA_MUL token       { $<node>$ = dia_mul($<node>1, $<node>3); }
              | token DIA_DIV token       { $<node>$ = dia_div($<node>1, $<node>3); }
              | token DIA_MOD token       { $<node>$ = dia_mod($<node>1, $<node>3); }
              ;

dia_logical: token DIA_LOGICAL_AND token  { $<node>$ = dia_logical_and($<node>1, $<node>3); }
           | token DIA_LOGICAL_OR token   { $<node>$ = dia_logical_or($<node>1, $<node>3); }
           |       DIA_LOGICAL_NOT token  { $<node>$ = dia_logical_not($<node>2); }
           ;

dia_comparison: token DIA_EQUAL token         { $<node>$ = dia_equal($<node>1, $<node>3); }
              | token DIA_GREATER_EQUAL token { $<node>$ = dia_greater_equal($<node>1, $<node>3); }
              | token DIA_GREATER token       { $<node>$ = dia_greater($<node>1, $<node>3); }
              | token DIA_LESS_EQUAL token    { $<node>$ = dia_less_equal($<node>1, $<node>3); }
              | token DIA_LESS token          { $<node>$ = dia_less($<node>1, $<node>3); }
              ;

dia_bitwise: token DIA_BIT_AND token   { $<node>$ = dia_bit_and($<node>1, $<node>3); }
           | token DIA_BIT_OR token    { $<node>$ = dia_bit_or($<node>1, $<node>3); }
           | token DIA_BIT_XOR token   { $<node>$ = dia_bit_xor($<node>1, $<node>3); }
           |       DIA_BIT_NOT token   { $<node>$ = dia_bit_not($<node>2); }

token: DIA_STRING         { $<node>$ = dia_string($1); }
     | DIA_INTEGER        { $<node>$ = dia_integer($1); }
     | DIA_DOUBLE         { $<node>$ = dia_double($1); }
     ;


%%

void yyerror(const char *str) {
  puts("diac: There was an error while parsing the code");
  printf("[DIA:Error] %s\n", str);

  return;
}
