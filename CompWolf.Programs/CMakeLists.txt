cmake_minimum_required(VERSION 3.17)
project(CompWolf)

set(SUBPROJECTS
    "Core"
    "Graphics.Core"
    "Graphics.TestProgram"
)

foreach(SUBPROJECT ${SUBPROJECTS})
    add_subdirectory(${SUBPROJECT} ${PROJECT_NAME}.${SUBPROJECT})
endforeach()