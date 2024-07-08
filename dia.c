#include "dia.h"

extern uint8_t DIA_VERBOSE_LEVEL;
extern char DIA_VERSION[];
extern char* DIA_CODE_FILE_NAME;
extern FILE* yyout;

char* dia_string(char* arg) {
  DIA_DEBUG("Dia String: %s\n", arg);
  return arg;
}

int dia_integer(int arg) {
  DIA_DEBUG("Dia Integer: %d\n", arg);
  return arg;
}

double dia_double(double arg) {
  DIA_DEBUG("Dia Double: %lf\n", arg);
  return arg;
}


// The main function

void _dia_comment_generating() {
  DIA_DEBUG("Generating an explanation comment in the main function...\n");

  fputs("/*\n", yyout);
  fputs(" * This code is generated by diac, the Dia programming language compiler.\n", yyout);
  fputs(" * Version Info: ", yyout);
  fputs(DIA_VERSION, yyout);
  fputs("\n", yyout);
  fputs(" *\n", yyout);
  fputs(" * Original Source Code:\n", yyout);
  fputs(" *\n", yyout);
  fputs(" * ```dia\n", yyout);
  fputs(" * ", yyout);

  FILE* source_file = fopen(DIA_CODE_FILE_NAME, "r");
  while(1) {
    char c = fgetc(source_file);
    if (c == EOF) {
      fputs("\n", yyout);
      break;
    }
    else if (c == '\n')
      fputs("\n * ", yyout);
    else
      fputc(c, yyout);
  }
  fclose(source_file);

  fputs(" * ```\n", yyout);
  fputs(" */\n", yyout);
}

void dia_main() {
  _dia_comment_generating();

  DIA_DEBUG("Generating the main function code...\n");
}
