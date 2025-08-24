#pragma once

#include <stdbool.h>

#include "tel_chunk.h"
#include "tel_node.h"


// can safely receive a NULL node
bool Tel_compileExpression(Tel_Chunk *chunk, Tel_Node *node, Tel_Byte destination_slot, Tel_Byte next_free_stack_slot);
