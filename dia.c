#include "dia.h"

extern uint8_t DIA_VERBOSE_LEVEL;
extern char DIA_VERSION[];
extern char* DIA_CODE_FILE_NAME;
extern FILE* yyout;
extern void yyerror();
extern custom_function_t* custom_functions;
extern uint8_t _QUIET;
extern uint8_t _FASTER_IO;
extern uint8_t _MAKE_FUNCTIONS_PURE;
extern uint8_t _MAKE_FUNCTIONS_CONSTEXPR;

dia_node* current_function;

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
    case 0:
      return "void";
    default:
      DIA_DEBUG("dia_token_type_to_string: Unknown Type: %d\n", type);
      yyerror("Unknown type.");
  }
}

DIA_TOKEN_TYPE dia_token_type_from_string(char* strtype) {
  if (!strcmp(strtype, "int"))
    return DIA_INTEGER;
  else if (!strcmp(strtype, "double"))
    return DIA_DOUBLE;
  else if (!strcmp(strtype, "string"))
    return DIA_STRING;
  else if (!strcmp(strtype, "str"))
    return DIA_STRING;
  else if (!strcmp(strtype, "bool"))
    return DIA_BOOL;
  else if (!strcmp(strtype, "void"))
    return 0;
  else {
    DIA_DEBUG("dia_toekn_type_from_string: Unknown type: %s\n", strtype);
    yyerror("Unknown type.");
  }
}

