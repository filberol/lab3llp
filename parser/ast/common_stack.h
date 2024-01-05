#ifndef LLP_STACK_H
#define LLP_STACK_H
#include <stdint.h>
#include "variable.h"
#include <stdlib.h>
#include <string.h>

struct Stack {
    int32_t capacity;
    int32_t countStored;
    void** data;
};

void* pop(struct Stack* stack);

void push(struct Stack* stack, void* data);

void* peek(struct Stack* stack);

struct Stack* initStack();
//todo clear


#endif