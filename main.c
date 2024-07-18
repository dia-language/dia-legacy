#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "dia.tab.h"

#undef DIA_DEBUG
#define DIA_DEBUG(...) (DIA_VERBOSE_LEVEL < 1 ? : fprintf(stderr, "[DIA:main.c] " __VA_ARGS__))
#undef DIA_DEBUG_2
#define DIA_DEBUG_2(...) (DIA_VERBOSE_LEVEL < 2 ? : fprintf(stderr, "[DIA:main.c] " __VA_ARGS__))

extern FILE* yyin;
extern FILE* yyout;

char DIA_VERSION[] = "dia-2024.07.15.alpha";
char* DIA_CODE_FILE_NAME;
uint8_t DIA_VERBOSE_LEVEL;

void dia_help() {
  puts("Dia: The brilliance of Diamond, the elegance of the Language\n");
  puts("Usage: diac [options] file.dia\n");
  puts("Option:");
  puts("  -v[v..]   Increase the level of verbosity (-v to -vvvv)\n\n");
}

void dia_verbosity(char* argument) {
  DIA_VERBOSE_LEVEL = 1;

  for(int i=2; argument[i] == 'v' && i <= 4; i++)
    ++DIA_VERBOSE_LEVEL;
  fprintf(stderr, "Dia: Set verbosity level %d\n", DIA_VERBOSE_LEVEL);
}

typedef struct {
  char* option;
  void (*handler)(char* option);
} dia_option_table;

int main(int argc, char** argv) {
  fprintf(stderr, "argc: %d\n", argc);

  fprintf(stderr, "argv: [");
  for(int i=0; i<argc; i++) {
    fprintf(stderr, " %s ", argv[i]);
  }
  fputs("]\n", stderr);

  if (argc == 1) {
    puts("Dia: Switch to the interactive mode.");
    puts("This is an example of dia programming language\n");
    puts("main = \"Hello World!\".puts\n");
  }

  // diac --help
  else if (argc == 2 && !strncmp(argv[1], "--help", 6)) {
    dia_help();
    return 0;
  }

  // diac example/hello.dia
  // There should be '.dia' at the end of the argv[1].
  else if (strstr(argv[1], ".dia") - (strlen(argv[1]) - 4) == argv[1] ) {
    yyin = fopen(argv[1], "r");
    DIA_CODE_FILE_NAME = argv[1];
  }


  // To deal with options without getopt().
  dia_option_table table[] = {
    {"-v", dia_verbosity},
  };

  for (int i=1; argv[i] != NULL; i++) {
    for (int j=0; j<sizeof(table)/sizeof(table[0]); j++) {
      if (strstr(argv[i], table[j].option))
        table[j].handler(argv[i]);
    }
  }

  yyparse();

  return 0;
}
