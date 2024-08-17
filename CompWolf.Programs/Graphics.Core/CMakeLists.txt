cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)
include("../CMake.Libs/resources.cmake")
include("../CMake.Libs/compwolf.cmake")

set(COMPWOLF_TARGET "Graphics.Core")
set(COMPWOLF_TARGET_TYPE "LIBRARY")
set(DEPENDENT_COMPWOLF_TARGETS
    "Core"
)
set(SOURCES
    "src/graph_test.cpp"
)
set(RESOURCES
    "resources/CompWolf.Graphics.simple_vertex_shader.vert"
    "resources/CompWolf.Graphics.single_color_pixel_shader.frag"
)
set(TESTS
    "tests/test.cpp"
)


project(CompWolf)
compwolf_project(CompWolf)

add_compwolf_target(${COMPWOLF_TARGET} ${COMPWOLF_TARGET_TYPE}
    SOURCES ${SOURCES}
    RESOURCES ${RESOURCES}
)
compwolf_target_get_target_name(TARGET_FULLNAME ${COMPWOLF_TARGET})

target_link_compwolf_target(${TARGET_FULLNAME} ${DEPENDENT_COMPWOLF_TARGETS})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(../extern/glfw extern/glfw)
target_link_libraries(${TARGET_FULLNAME} glfw ${GLFW_LIBRARIES} Vulkan::Vulkan)
target_include_directories(${TARGET_FULLNAME} PUBLIC ../extern/glfw/include/GLFW)
find_package(Vulkan REQUIRED)

compwolf_add_tests(${COMPWOLF_TARGET} SOURCES ${TESTS})
