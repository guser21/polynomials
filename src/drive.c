//
// Created by guser on 5/19/17.
//

#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <limits.h>
#include "parser.h"
#include "../test/main_mock.h"

/**
 * reads character by character from the file and calls the parse function
 * */
//void run(char* fileName) {
void run(){
//    FILE* file = fopen(fileName, "r");
//    int character = fgetc(file);
    int character = getchar();
    Stack* stack = getNewStack();
    unsigned int lineCount = 0;

    while (character != EOF) {
        unsigned int curLineLength = 0;
        unsigned int curLineCapacity = 4;
        char* line = malloc(4 * sizeof(char));
        lineCount++;
        while (character != '\n' && character != EOF) {
            if (curLineLength >= curLineCapacity - 2) {
                line = realloc(line, 2 * sizeof(char) * curLineCapacity);
                curLineCapacity = 2 * curLineCapacity;
            }
            line[curLineLength] = (char) character;
            curLineLength++;
//            character = fgetc(file);
            character = getchar();
        }
        line[curLineLength] = '\0';
        char* lineToStack = malloc((curLineLength + 1) * sizeof(char));
        strcpy(lineToStack, line);
        lineToStack[curLineLength] = '\0';
        free(line);
        parse(lineToStack, curLineLength, stack, lineCount);
//        character = fgetc(file);
        character = getchar();
    }
    destroyStack(stack);
//    fclose(file);
}

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
//    run("lines.txt");
//    printf("%ld \n", LONG_MIN);
//    printf("%ld", LONG_MAX);
//    run(argv[1]);
    run();
    return 0;
}