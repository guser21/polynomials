cmake_minimum_required(VERSION 3.0)
set(CMAKE_C_STANDARD 11)


if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_FLAGS "-std=c11 -Wall -DUNIT_TESTING -Wextra ")

# find_program (CTEST_MEMORYCHECK_COMMAND NAMES valgrind)
find_library(CMOCKA cmocka)

if (NOT CMOCKA)
    message(FATAL_ERROR "Could not find cmocka.")
endif ()

enable_testing()

set(SOURCE_FILES

        src/poly.c
        src/poly.h
        src/parser.c
        src/drive.c
        src/parser.h
        src/polyVerify.c
        src/polyVerify.h

        )
add_executable(calculator  ${SOURCE_FILES})

add_executable(polyComposeTest  test/polyComposeTest.c
        test/utility_mocks.h test/main_mock.h ${SOURCE_FILES})
#
set_target_properties(
        polyComposeTest
        PROPERTIES
        COMPILE_DEFINITIONS UNIT_TESTING=1)
#
target_link_libraries(polyComposeTest ${CMOCKA})
add_test(polyComposeTest ${CMAKE_CURRENT_BINARY_DIR}/calculator_test)
