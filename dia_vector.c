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

  return _dia_create_cpp_variable(DIA_BOOL);
}
