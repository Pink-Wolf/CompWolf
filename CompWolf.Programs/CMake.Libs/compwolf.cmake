cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)
include("../CMake.Libs/resources.cmake")

macro(compwolf_project)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endmacro()

macro(compwolf_target_get_target_name VARIABLE COMPWOLF_TARGET)
    set(${VARIABLE} "CompWolf.${COMPWOLF_TARGET}")
endmacro()

function(add_compwolf_target COMPWOLF_TARGET TYPE)
    # get args
    list(FIND ARGV "SOURCES" SOURCE_INDEX)
    math(EXPR SOURCE_INDEX ${SOURCE_INDEX}+1)
    list(FIND ARGV "RESOURCES" RESOURCE_INDEX)
    math(EXPR RESOURCE_INDEX ${RESOURCE_INDEX}+1)

    math(EXPR SOURCE_COUNT ${RESOURCE_INDEX}-${SOURCE_INDEX}-1)
    if (${SOURCE_COUNT} LESS 0)
        set(SOURCE_COUNT -1)
    endif()
    
    if (${SOURCE_INDEX} GREATER 1 AND ${SOURCE_INDEX} LESS ${ARGC} AND NOT ${SOURCE_COUNT} EQUAL 0)
        list(SUBLIST ARGV ${SOURCE_INDEX} ${SOURCE_COUNT} SOURCES)
    else()
        set(SOURCES "")
    endif()
    if (${RESOURCE_INDEX} GREATER 1 AND ${RESOURCE_INDEX} LESS ${ARGC})
        list(SUBLIST ARGV ${RESOURCE_INDEX} -1 RESOURCES)
    else()
        set(RESOURCES "")
    endif()
    # add library
    compwolf_target_get_target_name(TARGET_FULLNAME ${COMPWOLF_TARGET})

    if(TYPE STREQUAL "LIBRARY")
        add_library(${TARGET_FULLNAME} ${SOURCES})
    else()
        add_executable(${TARGET_FULLNAME} ${SOURCES})
    endif()

    target_include_directories(${TARGET_FULLNAME} PUBLIC include)
    target_include_directories(${TARGET_FULLNAME} PRIVATE header)
    target_add_resources(${TARGET_FULLNAME} ${RESOURCES})
endfunction()

function(target_link_compwolf_target TARGET)
    if (${ARGC} LESS_EQUAL 1)
        return()
    endif()
    list(SUBLIST ARGV 1 -1 DEPENDENCY_TARGETS)

    foreach(DEPENDENCY_TARGET ${DEPENDENCY_TARGETS})
        compwolf_target_get_target_name(DEPENDENCY_TARGET_FULL_NAME ${DEPENDENCY_TARGET})
        add_subdirectory(../${DEPENDENCY_TARGET} ${DEPENDENCY_TARGET_FULL_NAME})
        target_link_libraries(${TARGET} ${DEPENDENCY_TARGET_FULL_NAME})
        target_link_resources(${TARGET} ${DEPENDENCY_TARGET_FULL_NAME})
    endforeach()
endfunction()
