cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)

macro(target_get_resources VARIABLE TARGET)
    get_target_property(${VARIABLE} ${TARGET} "resource_files")
    if ("${${VARIABLE}}" STREQUAL "${VARIABLE}-NOTFOUND")
        set(${VARIABLE} "")
    endif()
endmacro()
macro(target_get_resource_file VARIABLE TARGET RESOURCE)
    get_target_property(${VARIABLE} ${TARGET} BINARY_DIR)
    set(${VARIABLE} "${${VARIABLE}}/${RESOURCE}")
endmacro()

function(target_register_resources TARGET)
    if (${ARGC} LESS_EQUAL 1)
        return()
    endif()
    list(SUBLIST ARGV 1 -1 RESOURCES)

    set_property(TARGET ${TARGET} APPEND PROPERTY "resource_files" ${RESOURCES})
endfunction()

function(target_add_resources TARGET)
    if (${ARGC} LESS_EQUAL 1)
        return()
    endif()
    list(SUBLIST ARGV 1 -1 RESOURCES)
    
    get_target_property(RESOURCE_SOURCE_DIR ${TARGET} SOURCE_DIR)
    get_target_property(RESOURCE_BINARY_DIR ${TARGET} BINARY_DIR)

    foreach(RESOURCE ${RESOURCES})
        # Split RESOURCE (path) into directory, file name, and file extension
        string(FIND ${RESOURCE} "/" RESOURCE_DIRECTORY_INDEX REVERSE)
        MATH(EXPR RESOURCE_DIRECTORY_INDEX ${RESOURCE_DIRECTORY_INDEX}+1)
        string(FIND ${RESOURCE} "." RESOURCE_EXTENSION_INDEX REVERSE)
        MATH(EXPR RESOURCE_NAME_LENGTH ${RESOURCE_EXTENSION_INDEX}-${RESOURCE_DIRECTORY_INDEX})

        string(SUBSTRING ${RESOURCE} 0 ${RESOURCE_DIRECTORY_INDEX} RESOURCE_DIRECTORY)
        string(SUBSTRING ${RESOURCE} ${RESOURCE_DIRECTORY_INDEX} ${RESOURCE_NAME_LENGTH} RESOURCE_NAME)
        string(SUBSTRING ${RESOURCE} ${RESOURCE_EXTENSION_INDEX} -1 RESOURCE_EXTENSION)

        set(INPUT_RESOURCE "${RESOURCE_SOURCE_DIR}/${RESOURCE}")
        set(OUTPUT_RESOURCE "${RESOURCE_BINARY_DIR}/${RESOURCE}")
        # Some programs need directory of output to exist
        make_directory("${RESOURCE_BINARY_DIR}/${RESOURCE_DIRECTORY}")
        # Use glslc to compile shaders
        if (RESOURCE_EXTENSION STREQUAL ".vert" OR RESOURCE_EXTENSION STREQUAL ".frag")
            set(RESOURCE "${RESOURCE_DIRECTORY}${RESOURCE_NAME}.spv")
            set(OUTPUT_RESOURCE "${RESOURCE_BINARY_DIR}/${RESOURCE}")
            set(SHADER_COMPILER "${RESOURCE_SOURCE_DIR}/../extern/shader_compiler/compile.sh")
            if (WIN32)
                set(SHADER_COMPILER "${RESOURCE_SOURCE_DIR}/../extern/shader_compiler/compile.bat")
            else()
                execute_process(COMMAND chmod +x ${SHADER_COMPILER})
            endif()

            add_custom_command(
                OUTPUT ${OUTPUT_RESOURCE}
                MAIN_DEPENDENCY ${INPUT_RESOURCE}
                COMMAND ${SHADER_COMPILER} ${INPUT_RESOURCE} ${OUTPUT_RESOURCE}
                #COMMAND glslc ${INPUT_RESOURCE} -o ${OUTPUT_RESOURCE}
                VERBATIM
            )
        # Otherwise simply copy resource
        else()
            add_custom_command(
                OUTPUT ${OUTPUT_RESOURCE}
                MAIN_DEPENDENCY ${INPUT_RESOURCE}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${INPUT_RESOURCE}
                    ${OUTPUT_RESOURCE}
                VERBATIM
            )
        endif()
        # Make Target dependent on resource
        string(REPLACE "/" "." RESOURCE_NAME ${RESOURCE})
        set(RESOURCE_NAME "${TARGET}.${RESOURCE_NAME}")
        add_custom_target(${RESOURCE_NAME}
            DEPENDS ${OUTPUT_RESOURCE}
        )
        add_dependencies(${TARGET} ${RESOURCE_NAME})
        target_register_resources(${TARGET} ${RESOURCE})
    endforeach()
endfunction()

function(target_link_resources TARGET RESOURCE_TARGET)
    target_get_resources(RESOURCE ${RESOURCE_TARGET})

    foreach(RESOURCE ${RESOURCE})
        string(REPLACE "/" "." RESOURCE_NAME ${RESOURCE})
        set(RESOURCE_NAME "${TARGET}.${RESOURCE_NAME}")

        target_get_resource_file(RESOURCE_INPUT_FILE ${RESOURCE_TARGET} ${RESOURCE})
        target_get_resource_file(RESOURCE_OUTPUT_FILE ${TARGET} ${RESOURCE})

        add_custom_target(${RESOURCE_NAME}
            DEPENDS ${RESOURCE_FILE}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${RESOURCE_INPUT_FILE}
                ${RESOURCE_OUTPUT_FILE}
            VERBATIM
        )
        add_dependencies(${RESOURCE_NAME} ${RESOURCE_TARGET})
        add_dependencies(${TARGET} ${RESOURCE_NAME})
        target_register_resources(${TARGET} ${RESOURCE})
    endforeach()
endfunction()
