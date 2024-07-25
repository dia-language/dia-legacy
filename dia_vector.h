#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "dia.tab.h"
#include "dia_node.h"

#undef DIA_DEBUG
#define DIA_DEBUG(...) (DIA_VERBOSE_LEVEL < 1 ? : fprintf(stderr, "[DIA:dia_vector.c] " __VA_ARGS__))
#undef DIA_DEBUG_2
#define DIA_DEBUG_2(...) (DIA_VERBOSE_LEVEL < 2 ? : fprintf(stderr, "[DIA:dia_vector.c] " __VA_ARGS__))

#ifndef DIA_FUNC_ENTER
#define DIA_FUNC_ENTER(x) (DIA_DEBUG(x ": Entering " x " function\n"))
#endif

#ifndef _DIA_VECTOR_H
#define _DIA_VECTOR_H

dia_node* dia_vector(dia_node* node);

#endif
