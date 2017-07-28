//
// Created by guser on 5/16/17.
//
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <limits.h>

#include "parser.h"

#define FPoly 1 /// indicates that the stack node keeps the data in a Polynomial format
#define FChar 2 /// indicates that the stack node keeps the data in a Character format
#define FBoth 3 /// indicates that the stack node keeps the data in both formats

Poly polyParse(char* line, int* curIndex, int length);

/**
 * creates and returns a new stack Node
 * */
StackNode* getNewStackNode() {
    StackNode* newStackNode = malloc(sizeof(StackNode));
    newStackNode->expr = NULL;
    newStackNode->up = NULL;
    newStackNode->down = NULL;
    return newStackNode;
}

/**
 * creates and returns a new stack
 * */
Stack* getNewStack() {
    Stack* newStack = malloc(sizeof(Stack));
    newStack->top = NULL;
    newStack->bottom = NULL;
    newStack->size = 0;
    return newStack;
}

/**
 * adds the stack node to the stack
 * */
void addNodeToStack(Stack* stack, StackNode* newNode) {
    if (stack->top == NULL) {
        stack->size++;
        stack->bottom = newNode;
        stack->top = newNode;
        return;
    }
    stack->size++;
    stack->top->up = newNode;
    newNode->down = stack->top;
    stack->top = newNode;

}

/**
 * adds the polynomial to the stack (support for lazy evaluation)
 * */
void addPolyToStack(Stack* stack, Poly p) {
    StackNode* newNode = getNewStackNode();
    newNode->p = p;
    newNode->format = FPoly;
    newNode->expr = NULL;
    addNodeToStack(stack, newNode);

}

/**
 * adds the expression to the stack to (support for lazy evaluation)
 * */
void addExprToStack(Stack* stack, char* expr, int len) {
    StackNode* newNode = getNewStackNode();
    newNode->expr = expr;
    newNode->expLen = len;
    newNode->format = FChar;
    addNodeToStack(stack, newNode);

}

/**
 * returns the top element in the stack
 * */
Poly peek(Stack* stack) {
    if (stack->top->format != FChar) {
        return stack->top->p;
    }
    int i = 0;
    Poly p = polyParse(stack->top->expr, &i, stack->top->expLen);
    stack->top->p = p;
    stack->top->format = FBoth;
    return p;
}

/**
 * deletes the top element in the stack
 * */
void deleteFirst(Stack* stack) {
    if (stack->size == 0) {
        assert(stack->top = NULL);
        return;
    }
    StackNode* toDelete = stack->top;
    stack->size--;
    if (stack->top == stack->bottom) {
        stack->top = NULL;
        stack->bottom = NULL;
    } else {
        stack->top = stack->top->down;
        stack->top->up = NULL;
    }

    if (toDelete->format == FChar) {
        free(toDelete->expr);
        free(toDelete);
        return;
    }
    if (toDelete->format == FBoth) {
        free(toDelete->expr);
        PolyDestroy(&toDelete->p);
        free(toDelete);
        return;
    }
    if (toDelete->format == FPoly) {
        PolyDestroy(&toDelete->p);
        free(toDelete);
        return;
    }
    return;

}

/**
 * returns the element on the top of the stack
 * and deletes the element from stack
 * */
Poly popFromStack(Stack* stack) {
    Poly p = peek(stack);
    StackNode* toDelete = stack->top;
    stack->size--;
    if (stack->top == stack->bottom) {
        stack->top = NULL;
        stack->bottom = NULL;
    } else {
        stack->top = stack->top->down;
        stack->top->up = NULL;
    }

    free(toDelete->expr);
    free(toDelete);
    return p;

}
/**
 * checks if the stack is empty
 * */
bool isStackEmpty(Stack* stack) {
    return stack->size == 0;
}

/**
 * destroys the stack and frees the allocated memory
 * */
void destroyStack(Stack* stack) {
    while (!isStackEmpty(stack)) {
        deleteFirst(stack);
    }
    free(stack);
}


/**
 * Parses and finds the the appropriate command to execute
 */
