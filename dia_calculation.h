#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "dia.tab.h"
#include "dia_node.h"

#undef DIA_DEBUG
#define DIA_DEBUG(...) (DIA_VERBOSE_LEVEL < 1 ? : fprintf(stderr, "[DIA:dia_calculation.c] " __VA_ARGS__))
#undef DIA_DEBUG_2
#define DIA_DEBUG_2(...) (DIA_VERBOSE_LEVEL < 2 ? : fprintf(stderr, "[DIA:dia_calculation.c] " __VA_ARGS__))

#ifndef DIA_FUNC_ENTER
#define DIA_FUNC_ENTER(x) (DIA_DEBUG(x ": Entering " x " function\n"))
#endif

#ifndef _DIA_CALC_H
#define _DIA_CALC_H

/* Calculation */
dia_node* dia_plus(dia_node* node);
dia_node* dia_minus(dia_node* node);
dia_node* dia_mul(dia_node* node);
dia_node* dia_div(dia_node* node);
dia_node* dia_mod(dia_node* node);
/* Logic */
dia_node* dia_logical_and(dia_node* node);
dia_node* dia_logical_or(dia_node* node);
dia_node* dia_logical_not(dia_node* node);
/* Comparison */
dia_node* dia_equal(dia_node* node);
dia_node* dia_not_equal(dia_node* node);
dia_node* dia_greater_equal(dia_node* node);
dia_node* dia_greater(dia_node* node);
dia_node* dia_less_equal(dia_node* node);
dia_node* dia_less(dia_node* node);
/* Bitwise operation */
dia_node* dia_bit_and(dia_node* node);
dia_node* dia_bit_or(dia_node* node);
dia_node* dia_bit_xor(dia_node* node);
dia_node* dia_bit_not(dia_node* node);

dia_node* _dia_create_cpp_variable(DIA_TOKEN_TYPE type);
#endif
