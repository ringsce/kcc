//
// Created by Pedro Dias Vicente on 18/10/2025.
//

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdbool.h>
#include "types.h"

// Semantic analysis functions
bool semantic_check_const_assignment(ASTNode *node);
bool semantic_check_tree(ASTNode *node);

#endif // SEMANTIC_H