uint8_t is_variable(dia_node* node) {
  char _first = node->name[0];
  DIA_TOKEN_TYPE _type = node->type;

  if ((_type == DIA_INTEGER && (_first >= '0' && _first <= '9')) ||
   (_type == DIA_DOUBLE && (_first >= '0' && _first <= '9')) ||
   (_type == DIA_STRING && _first == '"') ||
   (_type == DIA_BOOL &&
    strcmp(node->name, "true") && strcmp(node->name, "True") &&
    strcmp(node->name, "false") && strcmp(node->name, "False")))
    return 0; // false
  else
    return 1; // true
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
  DIA_DEBUG("=== Function Structure description Started ===\n");
  DIA_DEBUG("Depth: %d\n", depth);
  DIA_DEBUG("Type: %s\n", dia_token_type_to_string(node->type));
  DIA_DEBUG("Function Name: %s\n", node->name);

  // node traversal to generate parameters
  for (int i=0; i<node->num_of_params; i++) {
    dia_node* _param = node->parameters[i];
    if (node->parameters[i]->type) {
      DIA_DEBUG("- Parameter: %s (%s)\n",
          _param->name, dia_token_type_to_string(_param->type));
    }
    else
      DIA_DEBUG("- Parameter: %s\n", _param->name);

    if (node->parameters[i]->parameters != NULL)
      dia_debug_function_descriptor(node->parameters[i], ++depth);
  }

  DIA_DEBUG("=== Function Structure description Concluded ===\n\n");
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

  if (_FASTER_IO)
    fputs("<<\"\\n\"\n", yyout);
  else
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

dia_node* dia_gets(dia_node* node) {
  DIA_DEBUG("dia_gets\n");

  if (node != NULL && node->num_of_params != 0) yyerror();

  fprintf(yyout, "std::string v%d;\n", VARIABLE_INDEX);
  fprintf(yyout, "std::cin>>v%d;\n", VARIABLE_INDEX);
  DIA_DEBUG("It is about to return a string from the dia_gets function.\n");
  return _dia_create_cpp_variable(DIA_STRING);
}

dia_node* dia_if(dia_node* node, uint8_t _recursed) {
  if (node->num_of_params != 3)
    yyerror("If clause does not have three parameters.");

  DIA_DEBUG("dia_if: Code generating of if-else clause\n");

  if (!_recursed) {
    // First, generate code inside the parenthesis of 'if' clause.
    // Then, the generated code should have the 'v%d' variable.
    // This is why I record the VARIABLE_INDEX first.
    int _variable_index = VARIABLE_INDEX;
    for (dia_node* _node = node; !strcmp(_node->name, "if"); _node = _node->parameters[2])
      dia_generate_code(_node->parameters[0]);
    VARIABLE_INDEX = _variable_index;
  }

  fprintf(yyout, "if(v%d) {\n", VARIABLE_INDEX++);
  if (node->parameters[1]->type == DIA_BOOL ||
      node->parameters[1]->type == DIA_STRING ||
      node->parameters[1]->type == DIA_INTEGER ||
      node->parameters[1]->type == DIA_DOUBLE)
    fprintf(yyout, "return %s;\n", node->parameters[1]->name);
  else if (node->parameters[1]->type == 0 /* void */)
    dia_generate_code(node->parameters[1]);
  else {
    dia_generate_code(node->parameters[1]);
    fprintf(yyout, "return v%d;\n", --VARIABLE_INDEX);
  }

  fputs("}", yyout);
  if (!strcmp(node->parameters[2]->name, "if")) {
    fputs(" else ", yyout);
    dia_if(node->parameters[2], 1);
  }
  else {
    fputs(" else {", yyout);
    if (node->parameters[2]->type == DIA_BOOL ||
        node->parameters[2]->type == DIA_STRING ||
        node->parameters[2]->type == DIA_INTEGER ||
        node->parameters[2]->type == DIA_DOUBLE)
      fprintf(yyout, "return %s;\n", node->parameters[2]->name);
    else {
      dia_generate_code(node->parameters[2]);
      fprintf(yyout, "return v%d;\n", --VARIABLE_INDEX);
    }
    fputs("}\n", yyout);
  }

  if ((node->parameters[1]->type != node->parameters[2]->type) &&
    !strcmp(node->parameters[2]->name, "if"))
  DIA_DEBUG("warning: Body of if and else returns different types: %d, %d\n",
      node->parameters[1]->type, node->parameters[2]->type);
  return node;
}

// Generate function
dia_node* dia_generate_code(dia_node* node) {
  if (node == NULL) {
    DIA_DEBUG("dia_generate_code: the node parameter is NULL\n");
    return NULL;
  }

  if (!strcmp(node->name, "if")) {
    dia_if(node, 0);
    return NULL;
  }

  // If the function in parameter is custom created function,
  // generate code and return v1, v2 ... variable.
  custom_function_t* _func = custom_functions;
  for(; _func; _func = _func->next) {
    DIA_DEBUG_2("dia_generate_code: comparing two thing; %s and %s\n", _func->node->name, node->name);
    if (!strcmp(node->name, _func->node->name)) {
      DIA_DEBUG("dia_generate_code: %s was the custom function.\n", _func->node->name);

      for (int i=0; i<node->num_of_params; i++) {
        if (is_variable(node->parameters[i]))
          node->parameters[i] = dia_generate_code(node->parameters[i]);
      }

      if (_func->node->type == 0 /* void */)
        fprintf(yyout, "%s(", node->name);
      else
        fprintf(yyout, "auto v%d = %s(", VARIABLE_INDEX, node->name);

      for (int i=0; i<node->num_of_params-1; i++)
        fprintf(yyout, "%s,", node->parameters[i]->name);
      if (node->num_of_params)
        fprintf(yyout, "%s", node->parameters[node->num_of_params-1]->name);
      fprintf(yyout, ");\n");
      return _dia_create_cpp_variable(_func->node->type);
    }
  }

  for (int i=0; current_function && i<current_function->num_of_params; i++) {
    if (!strcmp(node->name, current_function->parameters[i]->name)) {
      DIA_DEBUG("dia_generate_code: %s was one of the parameter of "
          "the custom function.\n", node->name);

      node->type = current_function->parameters[i]->type;
      fputs(node->name, yyout);
      return node;
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
    {"gets", dia_gets, 0},
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
    {"equal", dia_equal, 2},
    {"not_equal", dia_not_equal, 2},
    {"greater_equal", dia_greater_equal, 2},
    {"greater", dia_greater, 2},
    {"less_equal", dia_less_equal, 2},
    {"less", dia_less, 2},
    /* Bitwise operation */
    {"bit_and", dia_bit_and, 2},
    {"bit_or", dia_bit_and, 2},
    {"bit_xor", dia_bit_and, 2},
    {"bit_not", dia_bit_and, 1},
    /* Vector */
    {"vector", dia_vector, -1},
    {"at", dia_vector_at, 2},
  };

  for (int i=0; i<node->num_of_params; i++) {
    if (node->parameters[i]->name != NULL) {
      // If the parameter is one of the custom functions
      for (custom_function_t* _function = custom_functions; _function; _function = _function->next) {
        DIA_DEBUG_2("dia_generate_code: searching from the custom functions; %s and %s\n",
            _function->node->name, node->parameters[i]->name);
        if (!strcmp(_function->node->name, node->parameters[i]->name)) {
          node->parameters[i] = dia_generate_code(node->parameters[i]);
          break;
        }
      }

      // If the parameter is one of the predefined functions
      int size = sizeof(functions) / sizeof(functions[0]);
      for (int j=0; j<size; j++) {
        DIA_DEBUG_2("dia_generate_code: searching from the predefined functions; %s and %s\n",
            node->parameters[i]->name, functions[j].identifier);
        if (!strcmp(node->parameters[i]->name, functions[j].identifier)) {
          node->parameters[i] = dia_generate_code(node->parameters[i]);
        }
      }

      // If the identifier is one of the parameter
      for (int j=0; current_function && j<current_function->num_of_params; j++) {
        DIA_DEBUG_2("dia_generate_code: searching from 'current_function'; %s and %s\n",
            node->parameters[i]->name, current_function->parameters[j]->name);
        if (!strcmp(node->parameters[i]->name, current_function->parameters[j]->name))
          node->parameters[i]->type = current_function->parameters[i]->type;
      }
    }
  }

  // Jump to the function if the
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
      return node;
    }
  }

  DIA_DEBUG("dia_generate_code: Perhaps it is in the list of parameters.\n");

  if (_func == NULL) {
    fprintf(stderr, "Undefined method: %s\n", node->name);
    yyerror("Undefined method.");
  }

  return node;
}

