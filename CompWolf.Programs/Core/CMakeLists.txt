cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)
include("../CMake.Libs/resources.cmake")
include("../CMake.Libs/compwolf.cmake")
include("../CMake.Libs/test.cmake")

set(COMPWOLF_TARGET "Core")
set(COMPWOLF_TARGET_TYPE "LIBRARY")
set(DEPENDENT_COMPWOLF_TARGETS
)
set(SOURCES
    "src/version_number.cpp"
)
set(RESOURCES
)
set(TESTS
    "tests/enum_bitset.cpp"
    "tests/event.cpp"
    "tests/listenable.cpp"
    "tests/dimension.cpp"
    "tests/type_list.cpp"
)


project(CompWolf)
compwolf_project(CompWolf)

add_compwolf_target(${COMPWOLF_TARGET} ${COMPWOLF_TARGET_TYPE}
    SOURCES ${SOURCES}
    RESOURCES ${RESOURCES}
)
compwolf_target_get_target_name(TARGET_FULLNAME ${COMPWOLF_TARGET})

target_link_compwolf_target(${TARGET_FULLNAME} ${DEPENDENT_COMPWOLF_TARGETS})

compwolf_add_tests(${COMPWOLF_TARGET} SOURCES ${TESTS})