void parseCommand(char* line, unsigned int len, Stack* stack, unsigned int lineCount) {

    if (strcmp("ZERO", line) == 0) {
        addPolyToStack(stack, PolyZero());
        return;
    }

    if (strncmp(line, "COMPOSE", 7) == 0) {
        long a = atol(line + 8);
        if (isProperLong(line + 8, len - 8) != OK || a < 0 || a > UINT_MAX) {
            fprintf(stderr, "ERROR %d WRONG COUNT\n", lineCount);
            return;
        }
        if (a == 0) { return; }
        if (stack->size < a + 1) {
            fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", lineCount);
            return;
        }

        Poly p = popFromStack(stack);
        Poly* arr = malloc(sizeof(Poly) * 4);
        int curCount = 0;
        int arrSize = 4;
        while (curCount < a) {
            if (arrSize < curCount + 1) {
                arr = realloc(arr, 2 * arrSize * sizeof(Poly));
                arrSize *= 2;
            }
            arr[curCount] = popFromStack(stack);
            curCount++;
        }
        Poly res = PolyCompose(&p, (unsigned) a, arr);
        addPolyToStack(stack, res);
        curCount = 0;
        while (curCount < a) {
            PolyDestroy(&arr[curCount]);
            curCount++;
        }
        free(arr);
        PolyDestroy(&p);
        return;
    }

    if (isStackEmpty(stack)) {
        fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", lineCount);
        return;
    }
    if (strcmp("PRINT", line) == 0) {

        printPolynomial(peek(stack));
        putchar('\n');
        return;
    }
    if (strncmp(line, "AT", 2) == 0) {
        long long a = atoll(line + 3);
        if (isProperLong(line + 3, len - 3) != OK) {
            fprintf(stderr, "ERROR %d WRONG VALUE\n", lineCount);
            return;
        }
        Poly p = popFromStack(stack);
        addPolyToStack(stack, PolyAt(&p, a));
        PolyDestroy(&p);
        return;
    }

    if (strncmp(line, "DEG_BY", 6) == 0) {
        long a = atol(line + 7);
        if (a > UINT_MAX || a < 0 || isProperNumber(line + 7, len - 7) != OK) {
            fprintf(stderr, "ERROR %d WRONG VARIABLE\n", lineCount);
            return;
        }
        Poly p = peek(stack);
        printf("%d\n", PolyDegBy(&p, a));
        return;
    }

    if (strcmp("DEG", line) == 0) {
        Poly p = peek(stack);
        printf("%d\n", PolyDeg(&p));
        return;
    }

    if (strcmp("POP", line) == 0) {
        deleteFirst(stack);
        return;
    }
    if (strcmp("NEG", line) == 0) {
        Poly p = peek(stack);
        PolyNegDest(&p);
        if (stack->top->format != FPoly) {
            if (stack->top->expr != NULL) {
                free(stack->top->expr);
                stack->top->expr = NULL;
            }
            stack->top->format = FPoly;
        }
        return;
    }

    if (strcmp("CLONE", line) == 0) {
        if (stack->top->format == FChar) {
            char* exprCopy = malloc(sizeof(char) * stack->top->expLen);
            strcpy(exprCopy, stack->top->expr);
            addExprToStack(stack, exprCopy, stack->top->expLen);
            return;
        }
        Poly q = PolyClone(&stack->top->p);
        addPolyToStack(stack, q);

        if (stack->top->format == FBoth) {
            char* exprCopy = malloc(sizeof(char) * stack->top->expLen);
            strcpy(exprCopy, stack->top->expr);
            stack->top->format = FBoth;
            stack->top->expLen = stack->top->down->expLen;
            stack->top->expr = exprCopy;
        }
        return;
    }
    if (strcmp("IS_COEFF", line) == 0) {
        Poly p = peek(stack);
        if (PolyIsCoeff(&p)) {
            printf("%d\n", 1);
        } else {
            printf("%d\n", 0);
        }
        return;
    }
    if (strcmp("IS_ZERO", line) == 0) {
        Poly p = peek(stack);
        if (PolyIsZero(&p)) {
            printf("%d\n", 1);
        } else {
            printf("%d\n", 0);
        }
        return;
    }

    if (stack->size < 2) {
        fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", lineCount);
        return;
    }

    if (strcmp("ADD", line) == 0) {
        Poly p1 = popFromStack(stack);
        Poly p2 = popFromStack(stack);
        PolyAddDest(&p1, &p2);
        addPolyToStack(stack, p1);
        return;
    }
    if (strcmp("MUL", line) == 0) {
        Poly p1 = popFromStack(stack);
        Poly p2 = popFromStack(stack);
        Poly mul = PolyMul(&p1, &p2);

        addPolyToStack(stack, mul);
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        return;
    }

    if (strcmp("SUB", line) == 0) {
        Poly p1 = popFromStack(stack);
        Poly p2 = popFromStack(stack);
        PolyNegDest(&p2);
        PolyAddDest(&p1, &p2);
        addPolyToStack(stack, p1);
        return;
    }
    if (strcmp("IS_EQ", line) == 0) {
        Poly p1 = peek(stack);
        Poly p2;
        if (stack->top->down->format == FChar) {
            int i = 0;
            p2 = polyParse(stack->top->down->expr, &i, stack->top->down->expLen);
            stack->top->down->p = p2;
            stack->top->down->format = FBoth;
        } else {
            p2 = stack->top->down->p;
        }
        bool equal = PolyIsEq(&p1, &p2);
        if (equal) {
            printf("%d\n", 1);
        } else {
            printf("%d\n", 0);
        }
        return;
    }


}
/**
 * verifies that the entered command is legit
 * @param[in] line
 * @param[in] lineCount
 * @return bool
 * */