// The main function

void _dia_header_definition() {
  fputs("#include<iostream>\n", yyout);
  fputs("#include<string>\n", yyout);
  fputs("#include<vector>\n\n", yyout);
}

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
    if (c == (char)0xff /* EOF */) {
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
  fputs(" */\n\n", yyout);
}

void dia_custom_function(dia_node* node) {
  DIA_DEBUG("=== Function %s Started ===\n", node->name);
  VARIABLE_INDEX = 0;
  current_function = node;

  if (_MAKE_FUNCTIONS_PURE) fprintf(yyout, "[[gnu::pure]] ");
  if (_MAKE_FUNCTIONS_CONSTEXPR) fprintf(yyout, "constexpr ");
  fprintf(yyout, "%s %s(", dia_token_type_to_string(node->type), node->name);
  if (node->num_of_params > 0)
    fprintf(yyout, "%s %s", dia_token_type_to_string(node->type), node->parameters[0]->name);
  for (int i=1; i<node->num_of_params; i++)
    fprintf(yyout, ",%s %s", dia_token_type_to_string(node->type), node->parameters[i]->name);
  fprintf(yyout, ")");
  fprintf(yyout, "{\n");

  // To see the function is a constant function
  // enjoy = "Enjoy!"
  if (!is_variable(node->next_function)) {
    fprintf(yyout, "auto v%d = %s;\n", VARIABLE_INDEX, node->next_function->name);
    ++VARIABLE_INDEX;
  }
  else {
    for (dia_node* _node = node->next_function; _node; _node = _node->next_function)
      dia_generate_code(_node);
  }

  if (node->type != 0 /* void */ && strcmp(node->next_function->name, "if"))
    fprintf(yyout, "return v%d;\n", --VARIABLE_INDEX);
  fputs("}\n", yyout);

  current_function = NULL;
}

void dia_main(dia_node* node) {
  DIA_DEBUG("Lemme write down bill of main function...\n");
  DIA_DEBUG("=== Main Function Started ===\n");

  for (dia_node* _node = node; _node != NULL; _node = _node->next_function)
    dia_debug_function_descriptor(_node, 0);

  DIA_DEBUG("=== Main Function Concluded ===\n\n");
  DIA_DEBUG("Generating the main function code...\n");

  fputs("int main(int argc, char** argv) {\n", yyout);
  if (_FASTER_IO) {
    fputs("std::cin.tie(nullptr);\n", yyout);
    fputs("std::ios_base::sync_with_stdio(false);\n", yyout);
  }

  for (dia_node* _node = node; _node != NULL; _node = _node->next_function)
    dia_generate_code(_node);

  fputs("}", yyout);
  dia_free_node(node);
}
