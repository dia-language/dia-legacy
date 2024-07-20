#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "dia.tab.h"

#undef DIA_DEBUG
#define DIA_DEBUG(...) (DIA_VERBOSE_LEVEL < 1 ? : fprintf(stderr, "[DIA:dia.c] " __VA_ARGS__))
#undef DIA_DEBUG_2
#define DIA_DEBUG_2(...) (DIA_VERBOSE_LEVEL < 2 ? : fprintf(stderr, "[DIA:dia.c] " __VA_ARGS__))

#ifndef _DIA_NODE_STRUCT
#define _DIA_NODE_STRUCT
typedef int DIA_TOKEN_TYPE;
typedef struct _dia_node {
  char* name;
  char* generated_code;
  struct _dia_node* next_parameter;     /* This will be used to store the add-
                                         * ress of the function parameters
                                         * in a single linked list.
                                         */
  struct _dia_node* next_function;      /* This will be held the next chain of
                                         * function, that will be used to chain
                                         * the next function.
                                         */
  DIA_TOKEN_TYPE type;
} dia_node;
#endif

#ifndef _DIA_H
#define _DIA_H

dia_node* dia_string(dia_node* arg);
dia_node* dia_integer(dia_node* arg);
dia_node* dia_double(dia_node* arg);

// Utility functions
void dia_debug_function_descriptor(dia_node* node);

// Pre-defined functions
void dia_print(dia_node* node);
void dia_puts(dia_node* node);

void dia_generate_code(dia_node* node);

void dia_main(dia_node* node);

#endif
