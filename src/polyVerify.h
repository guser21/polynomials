//
// Created by guser on 5/22/17.
//

#ifndef IPP_GS381501_POLYVERIFY_H
#define IPP_GS381501_POLYVERIFY_H

#include "poly.h"

#define OK -42 /// indicates a successful evaluation
/**
 * returns minimum of two values
 * */
#define min(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

/**
 * Char stack node
 * pointer to the nodes
 * */
typedef struct charStackNode {
    char c; /// the character in the stack node
    struct charStackNode* up; /// pointer to the element up in stack
    struct charStackNode* down; /// pointer to the element down in stack
} charStackNode;

/**
 * Char stack
 * pointers to the top and the bottom of the stack
 * */
typedef struct charStack {
    charStackNode* top; /// the element at the top of the Stack
    charStackNode* bottom; /// the element at the bottom of the Stack
    int size; /// the size of the Stack
} charStack;

/**
 * returns the prefix of a given length
 * */
char* getPrefix(char* line, int len);

/**
 * Checks if the number is a proper coefficient of a polynomial
 * */
int isProperLong(char* line, int len);

/**
 * Checks if the number is a proper exponent of a polynomial
 * */
int isProperExp(char* line, int len);

/**
 * Checks if the expression is a proper number
 * */
int isProperNumber(char* line, int len);

/**
 * Checks the proper formatting of the expression
 * @param[in] line expression
 * @param[in] len length of the expression
 * @return either OK or the index of the first illegal charecter +1
 * */
int isProperPolynomial(char* line, int len);

#endif //IPP_GS381501_POLYVERIFY_H
