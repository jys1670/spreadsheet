find_package(Java QUIET COMPONENTS Runtime)

if (NOT ANTLR_EXECUTABLE)
    find_program(ANTLR_EXECUTABLE
            NAMES antlr.jar antlr4.jar antlr-4.jar antlr-complete.jar)
endif ()

if (ANTLR_EXECUTABLE AND Java_JAVA_EXECUTABLE)
    execute_process(
            COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
            OUTPUT_VARIABLE ANTLR_COMMAND_OUTPUT
            ERROR_VARIABLE ANTLR_COMMAND_ERROR
            RESULT_VARIABLE ANTLR_COMMAND_RESULT
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    if (ANTLR_COMMAND_RESULT EQUAL 0)
        string(REGEX MATCH "Version [0-9]+(\\.[0-9])*" ANTLR_VERSION ${ANTLR_COMMAND_OUTPUT})
        string(REPLACE "Version " "" ANTLR_VERSION ${ANTLR_VERSION})
    else ()
        message(
                SEND_ERROR
                "Command '${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}' "
                "failed with the output '${ANTLR_COMMAND_ERROR}'")
    endif ()

    macro(ANTLR_TARGET Name InputFile)
        set(ANTLR_OPTIONS LEXER PARSER LISTENER VISITOR)
        set(ANTLR_ONE_VALUE_ARGS PACKAGE OUTPUT_DIRECTORY DEPENDS_ANTLR)
        set(ANTLR_MULTI_VALUE_ARGS COMPILE_FLAGS DEPENDS)
        cmake_parse_arguments(ANTLR_TARGET
                "${ANTLR_OPTIONS}"
                "${ANTLR_ONE_VALUE_ARGS}"
                "${ANTLR_MULTI_VALUE_ARGS}"
                ${ARGN})

        set(ANTLR_${Name}_INPUT ${InputFile})

        get_filename_component(ANTLR_INPUT ${InputFile} NAME_WE)

        set(ANTLR_${Name}_OUTPUT_DIR
                ${CMAKE_CURRENT_SOURCE_DIR}/antlr/${ANTLR_INPUT})

        unset(ANTLR_${Name}_CXX_OUTPUTS)

        message(STATUS "OUTPUT DIR ${ANTLR_${Name}_OUTPUT_DIR}")
        add_custom_target(
                antlr4-generate-files
                COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
                ${InputFile}
                -o ${ANTLR_${Name}_OUTPUT_DIR}/
                -Xexact-output-dir
                -Dlanguage=Cpp
                ${ANTLR_TARGET_COMPILE_FLAGS}
                DEPENDS ${InputFile}
                ${ANTLR_TARGET_DEPENDS}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating parser for target ${Name} with ANTLR ${ANTLR_VERSION}"
        )
    endmacro(ANTLR_TARGET)

endif (ANTLR_EXECUTABLE AND Java_JAVA_EXECUTABLE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
        ANTLR
        REQUIRED_VARS ANTLR_EXECUTABLE Java_JAVA_EXECUTABLE
        VERSION_VAR ANTLR_VERSION)
