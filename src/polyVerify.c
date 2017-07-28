//
// Created by guser on 5/22/17.
//
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "polyVerify.h"

#define EXP '1' /// the intended value in the node was exponent
#define COEFF '2' /// the intended value in the node was coefficient
#define POLY '3' /// the intended value in the node was Polynomial
#define PLUS '4' /// the intended value in the node was PlUS

/**
 * returns a new stack node
 * */
charStackNode* getNewCharStackNode(char c) {
    charStackNode* newNode = malloc(sizeof(charStackNode));
    newNode->c = c;
    newNode->up = NULL;
    newNode->down = NULL;
    return newNode;
}

/**
 * creates a new stack
 * */
charStack* getNewCharStack() {
    charStack* cStack = malloc(sizeof(charStack));
    cStack->top = NULL;
    cStack->bottom = NULL;
    cStack->size = 0;
    return cStack;
}

/**
 * adds elements to the stack
 * */
charStackNode* addToCharStack(charStack* stack, char c) {
    charStackNode* node = getNewCharStackNode(c);
    if (stack->size == 0) {
        stack->top = node;
        stack->bottom = node;
        stack->size++;
        return node;
    }
    stack->top->up = node;
    node->down = stack->top;
    stack->top = node;
    stack->size++;
    return node;
}

/**
 * returns the value in the top of the stack and deletes the top element
 * */
char popFromCharStack(charStack* stack) {
    if (stack->top == NULL) {
        return '\0';
    }
    char c = stack->top->c;
    charStackNode* toDelete = stack->top;
    if (stack->top == stack->bottom) {
        stack->top = NULL;
        stack->bottom = NULL;
    } else {
        stack->top = stack->top->down;
        stack->top->up = NULL;
    }
    free(toDelete);
    stack->size--;
    return c;
}
/**
 * checks if the Stack is empty
 * */
bool isCharStackEmpty(charStack* stack) {
    return stack->size == 0;
}

/**
 * deallocates the allocated memory
 * */
void destroyCharStack(charStack* stack) {
    while (!isCharStackEmpty(stack)) {
        popFromCharStack(stack);
    }
    free(stack);
}

/**
 * deletes element of the stack starting from the given node up to the end of the stack
 * */
void deleteCharStackFrom(charStackNode* node, charStack* stack) {
    if (node == NULL) {
        return;
    }
    if (stack->bottom == node) {
        while (!isCharStackEmpty(stack)) {
            popFromCharStack(stack);
        }
        stack->top = NULL;
        stack->bottom = NULL;
        return;
    }

    node->down->up = NULL;
    stack->top = node->down;

    charStackNode* curNode = node;
    while (curNode != NULL) {
        charStackNode* toDelete = curNode;
        curNode = curNode->up;
        free(toDelete);
        stack->size--;
    }
    return;
}

int isProperNumber(char* line, int len) {
    if (line[0] != '-' && !isdigit(line[0])) {
        return 1;
    }
    for (int i = 1; i < len; ++i) {
        if (!isdigit(line[i])) {
            return 1 + i;
        }
    }
    return OK;
}

char* getPrefix(char* line, int len) {
    char* pref = malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; ++i) {
        pref[i] = line[i];
    }
    pref[len] = '\0';
    return pref;
}

int isProperLong(char* line, int len) {
    int ans = isProperNumber(line, min(len, 20));
    if (ans != OK) {
        return ans;
    }
    if (line[0] == '-') {
        unsigned long long num = strtoull(line + 1, NULL, 10);
        char* pref = getPrefix(line + 1, min(19, len));
        unsigned long long prefNum = strtoull(pref, NULL, 10);
        free(pref);
        if (prefNum > (unsigned long) LONG_MAX + 1) {
            return 20;
        }
        if (num > (unsigned long) LONG_MAX + 1) {
            return 21;
        }
        if (len > 20) {
            return 21;
        }

        return OK;
    }

    if (isdigit(line[0])) {
        unsigned long long num = strtoull(line, NULL, 10);
        char* pref = getPrefix(line, min(19, len));
        unsigned long long prefNum = strtoull(pref, NULL, 10);
        free(pref);
        if (prefNum > LONG_MAX) {
            return 19;
        }
        if (num > LONG_MAX) {
            return 20;
        }
        if (len > 19) {
            return 20;
        }
        return OK;
    }
    return -1;
}

