%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if __has_include ("dia.tab.h")
#include "dia.tab.h"
#else
#define DIA_LEXER_MODE
#endif

#undef DIA_DEBUG
#define DIA_DEBUG(...) (DIA_VERBOSE_LEVEL < 1 ? : fprintf(stderr, "[DIA:dia.l] " __VA_ARGS__))

extern uint8_t DIA_VERBOSE_LEVEL;

void yyerror(const char* str);
%}

%x C_COMMENT

%%

#.+                         { DIA_DEBUG("Comment starts with sharp(#)!\n"); }
\/\/.+                      { DIA_DEBUG("Comment starts with slashes(//)!\n"); }

"/*"                        { BEGIN(C_COMMENT); }
<C_COMMENT>"*/"             { BEGIN(INITIAL); DIA_DEBUG("Multi-line comment! (/* ... */)\n"); }
<C_COMMENT>\n               { }
<C_COMMENT>.                { }

0x[0-9a-fA-F]+              { DIA_DEBUG("DIA_HEXADECIMAL: %s\n", yytext);

                              dia_node* node = (dia_node*) malloc(sizeof(dia_node));
                              node->name = strndup(yytext, yyleng);
                              node->length_of_name = yyleng;
                              node->type = DIA_INTEGER;

                              yylval.node = node;
                              return DIA_INTEGER; }

[0-9]+\.[0-9]+              { DIA_DEBUG("DIA_DOUBLE: %s\n", yytext);

                              dia_node* node = (dia_node*) malloc(sizeof(dia_node));
                              node->name = strndup(yytext, yyleng);
                              node->length_of_name = yyleng;
                              node->type = DIA_DOUBLE;

                              yylval.node = node;
                              return DIA_DOUBLE; }

[1-9][0-9]*|0               { DIA_DEBUG("DIA_INTEGER: %s\n", yytext);

                              dia_node* node = (dia_node*) malloc(sizeof(dia_node));
                              node->name = strndup(yytext, yyleng);
                              node->length_of_name = yyleng;
                              node->type = DIA_INTEGER;

                              yylval.node = node;

                              return DIA_INTEGER; }

\"([^\\\"]|\\.)*\"          { DIA_DEBUG("DIA_STRING: %s\n", yytext);
                              dia_node* node = (dia_node*) malloc(sizeof(dia_node));
                              node->name = strndup(yytext, yyleng);
                              node->length_of_name = yyleng;
                              node->type = DIA_STRING;

                              yylval.node = node;

                              return DIA_STRING; }

(T|t)rue                    { DIA_DEBUG("DIA_BOOL: %s\n", yytext);
                              dia_node* node = (dia_node*)malloc(sizeof(dia_node));
                              node->name = strdup("true");
                              node->type = DIA_BOOL;

                              yylval.node = node;

                              return DIA_BOOL; }

(F|f)alse                   { DIA_DEBUG("DIA_BOOL: %s\n", yytext);
                              dia_node* node = (dia_node*)malloc(sizeof(dia_node));
                              node->name = strdup("false");
                              node->type = DIA_BOOL;

                              yylval.node = node;

                              return DIA_BOOL; }


  /* Calculation Token */
  /* dia_arithmetic */
"+"                         { DIA_DEBUG("DIA_PLUS\n"); return DIA_PLUS; }
"-"                         { DIA_DEBUG("DIA_MINUS\n"); return DIA_MINUS; }
"*"                         { DIA_DEBUG("DIA_MUL\n"); return DIA_MUL; }
"/"                         { DIA_DEBUG("DIA_DIV\n"); return DIA_DIV; }
"%"                         { DIA_DEBUG("DIA_MOD\n"); return DIA_MOD; }

  /* dia_logical */
"&&"                        { DIA_DEBUG("DIA_LOGICAL_AND\n"); return DIA_LOGICAL_AND; }
"and"                       { DIA_DEBUG("DIA_LOGICAL_AND\n"); return DIA_LOGICAL_AND; }
"||"                        { DIA_DEBUG("DIA_LOGICAL_OR\n"); return DIA_LOGICAL_OR; }
"or"                        { DIA_DEBUG("DIA_LOGICAL_OR\n"); return DIA_LOGICAL_OR; }
"!"                         { DIA_DEBUG("DIA_LOGICAL_NOT\n"); return DIA_LOGICAL_NOT; }
"not"                       { DIA_DEBUG("DIA_LOGICAL_NOT\n"); return DIA_LOGICAL_NOT; }

  /* dia_comparison */
"=="                        { DIA_DEBUG("DIA_EQUAL"); return DIA_EQUAL; }
">="                        { DIA_DEBUG("DIA_GREATER_EQUAL"); return DIA_GREATER_EQUAL; }
">"                         { DIA_DEBUG("DIA_GREATER"); return DIA_GREATER; }
"<="                        { DIA_DEBUG("DIA_LESS_EQUAL"); return DIA_LESS_EQUAL; }
"<"                         { DIA_DEBUG("DIA_LESS"); return DIA_LESS; }

  /* dia_bitwise */
"&"                         { DIA_DEBUG("DIA_BIT_AND"); return DIA_BIT_AND; }
"^"                         { DIA_DEBUG("DIA_BIT_XOR"); return DIA_BIT_XOR; }
"xor"                       { DIA_DEBUG("DIA_BIT_XOR"); return DIA_BIT_XOR; }
"|"                         { DIA_DEBUG("DIA_BIT_OR"); return DIA_BIT_OR; }
"~"                         { DIA_DEBUG("DIA_BIT_NOT"); return DIA_BIT_NOT; }

  /* Calculation Token: End */

","                         { DIA_DEBUG("DIA_COMMA\n"); return DIA_COMMA; }
"."                         { DIA_DEBUG("DIA_BIND\n"); return DIA_BIND; }
";"                         { DIA_DEBUG("DIA_NEXT\n"); return DIA_NEXT; }


"("                         { DIA_DEBUG("DIA_OPEN_PARENTHESIS\n"); return DIA_OPEN_PARENTHESIS; }
")"                         { DIA_DEBUG("DIA_CLOSE_PARENTHESIS\n"); return DIA_CLOSE_PARENTHESIS; }
"["                         { DIA_DEBUG("DIA_OPEN_BRACKET\n"); return DIA_OPEN_BRACKET; }
"]"                         { DIA_DEBUG("DIA_CLOSE_BRACKET\n"); return DIA_CLOSE_BRACKET; }


"main"                      { DIA_DEBUG("DIA_MAIN_FUNC\n"); return DIA_MAIN_FUNC; }
"if"                        { DIA_DEBUG("DIA_IF\n"); return DIA_IF; }
"else"                      { DIA_DEBUG("DIA_ELSE\n"); return DIA_ELSE; }
[a-zA-Z_][a-zA-Z0-9_-]+     { DIA_DEBUG("DIA_IDENTIFIER: %s\n", yytext);
                              yylval.str = strndup(yytext, strlen(yytext));
                              return DIA_IDENTIFIER; }

"="                         { DIA_DEBUG("DIA_ALLOC\n"); return DIA_ALLOC; }

.                           { DIA_DEBUG("Error: Unknown Token - %s\n", yytext); }


%%

#ifdef DIA_LEXER_MODE
int main() {
  yyparse();

  return 0;
}
#endif

