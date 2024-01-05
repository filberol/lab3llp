//
// Created by moyak on 22.10.2023.
//
#include "variable_stack.h"

//хранит информацию о переменных в текущем скоупе
static void reallocStack(struct VariableStack *stack) {
    void *newData = calloc(stack->capacity * 2, sizeof(struct VariableNode));
    memcpy(newData, stack->data, stack->countStored * sizeof(struct VariableNode));
    free(stack->data);
    stack->data = newData;
}

void addData(struct VariableStack *stack, struct VariableNode node) {

    if (stack->countStored + 1 < stack->capacity) {
        reallocStack(stack);
    }
    stack->data[stack->countStored] = node;
    stack->countStored++;
}

struct VariableStack *initVariableStack(uint32_t capacity) {
    void *data = calloc(capacity, sizeof(struct VariableNode));
    struct VariableStack *stack = malloc(sizeof(struct VariableStack));
    stack->countStored = 0;
    stack->capacity = capacity;
    stack->data = data;
    return stack;
}

struct VariableNode *getStackVariable(struct VariableStack *stack, char *name) {
    for (int i = 0; i < stack->countStored; i++) {
        if (strcmp(name, stack->data[i].variableName) == 0) {
            return &stack->data[i];
        }
    }
    return NULL;
}

void freeStack(struct VariableStack *stack) {
    if (stack != NULL) {
        free(stack->data);
    }
    free(stack);
}