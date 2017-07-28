//
// Created by guser on 6/3/17.
//
#ifdef UNIT_TESTING

#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include <memory.h>
#include "cmocka.h"
#include "../src/poly.h"

static char fprintf_buffer[256];///< fprintf buffer
static char printf_buffer[256];///< printf buffer
static int fprintf_position = 0;///< current position in printf buffer
static int printf_position = 0;///< current position in fprintf buffer
static char input_stream_buffer[256];///< buffer for an input stream
static int input_stream_position = 0;///< current position in the buffer
static size_t input_stream_end = 0;///< the index of the last char in the buffer
extern int driveMain(int argc, char* argv[]); ///< mock function for the main

/**
 * Mock function for fprintf
 * buffers are in the file and are kept as static global variables
 * */
int mock_fprintf(FILE* const file, const char* format, ...) {
    int return_value;
    va_list args;

    assert_true(file == stderr);

    assert_true((size_t) fprintf_position < sizeof(fprintf_buffer));

    va_start(args, format);
    return_value = vsnprintf(fprintf_buffer + fprintf_position,
                             sizeof(fprintf_buffer) - fprintf_position,
                             format,
                             args);
    va_end(args);

    fprintf_position += return_value;
    assert_true((size_t) fprintf_position < sizeof(fprintf_buffer));
    return return_value;
}

/**
 * Mock function for printf
 * */
int mock_printf(const char* format, ...) {
    int return_value;
    va_list args;
    assert_true((size_t) printf_position < sizeof(printf_buffer));

    va_start(args, format);
    return_value = vsnprintf(printf_buffer + printf_position,
                             sizeof(printf_buffer) - printf_position,
                             format,
                             args);
    va_end(args);

    printf_position += return_value;
    assert_true((size_t) printf_position < sizeof(printf_buffer));
    return return_value;
}


/**
 * Mock function for getchar
 * */
int mock_getchar() {
    if (input_stream_position < input_stream_end)
        return input_stream_buffer[input_stream_position++];
    else
        return EOF;
}

/**
 * Mock stdin with an input buffer
 */
static void init_input_stream(const char* str) {
    memset(printf_buffer, 0, sizeof(printf_buffer));
    memset(fprintf_buffer, 0, sizeof(fprintf_buffer));
    fprintf_position = 0;
    printf_position = 0;

    memset(input_stream_buffer, 0, sizeof(input_stream_buffer));
    input_stream_position = 0;
    input_stream_end = strlen(str);
    assert_true(input_stream_end < sizeof(input_stream_buffer));
    strcpy(input_stream_buffer, str);
}

static jmp_buf jmp_at_exit;
static int exit_status;

/**
 * Mock function for main function
 */
int mock_main(int argc, char* argv[]) {
    if (!setjmp(jmp_at_exit))
        return driveMain(argc, argv);
    return exit_status;
}

/**
 * Mock function for exit
 */
void mock_exit(int status) {
    exit_status = status;
    longjmp(jmp_at_exit, 1);
}

/**
 * Test for a polynomial = 0
 * */
static void polyZeroTest(void** state) {
    (void) state;
    Poly polyarr[1];
    Poly polyZero = PolyZero();
    Poly res = PolyCompose(&polyZero, 0, polyarr);
    assert_true(PolyIsZero(&res));
}

/**
 * Test for a polynomial = 0
 */
static void polyZeroTestWithConst(void** state) {
    (void) state;
    Poly polyarr[] = {PolyFromCoeff(5)};
    Poly polyZero = PolyZero();
    Poly res = PolyCompose(&polyZero, 1, polyarr);
    assert_true(PolyIsZero(&res));
}

/**
 * Test with Constant polynomial
 */
static void polyConstTest(void** state) {
    (void) state;
    Poly polyarr[1];
    Poly polyConst = PolyFromCoeff(5);
    Poly res = PolyCompose(&polyConst, 0, polyarr);
    assert_true(PolyIsEq(&res, &polyConst));
}

/**
 * Test with 2 Constant Polynomials
 */
static void polyTwoConstTests(void** state) {
    (void) state;
    Poly polyarr[1] = {PolyFromCoeff(7)};
    Poly polyConst = PolyFromCoeff(5);
    Poly res = PolyCompose(&polyConst, 1, polyarr);
    assert_true(PolyIsEq(&res, &polyConst));
}

