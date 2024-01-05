//
// Created by moyak on 22.10.2023.
//

#ifndef LLP_VARIABLE_H
#define LLP_VARIABLE_H
#include "ast_node.h"
struct VariableNode {
    enum AstNodeValueType type;
    int depth;
    char* variableName;
    void* val;
};
#endif //LLP_VARIABLE_H
