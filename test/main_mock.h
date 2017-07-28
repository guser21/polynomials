//
// Created by guser on 6/3/17.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H

#ifndef UTILS_H
#define UTILS_H

#ifdef UNIT_TESTING


#include <stdio.h>

#define main(...) driveMain(__VA_ARGS__)
int driveMain(int argc, char *argv[]);


#endif

#endif /* UTILS_H*/
#endif //PROJECT_UTILS_H
