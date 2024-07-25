#include "dia.h"

extern uint8_t DIA_VERBOSE_LEVEL;
extern char DIA_VERSION[];
extern char* DIA_CODE_FILE_NAME;
extern FILE* yyout;
extern void yyerror();

int VARIABLE_INDEX;

dia_node* dia_string(dia_node* arg) {
  DIA_DEBUG("Dia String: %s\n", arg->name);
  return arg;
}

dia_node* dia_integer(dia_node* arg) {
  DIA_DEBUG("Dia Integer: %s\n", arg->name);
  return arg;
}

dia_node* dia_double(dia_node* arg) {
  DIA_DEBUG("Dia Double: %s\n", arg->name);
  return arg;
}

dia_node* dia_bool(dia_node* arg) {
  DIA_DEBUG("Dia Bool: %s\n", arg->name);
  return arg;
}

// Utility
char* dia_token_type_to_string(DIA_TOKEN_TYPE type) {
  switch(type) {
    case DIA_INTEGER:
      return "int";
    case DIA_DOUBLE:
      return "double";
    case DIA_STRING:
      return "std::string";
    case DIA_BOOL:
      return "bool";
    default:
      DIA_DEBUG("dia_token_type_to_string: Unknown Type: %d\n", type);
      yyerror("Unknown Type.");
  }
}

void dia_free_node (dia_node* node) {
  DIA_DEBUG("This node will be freed: %s\n", node->name);

  // This might take time, because the structure of dia_node is
  // a single linked list. I should benchmark and calculate which
  // approach would be appropriate.
  DIA_DEBUG("- next_parameter->name: %s\n",
      node->next_parameter == NULL? "NULL" : node->next_parameter->name);
  if (node->next_parameter != NULL)  dia_free_node(node->next_parameter);
  DIA_DEBUG("- next_function->name: %s\n",
      node->next_function == NULL? "NULL" : node->next_function->name);
  if (node->next_function != NULL)   dia_free_node(node->next_function);

  DIA_DEBUG("This parameter will be freed:\n");
  for (int i=0; i<node->num_of_params; i++) {
    if (node->parameters[i] != NULL) {
      DIA_DEBUG("- parameter[%d]->name: %s\n", i, node->parameters[i]->name);
      dia_free_node(node->parameters[i]);
    }
    else
      DIA_DEBUG("- parameter[%d]->name: NULL\n", i);
  }
  DIA_DEBUG("Parameter Freed\n");
  if (node->name != NULL) {
    memset(node->name, 0, strlen(node->name));
    free(node->name);
  }

  memset(node, 0, sizeof(dia_node));
  free(node);
}

dia_node* dia_bind(dia_node* prev, dia_node* next) {
  dia_node** params = (dia_node**)malloc(sizeof(dia_node*)*next->num_of_params + 1);
  params[0] = prev;

  for(int i=1; i<=next->num_of_params; i++)
    params[i] = next->parameters[i-1];

  if (next->parameters != NULL) {
    memset(next->parameters, 0, sizeof(dia_node*)*next->num_of_params);
    free(next->parameters);
  }
  next->parameters = params;
  ++next->num_of_params;

  DIA_DEBUG("DIA_BIND: To weave function parameter to the adjacent(next) function.\n");
  dia_debug_function_descriptor(next, 0);
  return next;
}

void dia_debug_function_descriptor(dia_node* node, int depth) {
  DIA_DEBUG("=== Function Structure description ===\n");
  DIA_DEBUG("Depth: %d\n", depth);
  DIA_DEBUG("Function Name: %s\n", node->name);

  // node traversal to generate parameters
  for (int i=0; i<node->num_of_params; i++) {
    DIA_DEBUG("- Parameter: %s\n", node->parameters[i]->name);
    if (node->parameters[i]->parameters != NULL)
      dia_debug_function_descriptor(node->parameters[i], ++depth);
  }

  DIA_DEBUG("=== Function Structure description ===\n");
  return;
}


// Predefined functions

dia_node* dia_puts(dia_node* node) {
  DIA_DEBUG("dia_puts\n");
  // We don't need to check type of parameters;
  if (node == NULL)  yyerror();

  fputs("std::cout", yyout);
  for (int i=0; i<node->num_of_params; i++)
    fprintf(yyout, "<<%s", node->parameters[i]->name);
  fputs("<<std::endl;\n", yyout);

  DIA_DEBUG("End of dia_puts\n");
  return NULL;
}

dia_node* dia_print(dia_node* node) {
  DIA_DEBUG("dia_print\n");
  // We don't need to check type of parameters;

  fputs("std::cout", yyout);
  for (node = node->next_parameter; node != NULL; node = node->next_parameter)
    fprintf(yyout, "<<%s", node->name);

  DIA_DEBUG("End of dia_print\n");
  return NULL;
}


