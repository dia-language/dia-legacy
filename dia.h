#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "dia.tab.h"

#undef DIA_DEBUG
#define DIA_DEBUG(...) (DIA_VERBOSE_LEVEL < 1 ? : fprintf(stderr, "[DIA:dia.c] " __VA_ARGS__))
#define DIA_DEBUG_2(...) (DIA_VERBOSE_LEVEL < 2 ? : fprintf(stderr, "[DIA:dia.c] " __VA_ARGS__))

#ifndef _DIA_H
#define _DIA_H

typedef int DIA_TOKEN_TYPE;
typedef struct _dia_node {
  char* name;
  char* generated_code;
  struct _dia_node* next_parameter;
  DIA_TOKEN_TYPE type;
} dia_node;

dia_node* dia_string(dia_node* arg);
dia_node* dia_integer(dia_node* arg);
dia_node* dia_double(dia_node* arg);

// Pre-defined functions
void dia_print();
void dia_puts();


void dia_main(dia_node* node);

#endif
