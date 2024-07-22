#include "dia_calculation.h"

extern uint8_t DIA_VERBOSE_LEVEL;
extern void yyerror();
extern int VARIABLE_INDEX;
extern FILE* yyout;

dia_node* dia_plus(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  // All parameters should have the same types
  if (a->type != b->type) {
    DIA_DEBUG("dia_plus: Type of parameter mismatch: %d, %d\n",
        a->type, b->type);
    yyerror("dia_plus: Type of parameter mismatch.");
  }

  if (a->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = %s+%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_INTEGER);
  }
  else if (a->type == DIA_DOUBLE) {
    fprintf(yyout, "auto v%d = %s+%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_DOUBLE);
  }
  else if (a->type == DIA_STRING) {
    // "abcde" ++ "fghijk"  =>  "abcdefghijk"
    // ^~~~~~7    ^~~~~~~8      ^~~~~~~~~~~~13
    fprintf(yyout, "auto v%d = ", VARIABLE_INDEX);
    if (a->name[0] == '"')
      fprintf(yyout, "std::string(%s)", a->name);
    else
      fprintf(yyout, "%s", a->name);

    fprintf(yyout, "+");

    if (b->name[0] == '"')
      fprintf(yyout, "std::string(%s);\n", b->name);
    else
      fprintf(yyout, "%s;\n", b->name);

    return _dia_create_cpp_variable(DIA_STRING);
  }
  else {
    DIA_DEBUG("dia_plus: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_plus: Unsupported parameter type");
  }
}

dia_node* dia_minus(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if (a->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = %s-%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_INTEGER);
  }
  else if (a->type == DIA_DOUBLE) {
    fprintf(yyout, "auto v%d = %s-%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_DOUBLE);
  }
  else {
    DIA_DEBUG("dia_minus: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_minus: Unsupported parameter type");
  }
}

dia_node* dia_mul(dia_node* a) {
  return NULL;
}

dia_node* dia_div(dia_node* a) {
  return NULL;
}

dia_node* dia_mod(dia_node* a) {
  return NULL;
}


dia_node* dia_logical_and(dia_node* a, dia_node* b) {
  return NULL;
}
dia_node* dia_logical_or(dia_node* a, dia_node* b) {
  return NULL;
}
dia_node* dia_logical_not(dia_node* a) {
  return NULL;
}

dia_node* dia_equal(dia_node* a, dia_node* b) {
  return NULL;
}
dia_node* dia_greater_equal(dia_node* a, dia_node* b) {
  return NULL;
}
dia_node* dia_greater(dia_node* a, dia_node* b) {
  return NULL;
}
dia_node* dia_less_equal(dia_node* a, dia_node* b) {
  return NULL;
}
dia_node* dia_less(dia_node* a, dia_node* b) {
  return NULL;
}

dia_node* dia_bit_and(dia_node* a, dia_node* b) {
  return NULL;
}
dia_node* dia_bit_or(dia_node* a, dia_node* b) {
  return NULL;
}
dia_node* dia_bit_xor(dia_node* a, dia_node* b) {
  return NULL;
}
dia_node* dia_bit_not(dia_node* a) {
  return NULL;
}

dia_node* _dia_create_cpp_variable(DIA_TOKEN_TYPE type) {
  char _name[8];
  snprintf(_name, 7, "v%d", VARIABLE_INDEX);
  dia_node* _node = dia_create_node(_name, type);

  VARIABLE_INDEX++;
  return _node;
}
