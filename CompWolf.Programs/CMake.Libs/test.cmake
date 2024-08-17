cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)
include("../CMake.Libs/compwolf.cmake")

add_subdirectory(../extern/googletest extern/googletest)

function(compwolf_add_tests COMPWOLF_TARGET)
    if (ARGC GREATER 1)
        list(SUBLIST ARGV 1 -1 REMAINING_ARGS)
    endif()
    # Create test target
    set(TEST_COMPWOLF_TARGET "${COMPWOLF_TARGET}.Tests")
    add_compwolf_target(${TEST_COMPWOLF_TARGET} EXECUTABLE ${REMAINING_ARGS})
    compwolf_target_get_target_name(TEST_TARGET_FULLNAME ${TEST_COMPWOLF_TARGET})
    target_link_compwolf_target(${TEST_TARGET_FULLNAME} ${COMPWOLF_TARGET})
    # Link Google test
    enable_testing()
    target_include_directories(${TEST_TARGET_FULLNAME} PUBLIC ../extern/googletest/include/)
    target_link_libraries(${TEST_TARGET_FULLNAME} GTest::gtest GTest::gtest_main)
endfunction()
