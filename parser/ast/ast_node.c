//
// Created by moyak on 21.10.2023.
//
#include <malloc.h>
#include "ast_node.h"

static void reallocChildrenList(struct AstNode* parent) {
    struct AstNode** newData = calloc(parent->childrenList.capacity*2 + 2, sizeof(struct AstNode*));
    parent->childrenList.capacity = parent->childrenList.capacity*2 + 2;
    memcpy(newData, parent->childrenList.data, parent->childrenList.childrenCount * sizeof(struct AstNode*));
    free(parent->childrenList.data);
    parent->childrenList.data = newData;
}

void addAstOperation(struct AstNode* parent, struct AstNode* children) {
    if(parent->childrenList.capacity <= parent->childrenList.childrenCount + 1) {
        reallocChildrenList(parent);
    }

    parent->childrenList.data[parent->childrenList.childrenCount] = children;
    parent->childrenList.childrenCount++;
}

void freeNodeNonRecursively(struct AstNode *node) {
    if(node->astNodeValue.type == STRING_TYPE) {
        free(node->astNodeValue._string);
    }
    if(node->childrenList.data != NULL) {
        free(node->childrenList.data);
    }
    free(node);
}