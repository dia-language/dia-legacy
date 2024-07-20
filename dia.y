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

%token DIA_PLUS               "+"
%token DIA_MINUS              "-"
%token DIA_MUL                "*"
%token DIA_DIV                "/"

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