/**
 * Test for a linear Polynomial
 */
static void polyXCountZeroTest(void** state) {
    (void) state;
    Poly p = PolyFromCoeff(1);
    Mono m = MonoFromPoly(&p, 1);
    Mono mList[] = {m};
    Poly polyX = PolyAddMonos(1, mList);

    Poly mockPolyList[1];
    Poly res = PolyCompose(&polyX, 0, mockPolyList);
    assert_true(PolyIsEq(&res, &polyX));
    PolyDestroy(&res);
    PolyDestroy(&polyX);

}

/**
 * Test with a linear and const Polynomial
 */
static void polyXWithConstTest(void** state) {
    (void) state;
    Poly p = PolyFromCoeff(1);
    Mono m = MonoFromPoly(&p, 1);
    Mono mList[] = {m};
    Poly polyX = PolyAddMonos(1, mList);

    Poly mockPolyList[1] = {PolyFromCoeff(5)};
    Poly res = PolyCompose(&polyX, 1, mockPolyList);
    assert_true(PolyIsEq(&res, &mockPolyList[0]));
    PolyDestroy(&res);
    PolyDestroy(&polyX);
}

/**
 * Test with 2 linear polynomial
 */
static void polyXWithPolyXTest(void** state) {
    (void) state;
    Poly p = PolyFromCoeff(1);
    Mono m = MonoFromPoly(&p, 1);
    Mono mList[] = {m};
    Poly polyX = PolyAddMonos(1, mList);

    Poly mockPolyList[1] = {PolyClone(&polyX)};
    Poly res = PolyCompose(&polyX, 1, mockPolyList);
    assert_true(PolyIsEq(&res, &mockPolyList[0]));
    PolyDestroy(&res);
    PolyDestroy(&polyX);
    PolyDestroy(&mockPolyList[0]);

}
/**
 * Test no Argument
 * */
static void noArgTest(void** state) {
    (void) state;
    init_input_stream("COMPOSE\n");
    assert_int_equal(mock_main(1, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test COMPOSE 0
 * * */
static void minValueTest(void** state) {
    (void) state;
    init_input_stream("COMPOSE 0\n");

    assert_int_equal(mock_main(1, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "");
}

/**
 * Test COMPOSE UMAX_INT
 * */
static void maxValueTest(void** state) {
    (void) state;
    init_input_stream("COMPOSE 4294967295");

    assert_int_equal(mock_main(1, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 STACK UNDERFLOW\n");
}

/**
 * Test COMPOSE -1
 * */
static void oneOffMinTest(void** state) {
    (void) state;
    init_input_stream("COMPOSE -1");

    assert_int_equal(mock_main(1, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test COMPOSE UMAX_INT+1
 * */
static void maxPlusOneTest(void** state) {
    (void) state;
    init_input_stream("COMPOSE 4294967296");

    assert_int_equal(mock_main(1, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test COMPOSE number much bigger than UINT_MAX
 * */
static void bigNumTest(void** state) {
    (void) state;
    init_input_stream("COMPOSE 785372657648767417");

    assert_int_equal(mock_main(1, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test COMPOSE random  letters
 * */
static void randomLettersTest(void** state) {
    (void) state;
    init_input_stream("COMPOSE akdmASKMoP");

    assert_int_equal(mock_main(1, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");

}

/**
 * Test COMPOSE random letters and numbers
 * */
static void randomCharsTest(void** state) {
    (void) state;
    init_input_stream("COMPOSE 7s7skmn9jnje");

    assert_int_equal(mock_main(1, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}
/**
 * Current main function
 * */
int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(polyZeroTest),
            cmocka_unit_test(polyTwoConstTests),
            cmocka_unit_test(polyConstTest),
            cmocka_unit_test(polyZeroTestWithConst),
            cmocka_unit_test(polyXCountZeroTest),
            cmocka_unit_test(polyXWithConstTest),
            cmocka_unit_test(polyXWithPolyXTest),
            cmocka_unit_test(noArgTest),
            cmocka_unit_test(minValueTest),
            cmocka_unit_test(maxValueTest),
            cmocka_unit_test(oneOffMinTest),
            cmocka_unit_test(maxPlusOneTest),
            cmocka_unit_test(bigNumTest),
            cmocka_unit_test(randomCharsTest),
            cmocka_unit_test(randomLettersTest)

    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}

#endif