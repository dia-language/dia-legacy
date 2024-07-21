#ifndef _DIA_NODE_STRUCT
#define _DIA_NODE_STRUCT
typedef int DIA_TOKEN_TYPE;

typedef struct _dia_node {
  char* name;
  int length_of_name;
  int num_of_params;
  struct _dia_node** parameters;
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
