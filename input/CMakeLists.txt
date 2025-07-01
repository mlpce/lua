if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" )
  # Using GCC
  set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -pedantic -Wconversion")
endif()

add_library(${MLPCE_INPUT_LIBRARY_NAME} STATIC
  input.c
)
