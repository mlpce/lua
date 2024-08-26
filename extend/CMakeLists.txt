if (NOT ATARI_TOS_VBCC_ENABLED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter -pedantic -Wconversion")
endif()

add_library(extend STATIC
  extend.c
)