// Generate function
dia_node* dia_generate_code(dia_node* node) {
  if (node == NULL) {
    DIA_DEBUG("dia_plus: the node parameter is NULL\n");
    return NULL;
  }

  for (int i=0; i<node->num_of_params; i++) {
    if (node->parameters[i]->name != NULL) {
      if (node->parameters[i]->parameters != NULL)
        node->parameters[i] = dia_generate_code(node->parameters[i]);
    }
  }

  typedef struct {
    char* identifier;
    dia_node* (*handler)(dia_node* node);
    int num_of_params;
  } dia_predefined_function;

  // If num_of_params is -1, that means it does not need to check
  // the number of parameters
  dia_predefined_function functions[] = {
    /* Printing */
    {"puts", dia_puts, -1},
    {"print", dia_print, -1},
    /* Arithmetic */
    {"plus", dia_plus, 2},
    {"minus", dia_minus, 2},
    {"times", dia_mul, 2},
    {"divide", dia_div, 2},
    {"modular", dia_mod, 2},
    /* Logic */
    {"logical_and", dia_logical_and, 2},
    {"logical_or", dia_logical_or, 2},
    {"logical_not", dia_logical_not, 1},
    /* Comparison */
    {"equal", dia_logical_and, 2},
    {"not_equal", dia_logical_and, 2},
    {"greater_equal", dia_logical_and, 2},
    {"greater", dia_logical_and, 2},
    {"less_equal", dia_logical_and, 2},
    {"less", dia_logical_and, 2},
    /* Bitwise operation */
    {"bit_and", dia_bit_and, 2},
    {"bit_or", dia_bit_and, 2},
    {"bit_xor", dia_bit_and, 2},
    {"bit_not", dia_bit_and, 1},
    /* Vector */
    {"vector", dia_vector, -1},
  };

  int i=0;
  int size = sizeof(functions) / sizeof(functions[0]);
  for (; i<size; i++) {
    if (!strcmp(node->name, functions[i].identifier)) {
      if (node->num_of_params != functions[i].num_of_params &&
          functions[i].num_of_params != -1) {
        DIA_DEBUG("%s: Size of parameter should be %d, but it's %d.\n",
            node->name, functions[i].num_of_params, node->num_of_params);
        yyerror("Type of parameter mismatch.");
      }

      DIA_DEBUG("%s: Entering the function %s\n",
          functions[i].identifier, functions[i].identifier);
      node = functions[i].handler(node);
      DIA_DEBUG("%s: Escaping the function %s\n",
          functions[i].identifier, functions[i].identifier);
      break;
    }
  }

  if (i == size) {
    fprintf(stderr, "Undefined method: %s\n", node->name);
    yyerror("Undefined method.");
  }

  return node;
}

// The main function

void _dia_comment_generating() {
  DIA_DEBUG("Generating an explanation comment in the main function...\n");

  fputs("/*\n", yyout);
  fputs(" * This code is generated by diac, the Dia programming language compiler.\n", yyout);
  fputs(" * Version Info: ", yyout);
  fputs(DIA_VERSION, yyout);
  fputs("\n", yyout);
  fputs(" *\n", yyout);
  fputs(" * Original Source Code:\n", yyout);
  fputs(" *\n", yyout);
  fputs(" * ```dia\n", yyout);
  fputs(" * ", yyout);

  FILE* source_file = fopen(DIA_CODE_FILE_NAME, "r");
  while(1) {
    char c = fgetc(source_file);
    DIA_DEBUG_2("read byte %02x from the %s file;\n", c, DIA_CODE_FILE_NAME);
    if (c == EOF || c == (char)0xff) {
      fputs("\n", yyout);
      break;
    }
    else if (c == '\n')
      fputs("\n * ", yyout);
    else
      fputc(c, yyout);
  }
  fclose(source_file);

  fputs(" * ```\n", yyout);
  fputs(" */\n", yyout);
}

void dia_main(dia_node* node) {
  DIA_DEBUG("Welcome to the Dia World! main function detected!\n");
  DIA_DEBUG("Lemme write down bill of main function...\n");
  DIA_DEBUG("=== Main Function Started ===\n");

  for (dia_node* _node = node; _node != NULL; _node = _node->next_function)
    dia_debug_function_descriptor(_node, 0);

  DIA_DEBUG("=== Main Function Concluded ===\n\n");

  // DIA_CODE_FILE_NAME is NULL when the diac runs in interactive mode
  if (DIA_CODE_FILE_NAME != NULL)
    _dia_comment_generating();

  DIA_DEBUG("Generating the main function code...\n");

  fputs("#include<iostream>\n", yyout);
  fputs("#include<string>\n", yyout);
  fputs("#include<vector>\n", yyout);
  fputs("int main(int argc, char** argv) {\n", yyout);

  for (dia_node* _node = node; _node != NULL; _node = _node->next_function)
    dia_generate_code(_node);

  fputs("}", yyout);
  puts("");
  dia_free_node(node);
}
