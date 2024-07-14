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

dia: custom_func dia                       { DIA_DEBUG("Welcome to the Dia World! main function with custom functions detected!\n"); }
   | DIA_MAIN_FUNC DIA_ALLOC dia_expr      { dia_main($<node>3); DIA_DEBUG("Welcome to the Dia World! main function detected!\n"); }
   ;

custom_func:
           | DIA_IDENTIFIER DIA_ALLOC dia_expr custom_func   { DIA_DEBUG("Dia: custom function, which name is '%s'\n", $1); }
           ;

dia_expr: dia_expr DIA_BIND dia_expr                 {
                                                        DIA_DEBUG("DIA_BIND: To weave function parameter to the adjacent(next) function.\n");
                                                     }
        | dia_expr DIA_NEXT dia_expr                 { DIA_DEBUG("Reserved for DIA_NEXT\n"); }
        | dia_expr DIA_NEXT                          { DIA_DEBUG("Semicolon Terminated\n"); }
        | dia_function
        ;

dia_function: DIA_IDENTIFIER DIA_OPEN_PARENTHESIS dia_parameters DIA_CLOSE_PARENTHESIS    {
                                                                                            dia_node* _node = (dia_node*)malloc(sizeof(dia_node));

                                                                                            _node->name = strdup($1);
                                                                                            _node->next_parameter = $<node>3;

                                                                                            DIA_DEBUG("=== Function Structured ===\n");
                                                                                            DIA_DEBUG("Function Name: %s\n", $1);

                                                                                            // node traversal to generate parameters
                                                                                            for (dia_node* __node = _node; __node != NULL; __node = __node->next_parameter)
                                                                                              DIA_DEBUG("- Parameter: %s\n", __node->name);


                                                                                            $<node>$ = _node;
                                                                                          }
            | DIA_IDENTIFIER                          {
                                                        DIA_DEBUG("Function Name: %s\n", $1);

                                                        dia_node* _node = (dia_node*)malloc(sizeof(dia_node));
                                                        _node->name = strdup($1);
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
              | dia_function DIA_COMMA dia_parameters
                {
                  dia_node* _node = (dia_node*)malloc(sizeof(dia_node));
                  _node->next_parameter = $<node>3;

                  DIA_DEBUG("+- Function Parameter: %s,%s\n", $<str>1, $<str>3);
                  $<node>$ = _node;
                }
              ;

token: DIA_STRING         { $<node>$ = dia_string($1); }
     | DIA_INTEGER        { $<node>$ = dia_integer($1); }
     | DIA_DOUBLE         { $<node>$ = dia_double($1); }
     ;


%%

/*
char* dia_generate_code(char* dia_identifier, int number_of_args, YYSTYPE* args) {
  va_list args;
  char* _generated_code = (char*)malloc(120*sizeof(char)); // Less than 120 characters per line is advisable, isn't it?

  typedef struct {
    char* identifier;
    (void)(*handler)();
  } dia_predefined_function;

  dia_predefined_function functions[] = {
    {"puts", dia_puts},
    {"print", dia_print},
  }

  for(int i=0; i<sizeof(args)/sizeof(YYSTYPE)
}
*/

void yyerror(const char *str) {
  puts("diac: There was an error while parsing the code");
  printf("[DIA:Error] %s\n", str);

  return;
}

