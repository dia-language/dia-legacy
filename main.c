#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "dia.tab.h"

extern FILE* yyin;

uint8_t DIA_VERBOSE_LEVEL;

void dia_help() {
  puts("Dia: The brilliance of Diamond, the elegance of the Language\n");
  puts("Usage: diac [options] file.dia\n");
  puts("Option:");
  puts("  -v[v..]   Increase the level of verbosity (-v to -vvvv)\n\n");
}

void dia_verbosity(char* argument) {
  DIA_VERBOSE_LEVEL = 1;

  printf("Dia: Set verbosity level %d\n", DIA_VERBOSE_LEVEL);

  for(int i=2; argument[i] == 'v'; i++)
    ++DIA_VERBOSE_LEVEL;
}

typedef struct {
  char* option;
  void (*handler)(char* option);
} dia_option_table;

int main(int argc, char** argv) {
  printf("argc: %d\n", argc);

  printf("argv: [");
  for(int i=0; i<argc; i++) {
    printf(" %s ", argv[i]);
  }
  puts("]");

  // diac --help
  if (argc == 2 && !strncmp(argv[1], "--help", 6))
    dia_help();

  // diac example/hello.dia
  // There should be '.dia' at the end of the argv[1].
  else if (strstr(argv[1], ".dia") - (strlen(argv[1]) - 4) == argv[1] )
    yyin = fopen(argv[1], "r");

    // To deal with options without getopt().
    dia_option_table table[] = {
      {"-v", dia_verbosity},
    };

  for (int i=1; argv[i] != NULL; i++) {
    for (int j=0; j<sizeof(table)/sizeof(table[0]); j++) {
      if (strstr(table[j].option, argv[i]))
        table[j].handler(argv[i]);
    }
  }

  yyparse();

  return 0;
}
