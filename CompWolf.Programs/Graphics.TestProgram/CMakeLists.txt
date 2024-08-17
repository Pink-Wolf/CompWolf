cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)
include("../CMake.Libs/resources.cmake")
include("../CMake.Libs/compwolf.cmake")

set(COMPWOLF_TARGET "Graphics.TestProgram")
set(COMPWOLF_TARGET_TYPE "EXECUTABLE")
set(DEPENDENT_COMPWOLF_TARGETS
    "Core"
    "Graphics.Core"
)
set(SOURCES
    "src/main.cpp"
)
set(RESOURCES
    "resources/vertex.vert"
    "resources/pixel.frag"
)


project(CompWolf)
compwolf_project(CompWolf)

add_compwolf_target(${COMPWOLF_TARGET} ${COMPWOLF_TARGET_TYPE}
    SOURCES ${SOURCES}
    RESOURCES ${RESOURCES}
)
compwolf_target_get_target_name(TARGET_FULLNAME ${COMPWOLF_TARGET})

target_link_compwolf_target(${TARGET_FULLNAME} ${DEPENDENT_COMPWOLF_TARGETS})
