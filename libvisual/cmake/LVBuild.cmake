# Build helper functions and macros

INCLUDE(CMakeParseArguments)

FUNCTION(LV_GENERATE_ORC_INLINE_SOURCE SOURCE TARGET)
  CMAKE_PARSE_ARGUMENTS(PARSED_ARGS "HEADER" "INIT_FUNCTION" "INCLUDES" ${ARGN})

  IF(PARSED_ARGS_UNPARSED_ARGUMENTS)
    MESSAGE(FATAL_ERROR "Cannot generate Orc function(s), bad arguments: ${PARSED_ARGS_UNPARSED_ARGUMENTS}")
  ENDIF()

  SET(ORCC_FLAGS "--inline")

  IF(PARSED_ARGS_HEADER)
    LIST(APPEND ORCC_FLAGS "--header")
  ENDIF()

  IF(PARSED_ARGS_INCLUDES)
    FOREACH(INCLUDE ${PARSED_ARGS_INCLUDES})
      LIST(APPEND ORCC_FLAGS "--include" "${INCLUDE}")
    ENDFOREACH()
  ENDIF()

  IF(PARSED_ARGS_INIT_FUNCTION)
    LIST(APPEND ORCC_FLAGS "--init-function" "${PARSED_ARGS_INIT_FUNCTION}")
  ENDIF()

  ADD_CUSTOM_COMMAND(
    OUTPUT  ${TARGET}
    COMMAND ${ORC_COMPILER_EXECUTABLE}
    ARGS    -o ${TARGET} ${SOURCE} ${ORCC_FLAGS}
    MAIN_DEPENDENCY ${SOURCE}
  )
ENDFUNCTION()
