#include "dia_calculation.h"

extern uint8_t DIA_VERBOSE_LEVEL;
extern void yyerror(char* err);
extern int VARIABLE_INDEX;
extern FILE* yyout;

dia_node* dia_vector(dia_node* node) {
  fprintf(yyout, "std::vector<%s> v%d = ",
      dia_token_type_to_string(node->parameters[0]->type), VARIABLE_INDEX);
  fprintf(yyout, "{%s", node->parameters[0]->name);
  for (int i=1; i<node->num_of_params; i++) {
    fprintf(yyout, ",%s", node->parameters[i]->name);
  }
  fprintf(yyout, "};\n");

  return _dia_create_cpp_variable(DIA_VECTOR);
}

dia_node* dia_vector_at(dia_node* node) {
  if (node->parameters[0]->type == DIA_VECTOR &&
      node->parameters[1]->type == DIA_INTEGER) {
    DIA_DEBUG("vector_at: Parameter types are different than I expected: "
        "vector and int, but %d and %d given.\n",
        node->parameters[0]->type, node->parameters[1]->type);
  }

  dia_node* vec = node->parameters[0];
  if (vec->parameters)
    vec = dia_generate_code(vec);
  dia_node* index = node->parameters[1];
  if (index->parameters)
    index = dia_generate_code(index);

  if (index->name[0] < '0' || index->name[0] > '9' ) {
    int int_index = atoi(index->name);
    if (int_index < 0 || int_index >= vec->num_of_params) {
      DIA_DEBUG("vector_at: Index out of range: %d, %d\n",
          vec->num_of_params, int_index);
    }
  }

  fprintf(yyout, "auto v%d = %s[%s];\n", VARIABLE_INDEX, vec->name, index->name);
  return _dia_create_cpp_variable(vec->parameters[0]->type);
}
