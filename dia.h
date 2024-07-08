#include <stdio.h>
#include <stdint.h>
#include <string.h>

#undef DIA_DEBUG
#define DIA_DEBUG(...) (DIA_VERBOSE_LEVEL < 1 ? : fprintf(stderr, "[DIA:dia.c] " __VA_ARGS__))

// Pre-defined functions
char* dia_string(char* arg);
int dia_integer(int arg);
double dia_double(double arg);

void dia_main();
