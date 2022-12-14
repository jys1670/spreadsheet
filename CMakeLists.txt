cmake_minimum_required(VERSION 3.24)

project(spreadsheet LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# ANTLR 
set(ANTLR_EXECUTABLE /usr/share/java/antlr-complete.jar)
set(ANLTR_LIBRARY /usr/lib/libantlr4-runtime.a)
include_directories(
        /usr/include/antlr4-runtime
)
include(${CMAKE_CURRENT_SOURCE_DIR}/antlr/FindANTLR.cmake)
add_definitions(
        -DANTLR4CPP_STATIC
        -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)
antlr_target(FormulaParser antlr/Formula.g4 LEXER PARSER LISTENER)
file(GLOB ANTLR_OUTPUT
        antlr/Formula/*.cpp
        antlr/Formula/*.h
        )

# Doxygen 
find_package(Doxygen)
if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doxygen
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs)
endif ()

add_executable(
        spreadsheet
        ${ANTLR_OUTPUT}
)
target_link_libraries(spreadsheet ${ANLTR_LIBRARY})

 add_executable(
         unit-tests
         ${ANTLR_OUTPUT}
         src/cell.cpp
         src/cell.h
         src/common.h
         src/formula.cpp
         src/formula.h
         src/FormulaAST.cpp
         src/FormulaAST.h
         src/sheet.cpp
         src/sheet.h
         src/structures.cpp
         tests/main.cpp
         tests/test_runner_p.h
 )
target_link_libraries(unit-tests ${ANLTR_LIBRARY})