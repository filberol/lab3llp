#include "common_stack.h"
static void reallocStack(struct Stack *stack) {
    void *newData = calloc(stack->capacity * 2, sizeof(void*));
    memcpy(newData, stack->data, stack->countStored * sizeof(void*));
    free(stack->data);
    stack->data = newData;
}
void* pop(struct Stack* stack) {
    if(stack->countStored < 0) {
        return NULL;
    }
    stack->countStored--;
    return stack->data[stack->countStored];
       
}

void push(struct Stack* stack, void* data) {
    if(stack->capacity < stack->countStored+1) {
        reallocStack(stack);
    }
    stack->data[stack->countStored] = data;
    stack->countStored++;
}
struct Stack* initStack() {
    struct Stack* stack = malloc(sizeof(struct Stack));
    stack->data = calloc(16, sizeof(void*));
    stack->countStored = 0;
    stack->capacity = 16;
    return stack;
}
void* peek(struct Stack* stack) {
        if(stack->countStored < 0) {
        return NULL;
    }
    return stack->data[stack->countStored-1];
}