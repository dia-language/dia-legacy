#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "dia.tab.h"

#undef DIA_DEBUG
#define DIA_DEBUG(...) (DIA_VERBOSE_LEVEL < 1 ? : fprintf(stderr, "[DIA:dia_calculation.c] " __VA_ARGS__))
#undef DIA_DEBUG_2
#define DIA_DEBUG_2(...) (DIA_VERBOSE_LEVEL < 2 ? : fprintf(stderr, "[DIA:dia_calculation.c] " __VA_ARGS__))


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

#ifndef _DIA_CALC_H
#define _DIA_CALC_H

/* Calculation */
dia_node* dia_plus(dia_node* a, dia_node* b);
dia_node* dia_minus(dia_node* a, dia_node* b);
dia_node* dia_mul(dia_node* a, dia_node* b);
dia_node* dia_div(dia_node* a, dia_node* b);
dia_node* dia_mod(dia_node* a, dia_node* b);

dia_node* dia_logical_and(dia_node* a, dia_node* b);
dia_node* dia_logical_or(dia_node* a, dia_node* b);
dia_node* dia_logical_not(dia_node* a);

dia_node* dia_equal(dia_node* a, dia_node* b);
dia_node* dia_greater_equal(dia_node* a, dia_node* b);
dia_node* dia_greater(dia_node* a, dia_node* b);
dia_node* dia_less_equal(dia_node* a, dia_node* b);
dia_node* dia_less(dia_node* a, dia_node* b);

dia_node* dia_bit_and(dia_node* a, dia_node* b);
dia_node* dia_bit_or(dia_node* a, dia_node* b);
dia_node* dia_bit_xor(dia_node* a, dia_node* b);
dia_node* dia_bit_not(dia_node* a);
#endif
