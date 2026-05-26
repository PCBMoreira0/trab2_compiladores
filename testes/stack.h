#pragma once

typedef struct Stack Stack;

Stack* stack_create();
void stack_push(Stack* s, const void* data);
const void* stack_pop(Stack* s);
const void* stack_peek(Stack* s);
int stack_is_empty(Stack* s);
void stack_destroy(Stack* s);
