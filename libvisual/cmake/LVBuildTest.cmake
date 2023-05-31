INCLUDE(CMakeParseArguments)

FUNCTION(LV_BUILD_TEST TEST_NAME)
  SET(OPTION_ARGS "")
  SET(SINGLE_ARGS "")
  SET(MULTI_ARGS  "ARGS" "SOURCES" "COMPILE_DEFS" "COMPILE_OPTIONS" "INCLUDE_DIRS" "LINK_DIRS" "LINK_LIBS" "LINK_OPTIONS")
  CMAKE_PARSE_ARGUMENTS(PARSED_ARGS "${OPTION_FLAGS}" "${SINGLE_ARGS}" "${MULTI_ARGS}" ${ARGN})

  IF(PARSED_ARGS_UNPARSED_ARGUMENTS)
    MESSAGE(FATAL_ERROR "Cannot build unit test '${TEST_NAME}', bad arguments: ${PARSED_ARGS_UNPARSED_ARGUMENTS}")
  ENDIF()

  IF(NOT PARSED_ARGS_SOURCES)
    MESSAGE(FATAL_ERROR "Cannot build unit test '${TEST_NAME}', no source files specified.")
  ENDIF()

  ADD_EXECUTABLE(${TEST_NAME} ${PARSED_ARGS_SOURCES})

  TARGET_COMPILE_DEFINITIONS(${TEST_NAME} PRIVATE _LV_BUILD_PRIVATE_TEST_FUNCS)

  IF(${PARSED_ARGS_COMPILE_DEFS})
    TARGET_COMPILE_DEFINITIONS(${TEST_NAME} PRIVATE ${PARSED_ARGS_COMPILE_DEFS})
  ENDIF()

  IF(${PARSED_ARGS_COMPILE_OPTIONS})
    TARGET_COMPILE_OPTIONS(${TEST_NAME} PRIVATE ${PARSED_ARGS_COMPILE_OPTIONS})
  ENDIF()

  TARGET_INCLUDE_DIRECTORIES(${TEST_NAME}
    PRIVATE
    ${PARSED_ARGS_INCLUDE_DIRS}
  )

  IF(${PARSED_ARGS_LINK_DIRS})
    TARGET_LINK_DIRECTORIES(${TEST_NAME} PRIVATE ${PARSED_ARGS_LINK_DIRS})
  ENDIF()

  TARGET_LINK_LIBRARIES(${TEST_NAME}
    PRIVATE
    libvisual
    Threads::Threads
    ${PARSED_ARGS_LINK_LIBS}
  )

  TARGET_LINK_OPTIONS(${TEST_NAME}
    PRIVATE
    ${PARSE_ARGS_LINK_OPTIONS}
  )

  ADD_TEST(NAME ${TEST_NAME}
    COMMAND ${TEST_NAME} ${PARSE_ARGS_ARGS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND_EXPAND_LISTS
  )
ENDFUNCTION()