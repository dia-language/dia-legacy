#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "dia.tab.h"

#undef DIA_DEBUG
#define DIA_DEBUG(...) (DIA_VERBOSE_LEVEL < 1 ? : fprintf(stderr, "[DIA:main.c] " __VA_ARGS__))
#undef DIA_DEBUG_2
#define DIA_DEBUG_2(...) (DIA_VERBOSE_LEVEL < 2 ? : fprintf(stderr, "[DIA:main.c] " __VA_ARGS__))

extern FILE* yyin;
extern FILE* yyout;
extern int errno;

char DIA_VERSION[] = "dia-2024.07.30.alpha";
char* DIA_CODE_FILE_NAME;
uint8_t DIA_VERBOSE_LEVEL;
uint8_t _SKIP_FORMAT;
uint8_t _NO_COMPILE;
uint8_t _PRINT_TO_STDOUT;

void dia_interactive_banner() {
    puts("Dia: Switching to the interactive mode.");
    puts("The interactive mode is intended for debugging purposes.");
    puts("Press Ctrl+D (^D) to exit and generate C++ code.");
    puts("This is an example of dia programming language:\n");
    puts("main = \"Hello World!\".puts\n");
}

void dia_help() {
  puts("Dia: The brilliance of Diamond, the elegance of the Language\n");
  puts("Usage: diac [options] file.dia\n");
  puts("Option:");
  puts("  -v[v..]   Increase the level of verbosity (-v to -vvvv)");
  puts("  --stdout  Print compiled code to terminal\n\n");
}

void dia_verbosity(char* argument) {
  DIA_VERBOSE_LEVEL = 1;

  for(int i=2; argument[i] == 'v' && i <= 4; i++)
    ++DIA_VERBOSE_LEVEL;
  fprintf(stderr, "Dia: Set verbosity level %d\n", DIA_VERBOSE_LEVEL);
}

void _skip_format(char* argument) {
  _SKIP_FORMAT = 1;
}

void _no_compile(char* argument) {
  _NO_COMPILE = 1;
}

void _print_to_stdout(char* argument) {
  _PRINT_TO_STDOUT = 1;
}

typedef struct {
  char* option;
  void (*handler)(char* option);
} dia_option_table;

int main(int argc, char** argv) {
  char* _generated_cpp_file_name = NULL;

  fprintf(stderr, "argc: %d\n", argc);

  fprintf(stderr, "argv: [");
  for(int i=0; i<argc; i++) {
    fprintf(stderr, " %s ", argv[i]);
  }
  fputs("]\n", stderr);

  if (argc == 1) dia_interactive_banner();

  // diac --help
  else if (argc == 2 && !strncmp(argv[1], "--help", 6)) {
    dia_help();
    return 0;
  }

  else if (strstr(argv[1], ".dia") - (strlen(argv[1]) - 4) == argv[1] ) {
    yyin = fopen(argv[1], "r");
    DIA_CODE_FILE_NAME = argv[1];
  }


  // To deal with options without getopt().
  dia_option_table table[] = {
    {"-v", dia_verbosity},
    {"--stdout", _print_to_stdout},
    {"--no-compile", _no_compile},
    {"--skip-format", _skip_format},
  };

  // Setting up appropriate flags
  for (int i=1; i<argc; i++) {
    for (int j=0; j<sizeof(table)/sizeof(table[0]); j++) {
      if (strstr(argv[i], table[j].option))
        table[j].handler(argv[i]);
    }
  }

  for (int i=1; i<argc; i++) {
    // diac example/hello.dia
    // There should be '.dia' at the end of the argv[1].
    if (strstr(argv[i], ".dia") - (strlen(argv[i]) - 4) == argv[i] ) {
      yyin = fopen(argv[i], "r");
      DIA_CODE_FILE_NAME = argv[i];
      if (_PRINT_TO_STDOUT != 1) {
        int i = strlen(DIA_CODE_FILE_NAME);
        while (DIA_CODE_FILE_NAME[i-1] != '/') i--;

        _generated_cpp_file_name = strdup(&DIA_CODE_FILE_NAME[i]);
        strncpy(&_generated_cpp_file_name[strlen(_generated_cpp_file_name)-3], "cpp", 3);
        DIA_DEBUG("main.c: File will be written at %s\n", _generated_cpp_file_name);
        if ((yyout = fopen(_generated_cpp_file_name, "w")) == NULL) {
          printf("diac: Cannot create file %s.\n", _generated_cpp_file_name);
        }
      }
      else
        yyout = stdout;
    }
  }

  yyparse();
  if (yyout != stdout)
    fclose(yyout);

  if (_NO_COMPILE) {
    DIA_DEBUG("main.c: You chose not to compile the generated code. Skipping.\n");
  }
  else {
    DIA_DEBUG("main.c: Finding C++ compiler...\n");
    DIA_DEBUG("clang++ %s\n", _generated_cpp_file_name);

    pid_t pid = fork();
    int return_val = 0;
    if (pid == 0) {
      int _status = execlp("clang++", "clang++", _generated_cpp_file_name, (void*)NULL);

      if (_status == -1 && errno == ENOENT) {
        DIA_DEBUG("clang++ not found (ENOENT). Trying to compile the generated code with g++..\n");
        DIA_DEBUG("g++ %s\n", _generated_cpp_file_name);

        _status = execlp("g++", "g++", _generated_cpp_file_name, (void*)NULL);
        if (_status == -1 && errno == ENOENT) {
          DIA_DEBUG("main.c: We couldn't find any relative C++ compiler.\n");
          return 1;
        }
      }
      return 0;
    }
    else if (pid > 0)
      wait(&return_val);
    else {
      perror("There were error after calling fork().");
      return 1;
    }
  }

  if (_SKIP_FORMAT) {
    DIA_DEBUG("main.c: You chose not to use clang-format tool. Skipping.\n");
  }
  else if (DIA_CODE_FILE_NAME != NULL) {
    DIA_DEBUG("Finding clang-format...\n");
    int return_val = execlp("clang-format", "clang-format", _generated_cpp_file_name, "-i", (void*)NULL);
    int err = errno;
    if(return_val == -1 && err == ENOENT)
      DIA_DEBUG("main.c: clang-format not found. Skipping.\n");
  }

  return 0;
}