int isProperExp(char* line, int len) {
    if (line[0] == '-') {
        return 1;
    }
    int ans = isProperNumber(line, min(len, 11));
    if (ans != OK) {
        return ans;
    }
    if (line[0] == '-') {
        unsigned long long num = strtoull(line + 1, NULL, 10);
        char* pref = getPrefix(line + 1, min(10, len));
        unsigned long long prefNum = strtoull(pref, NULL, 10);
        free(pref);
        if (prefNum > -(long) (INT_MIN)) {
            return 11;
        }
        if (num > -(long) (INT_MIN)) {
            return 12;
        }
        if (len > 11) {
            return 12;
        }

        return OK;
    }

    if (isdigit(line[0])) {
        unsigned long long num = strtoull(line, NULL, 10);
        char* pref = getPrefix(line, min(10, len));
        unsigned long long prefNum = strtoull(pref, NULL, 10);
        free(pref);
        if (prefNum > (long) (INT_MAX)) {
            return 10;
        }
        if (num > (long) (INT_MAX)) {
            return 11;
        }
        if (len > 10) {
            return 11;
        }
        return OK;
    }
    return -1;
}

/**
 * @param[in] len  length of the expression
 * @param[in] lineInit the expression
 * @return first character of the expression which is not decimal digit
 * */
int getNumLen(char* lineInit, int len) {
    int cur = lineInit[0] == '-';
    while (cur < len) {
        if (!isdigit(lineInit[cur])) {
            return cur;
        }
        cur++;
    }
    return cur;
}


int isProperPolynomial(char* line, int len) {
    charStack* stack = getNewCharStack();
    charStackNode* lastOpenBraceNode;
    int i = 0;
    int ans = OK;
    if (line[0] == '-' || isdigit(line[0])) {
        destroyCharStack(stack);
        return isProperLong(line, len);
    }
    while (i < len) {
        if (line[i] == '(') {
            if (stack->top == NULL || stack->top->c == '(' || stack->top->c == PLUS) {
                if (stack->top != NULL && stack->top->c == PLUS) {
                    popFromCharStack(stack);
                }
                lastOpenBraceNode = addToCharStack(stack, '(');
                i++;
                continue;
            }

            ans = i + 1;
            break;
        }

        if (stack->size == 0) {
            ans = i + 1;
            break;
        }
        if (isdigit(line[i]) || line[i] == '-') {
            int numLen = getNumLen(line + i, len - i);
            if (stack->top->c == '(') {
                int isPLong = isProperLong(line + i, numLen);
                if (isPLong == OK) {
                    i = i + numLen;
                    addToCharStack(stack, COEFF);
                    continue;
                } else {
                    ans = i + isPLong;
                    break;
                }
            }
            if (stack->top->c == ',') {
                int isPExp = isProperExp(line + i, numLen);
                if (isPExp == OK) {
                    i = i + numLen;
                    addToCharStack(stack, EXP);
                    continue;
                } else {
                    ans = i + isPExp;
                    break;
                }
            }
            ans = i + 1;
            break;
        }
        if (line[i] == ')') {
            if (stack->top->c == EXP) {
                if (lastOpenBraceNode->down == NULL) {
                    deleteCharStackFrom(lastOpenBraceNode, stack);
                    addToCharStack(stack, POLY);
                    i = i + 1;
                    continue;
                }
                i = i + 1;
                charStackNode* toDeleteFrom = lastOpenBraceNode;
                lastOpenBraceNode = lastOpenBraceNode->down;
                deleteCharStackFrom(toDeleteFrom, stack);
                addToCharStack(stack, POLY);
                continue;
            }
            ans = i + 1;
            break;
        }
        if (line[i] == ',') {
            if (stack->top->c == COEFF || stack->top->c == POLY) {
                addToCharStack(stack, ',');
                i = i + 1;
                continue;
            }
            ans = i + 1;
            break;
        }
        if (line[i] == '+') {
            if (stack->top->c == POLY) {
                popFromCharStack(stack);
                addToCharStack(stack, PLUS);
                i = i + 1;
                continue;
            }
            ans = i + 1;
            break;
        }
        ans = i + 1;
        break;

    }
    if (ans != OK) {
        destroyCharStack(stack);
        return ans;
    }
    if (stack->size == 1 && stack->top->c == POLY) {
        destroyCharStack(stack);
        return OK;
    }
    if (stack->size == 0) {
        destroyCharStack(stack);
        return OK;
    }
    destroyCharStack(stack);
    return len + 1;
}

