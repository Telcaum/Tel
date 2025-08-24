#include "tel_node.h"


Tel_Node *Tel_newNode(Tel_NodeType type, Tel_Token token, Tel_NodeAs as) {
  Tel_Node *node = TEL_ALLOC(Tel_Node);
  TEL_CHECK_OOM(node);
  node->type = type;
  node->token = token;
  node->as = as;

  return node;
}
