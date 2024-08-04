#include "dia_calculation.h"

extern uint8_t DIA_VERBOSE_LEVEL;
extern void yyerror();
extern int VARIABLE_INDEX;
extern FILE* yyout;

/* Arithmetic */
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

  if (a->type == DIA_INTEGER && b->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = %s-%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_INTEGER);
  }
  else if ((a->type == DIA_INTEGER || a->type == DIA_DOUBLE)
      && (b->type == DIA_INTEGER || b->type == DIA_DOUBLE)) {
    fprintf(yyout, "auto v%d = %s-%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_DOUBLE);
  }
  else {
    DIA_DEBUG("dia_minus: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_minus: Unsupported parameter type");
  }
}

dia_node* dia_mul(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if (a->type == DIA_INTEGER && b->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = %s*%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_INTEGER);
  }
  else if ((a->type == DIA_INTEGER || a->type == DIA_DOUBLE)
      && (b->type == DIA_INTEGER || b->type == DIA_DOUBLE)) {
    fprintf(yyout, "auto v%d = %s*%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_DOUBLE);
  }
  else if (a->type == DIA_STRING && b->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = std::string()", VARIABLE_INDEX);
    for (int i=0; i<atoi(b->name); i++)
      fprintf(yyout, "+std::string(%s)", b->name);
    fprintf(yyout, ";\n");
    return _dia_create_cpp_variable(DIA_STRING);
  }
  else {
    DIA_DEBUG("dia_mul: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_mul: Unsupported parameter type");
  }
}

dia_node* dia_div(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if (atoi(a->name) == 0) {
    DIA_DEBUG("dia_div: Division by zero\n");
    yyerror("dia_div: Division by zero");
  }

  if (a->type == DIA_INTEGER && b->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = %s/%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_INTEGER);
  }
  else if ((a->type == DIA_INTEGER || a->type == DIA_DOUBLE)
      && (b->type == DIA_INTEGER || b->type == DIA_DOUBLE)) {
    fprintf(yyout, "auto v%d = (double)%s/%s;\n",
        VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_DOUBLE);
  }
  else {
    DIA_DEBUG("dia_mul: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_mul: Unsupported parameter type");
  }
}

dia_node* dia_mod(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if (atoi(b->name) == 0) {
    DIA_DEBUG("dia_div: Division by zero\n");
    yyerror("dia_div: Division by zero");
  }

  if (a->type == DIA_INTEGER && b->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = %s%%%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_INTEGER);
  }
  else {
    DIA_DEBUG("dia_mod: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_mod: Unsupported parameter type");
  }
}

/* Logical */
dia_node* dia_logical_and(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if (a->type == DIA_BOOL && b->type == DIA_BOOL) {
    fprintf(yyout, "auto v%d = %s&&%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_logical_and: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_logical_and: Unsupported parameter type");
  }
}

dia_node* dia_logical_or(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if (a->type == DIA_BOOL && b->type == DIA_BOOL) {
    fprintf(yyout, "auto v%d = %s||%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_logical_or: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_logical_or: Unsupported parameter type");
  }
}

dia_node* dia_logical_not(dia_node* node) {
  dia_node* a = node->parameters[0];

  if (a->type == DIA_BOOL) {
    fprintf(yyout, "auto v%d = !%s;\n", VARIABLE_INDEX, a->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_logical_not: Unsupported type parameter: %d\n", a->type);
    yyerror("dia_logical_not: Unsupported parameter type");
  }
}

/* Comparison */
dia_node* dia_equal(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if ((a->type == DIA_BOOL || a->type == DIA_INTEGER || a->type == DIA_DOUBLE) &&
      (b->type == DIA_BOOL || b->type == DIA_INTEGER || b->type == DIA_DOUBLE)) {
    fprintf(yyout, "auto v%d = %s==%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_equal: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_equal: Unsupported parameter type");
  }
}

dia_node* dia_not_equal(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if ((a->type == DIA_BOOL || a->type == DIA_INTEGER || a->type == DIA_DOUBLE) &&
      (b->type == DIA_BOOL || b->type == DIA_INTEGER || b->type == DIA_DOUBLE)) {
    fprintf(yyout, "auto v%d = %s!=%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_not_equal: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_not_equal: Unsupported parameter type");
  }
}

dia_node* dia_greater_equal(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if ((a->type == DIA_INTEGER || a->type == DIA_DOUBLE) &&
      (b->type == DIA_INTEGER || b->type == DIA_DOUBLE)) {
    fprintf(yyout, "auto v%d = %s>=%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_greater_equal: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_greater_equal: Unsupported parameter type");
  }
}

dia_node* dia_greater(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if ((a->type == DIA_INTEGER || a->type == DIA_DOUBLE) &&
      (b->type == DIA_INTEGER || b->type == DIA_DOUBLE)) {
    fprintf(yyout, "auto v%d = %s&>%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_greater: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_greater: Unsupported parameter type");
  }
}

dia_node* dia_less_equal(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if ((a->type == DIA_INTEGER || a->type == DIA_DOUBLE) &&
      (b->type == DIA_INTEGER || b->type == DIA_DOUBLE)) {
    fprintf(yyout, "auto v%d = %s<=%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_less_equal: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_less_equal: Unsupported parameter type");
  }
}

dia_node* dia_less(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if ((a->type == DIA_INTEGER || a->type == DIA_DOUBLE) &&
      (b->type == DIA_INTEGER || b->type == DIA_DOUBLE)) {
    fprintf(yyout, "auto v%d = %s<%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_less: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_less: Unsupported parameter type");
  }
}

/* Bitwise operation */
dia_node* dia_bit_and(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if (a->type == DIA_INTEGER && b->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = %s&%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_bit_and: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_bit_and: Unsupported parameter type");
  }
}

dia_node* dia_bit_or(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if (a->type == DIA_INTEGER && b->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = %s|%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_bit_or: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_bit_or: Unsupported parameter type");
  }
}

dia_node* dia_bit_xor(dia_node* node) {
  dia_node* a = node->parameters[0];
  dia_node* b = node->parameters[1];

  if (a->type == DIA_INTEGER && b->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = %s^%s;\n", VARIABLE_INDEX, a->name, b->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_bit_xor: Unsupported type parameter: %d, %d\n",
        a->type, b->type);
    yyerror("dia_bit_xor: Unsupported parameter type");
  }
}

dia_node* dia_bit_not(dia_node* node) {
  if (node->type == DIA_INTEGER) {
    fprintf(yyout, "auto v%d = ~%s;\n", VARIABLE_INDEX, node->name);
    return _dia_create_cpp_variable(DIA_BOOL);
  }
  else {
    DIA_DEBUG("dia_bit_not: Unsupported type parameter: %d\n", node->type);
    yyerror("dia_bit_not: Unsupported parameter type");
  }
}


dia_node* _dia_create_cpp_variable(DIA_TOKEN_TYPE type) {
  char _name[8];
  snprintf(_name, 7, "v%d", VARIABLE_INDEX);
  dia_node* _node = dia_create_node(_name, type);

  VARIABLE_INDEX++;
  return _node;
}
