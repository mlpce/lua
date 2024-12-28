cmake_minimum_required(VERSION 3.22)

project(lua VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 90)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" )
  # Using GCC
  set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -Wall -Wextra")

  if (ATARI_TOS_ENABLED)
    set(LIBCMINI_ENABLED OFF CACHE BOOL "Enable libcmini")
    if (LIBCMINI_ENABLED)
      # Using libcmini
      set(LIBCMINI_INCLUDE_PATH ${CMAKE_INSTALL_PREFIX}/libcmini/include CACHE STRING "Install path of libcmini headers")
      include_directories(BEFORE SYSTEM ${LIBCMINI_INCLUDE_PATH})
      if (ATARI_TOS_16BIT_INT)
        set(LIBCMINI_LIBRARY_PATH_SUBDIRECTORY "/mshort")
      endif()
      set(LIBCMINI_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/libcmini/lib${LIBCMINI_LIBRARY_PATH_SUBDIRECTORY} CACHE STRING "Install path of libcmini libraries")
      set(LIBCMINI_STARTUP_PATH ${CMAKE_INSTALL_PREFIX}/libcmini/startup CACHE STRING "Install path of libcmini startup")
      set(LIBCMINI_LIBRARIES gcc cmini gcc)

      # libcmini doesn't support signal or locale so disable through defines
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Dl_signalT=char -DMLPCE_NOSIGNAL -D\"lua_getlocaledecpoint()=(\'.'\)\"")
      # linker flags for libcmini. Target also needs to link to LIBCMINI_LIBRARIES.
      set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib ${LIBCMINI_STARTUP_PATH}/crt0.o -L${LIBCMINI_LIBRARY_PATH}")
    endif()
  endif()
endif()

include_directories(".")
include_directories("${CMAKE_INSTALL_PREFIX}")

set(MLPCE_ENABLED OFF CACHE BOOL "Build mlpce changes")

if (MLPCE_ENABLED)
  # Enable input and slinput
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_ENABLED -DMLPCE_SLINPUT_ENABLED")
  set(MLPCE_INPUT_LIBRARY_NAME "input")
  set(SLINPUT_LIBRARY_NAME "slinput")
  # Add input
  add_subdirectory(input)
endif()

if (ATARI_TOS_ENABLED)
  set(MLPCE_TOSBINDL_ENABLED OFF CACHE BOOL "Enable TOSBINDL")
  if (MLPCE_TOSBINDL_ENABLED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_TOSBINDL_ENABLED")
    set(TOSBINDL_LIBRARY_NAME "tosbindl")
    set(TBGEMDOS_LIBRARY_NAME "tbgemdos")
  endif()
endif()

# Lua iolib
set(MLPCE_IOLIB_ENABLED ON CACHE BOOL "Enable iolib")
if (MLPCE_IOLIB_ENABLED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_IOLIB_ENABLED")
endif()

# Lua oslib
set(MLPCE_OSLIB_ENABLED ON CACHE BOOL "Enable oslib")
if (MLPCE_OSLIB_ENABLED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_OSLIB_ENABLED")
endif()

# Lua mathlib
set(MLPCE_MATHLIB_ENABLED ON CACHE BOOL "Enable mathlib")
if (MLPCE_MATHLIB_ENABLED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_MATHLIB_ENABLED")
endif()

# Lua utf8lib
set(MLPCE_UTF8LIB_ENABLED ON CACHE BOOL "Enable utf8lib")
if (MLPCE_UTF8LIB_ENABLED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_UTF8LIB_ENABLED")
endif()

# Lua debuglib
set(MLPCE_DEBUGLIB_ENABLED ON CACHE BOOL "Enable debuglib")
if (MLPCE_DEBUGLIB_ENABLED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_DEBUGLIB_ENABLED")
endif()

link_directories("${CMAKE_INSTALL_PREFIX}/lib")

set(LUA_LIBRARY_NAME "lua54")

set(LUA_SRCS
  lapi.c
  lauxlib.c
  lbaselib.c
  lcode.c
  lcorolib.c
  lctype.c
  ldebug.c
  ldo.c
  ldump.c
  lfunc.c
  lgc.c
  linit.c
  llex.c
  lmem.c
  loadlib.c
  lobject.c
  lopcodes.c
  lparser.c
  lstate.c
  lstring.c
  lstrlib.c
  ltable.c
  ltablib.c
  ltm.c
  lundump.c
  lvm.c
  lzio.c
)

if (MLPCE_IOLIB_ENABLED)
  set(LUA_SRCS ${LUA_SRCS} liolib.c)
endif()

if (MLPCE_OSLIB_ENABLED)
  set(LUA_SRCS ${LUA_SRCS} loslib.c)
endif()

if (MLPCE_MATHLIB_ENABLED)
  set(LUA_SRCS ${LUA_SRCS} lmathlib.c)
endif()

if (MLPCE_UTF8LIB_ENABLED)
  set(LUA_SRCS ${LUA_SRCS} lutf8lib.c)
endif()

if (MLPCE_DEBUGLIB_ENABLED)
  set(LUA_SRCS ${LUA_SRCS} ldblib.c)
endif()

add_library(${LUA_LIBRARY_NAME} STATIC
  ${LUA_SRCS}
)

if (ATARI_TOS_ENABLED)
set(LUA_EXECUTABLE_NAME "lua.ttp")
else()
set(LUA_EXECUTABLE_NAME "lua")
endif()

add_executable(
  ${LUA_EXECUTABLE_NAME}
  lua.c
)

target_link_libraries(
  ${LUA_EXECUTABLE_NAME}
  ${LUA_LIBRARY_NAME}
  ${MLPCE_INPUT_LIBRARY_NAME}
  ${SLINPUT_LIBRARY_NAME}
  ${TBGEMDOS_LIBRARY_NAME}
  ${TOSBINDL_LIBRARY_NAME}
  m${ATARI_TOS_VBCC_LIB_SUFFIX}
  ${LIBCMINI_LIBRARIES}
)

set(
  LUA_PUBLIC_HEADERS
  "lua.h" "luaconf.h" "lualib.h" "lauxlib.h"
)

set_target_properties(
  ${LUA_LIBRARY_NAME}
  PROPERTIES PUBLIC_HEADER "${LUA_PUBLIC_HEADERS}"
)

install(
  TARGETS ${LUA_EXECUTABLE_NAME}
  RUNTIME DESTINATION bin
)

install(
  TARGETS ${LUA_LIBRARY_NAME}
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)
