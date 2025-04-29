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
    "src/vulkan_graphics_environments/vulkan_gpu_connection.cpp"
    "src/vulkan_graphics_environments/glfw_environment.cpp"
    "src/vulkan_graphics_environments/vulkan_environment.cpp"
    "src/vulkan_graphics_environments/vulkan_debug_environment.cpp"
    "src/vulkan_graphics_environments/vulkan_graphics_environment.cpp"
    "src/vulkan_programs/vulkan_gpu_fence.cpp"
    "src/vulkan_programs/vulkan_gpu_semaphore.cpp"
    "src/vulkan_programs/vulkan_gpu_program_manager.cpp"
    "src/vulkan_programs/vulkan_gpu_program.cpp"
    "src/vulkan_windows/window_surface.cpp"
    "src/vulkan_windows/window_swapchain.cpp"
    "src/vulkan_windows/vulkan_camera.cpp"
    "src/vulkan_windows/vulkan_window.cpp"
    "src/vulkan_shaders/vulkan_shader_internal.cpp"
    "src/vulkan_gpu_buffers/vulkan_gpu_buffer_internal.cpp"
    "src/vulkan_drawables/vulkan_brush_internal.cpp"
)
set(RESOURCES
    "resources/CompWolf.Graphics.simple_vertex_shader.vert"
    "resources/CompWolf.Graphics.single_color_pixel_shader.frag"
)
set(TESTS
    "tests/vulkan_graphics_environment.cpp"
    "tests/new_gpu_struct_info.cpp"
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
