//
// Created by guser on 5/16/17.
//
#ifndef WIELOMIANY2_PARSER_H
#define WIELOMIANY2_PARSER_H

#include "polyVerify.h"
/**
 * Stack node to keep the polynomial or the string representation of the polynomial
 * */
typedef struct StackNode {
    struct StackNode* up; ///pointer to the StackNode up in the stack
    struct StackNode* down; ///pointer to the StackNode down in the stack
    Poly p; /// Polynomial
    char* expr; /// string representation of the polynomial
    int format; /// the format identifier
    int expLen; /// the length of the expression
} StackNode;
/**
 * stack to keep polynomials
 * */
typedef struct Stack {
    unsigned int size; /// the size of the stack
    StackNode* top; /// the element at the top of the stack
    StackNode* bottom; /// the element at the bottom of the stack
} Stack;

Poly peek(Stack* stack);


Stack* getNewStack();


Poly popFromStack(Stack* stack);

bool isStackEmpty(Stack* stack);

void destroyStack(Stack* stack);


void parse(char* line, unsigned int len, Stack* stack, unsigned int lineCount);
#endif //WIELOMIANY2_PARSER_H
