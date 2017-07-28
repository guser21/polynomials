//
// Created by guser on 6/3/17.
//

#ifndef PROJECT_MOCKMEMALLOCS_H
#define PROJECT_MOCKMEMALLOCS_H

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <glob.h>
#include "cmocka.h"

#ifdef UNIT_TESTING

#ifdef assert
#undef assert
#endif /* assert */
#define assert(expression) \
    mock_assert((int)(expression), #expression, __FILE__, __LINE__)

void mock_assert(const int result, const char* expression, const char* file,
                 const int line);

/* Redirect calloc, malloc, realloc and free to _test_malloc, _test_calloc,
 * _test_realloc and _test_free, respectively, so cmocka can check for memory
 * leaks. */
#ifdef calloc
#undef calloc
#endif /* calloc */
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)

void* _test_calloc(size_t number_of_elements, size_t size, const char* file, int line);

#ifdef malloc
#undef malloc
#endif /* malloc */
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)

void* _test_malloc(const size_t size, const char* const file, const int line);

#ifdef realloc
#undef realloc
#endif /* realloc */
#define realloc(ptr, size) _test_realloc(ptr, size, __FILE__, __LINE__)

void* _test_realloc(void* const ptr, const size_t size, const char* const file, const int line);

#ifdef free
#undef free
#endif /* free */
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)

void _test_free(void* const ptr, const char* const file, const int line);


#ifdef printf
#undef printf
#endif /* printf */
#define printf(...) mock_printf(__VA_ARGS__)

extern int mock_printf(const char* format, ...);


#ifdef fprintf
#undef fprintf
#endif /* fprintf */
#define fprintf(...) mock_fprintf(__VA_ARGS__)

extern int mock_fprintf(FILE* const file, const char* format, ...);


#ifdef getchar
#undef getchar
#endif /* getchar */
#define getchar() mock_getchar()

extern int mock_getchar();


#ifdef exit
#undef exit
#endif /* exit */
#define exit(status) mock_exit(status)

extern void mock_exit(int status);

#endif
#endif //PROJECT_MOCKMEMALLOCS_H