bool verifyCommand(char* line, unsigned int lineCount) {
    if ((strcmp("IS_EQ", line) == 0) || (strcmp("SUB", line) == 0) ||
        (strcmp("MUL", line) == 0) || (strcmp("ADD", line) == 0) ||
        (strcmp("IS_ZERO", line) == 0) || (strcmp("IS_COEFF", line) == 0) ||
        (strcmp("CLONE", line) == 0) || (strcmp("NEG", line) == 0) ||
        (strcmp("POP", line) == 0) || (strcmp("DEG", line) == 0) ||
        (strncmp(line, "DEG_BY", 6) == 0) || (strncmp(line, "AT", 2) == 0) ||
        (strcmp("PRINT", line) == 0) || (strcmp("ZERO", line) == 0)
        || (strncmp(line, "COMPOSE", 7) == 0)) {
        return true;
    }
    fprintf(stderr, "ERROR %d WRONG COMMAND\n", lineCount);
    return false;
}

/**
 * calls the appropriate function according to the line format
 * */
void parse(char* line, unsigned int len, Stack* stack, unsigned int lineCount) {
    if (len == 0) {
        free(line);
        return;
    }
    if (isalpha(line[0])) {
        if (verifyCommand(line, lineCount)) {
            parseCommand(line, len, stack, lineCount);
            free(line);
            return;
        }
        free(line);
        return;
    }
    if (isdigit(line[0]) || line[0] == '(' || line[0] == '-') {
        int ans = isProperPolynomial(line, len);
        if (ans == OK) {
            addExprToStack(stack, line, len);
        } else {
            free(line);
            fprintf(stderr, "ERROR %d %d\n", lineCount, ans);
        }

        return;
    }
    free(line);
    fprintf(stderr, "ERROR %d %d\n", lineCount, 1);
    return;
}

/**
 * returns the index of the first char that is not digit
 * */
int getNumLength(char* line, int len) {
    for (int i = 0; i < len; ++i) {
        if (!isdigit(line[i])) {
            return i;
        }
    }
    return -1;
}


Mono monoParse(char* line, int* curIndex, int length);

/**
 * Parses Polynomial
 * @param[in] len: the length of the expression
 * @param[in] expr: the expression
 * @param[in] curIndex: just a hack to escape keeping global variable
 * @return monomial
 * */
Poly polyParse(char* line, int* curIndex, int length) {
    if (isdigit(line[*curIndex]) || line[*curIndex] == '-') {
        int sign = 1;
        long number = 0;
        if (line[*curIndex] == '-') {
            sign = -1;
            *curIndex = *curIndex + 1;
        }
        int shift = getNumLength(line + (*curIndex), length - *curIndex);
        number = strtol(line + *curIndex, NULL, 10);
        *curIndex = *curIndex + shift;
        Poly res = PolyFromCoeff(sign * number);
        return res;
    }

    unsigned int curLength = 0;
    unsigned int curSize = 200;
    Mono mono;

    Mono* monoArray = (Mono*) malloc(curSize * sizeof(Mono));
    while (line[*curIndex] != ',' && *curIndex < length) {
        mono = monoParse(line, curIndex, length);
        *curIndex = *curIndex + 1;
        if (*curIndex < length) {
            if (line[*curIndex] == '+')
                *curIndex = *curIndex + 1;
        }
        curLength++;
        if (curLength >= curSize) {
            curSize = curSize * 2;
            monoArray = realloc(monoArray, curSize * sizeof(Mono));
        }
        monoArray[curLength - 1] = mono;
    }
    Poly res = PolyAddMonos(curLength, monoArray);
    free(monoArray);
    return res;
}

/**
 * Parses monomials
 * @param[in] len: the length of the expression
 * @param[in] expr: the expression
 * @return monomial
 * */

Mono monoParse(char* line, int* curIndex, int length) {
    *curIndex = *curIndex + 1;
    Poly poly = polyParse(line, curIndex, length);
    *curIndex = *curIndex + 1;

    int number = (int) strtol(line + *curIndex, NULL, 10);
    int shift = getNumLength(line + *curIndex, length - *curIndex);
    *curIndex = *curIndex + shift;

    Mono res = MonoFromPoly(&poly, number);
    return res;
}

//
//int main(void) {
//    int i = 0;
//
//
//    Poly p1 = polyParse("((1,0)+(1,1),1)", &i, 15);
//    i = 0;
//    Poly p2 = polyParse("(1,4)", &i, 5);
//    i = 0;
//    Poly p3 = polyParse("(((1,6),5),2)+((1,0)+(1,2),3)+(5,7)", &i, 35);
//
//    Poly arr[] = {p2, p1};
//    Poly res = PolyCompose(&p3, 2, arr);
//
//    printPolynomial(res);
//    PolyDestroy(&p1);
//    PolyDestroy(&p2);
//    PolyDestroy(&p3);
//    PolyDestroy(&res);
//    return 0;
//}
