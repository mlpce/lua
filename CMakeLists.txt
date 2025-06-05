cmake_minimum_required(VERSION 3.22)

project(lua VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 90)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" )
  # Using GCC
  set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -Wall -Wextra")

  if (ATARI_TOS_ENABLED)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer")
    endif()

    set(LIBCMINI_ENABLED OFF CACHE BOOL "Enable libcmini")
    if (LIBCMINI_ENABLED)
      # Using libcmini. A small footprint C standard library for the Atari ST.
      # libcmini is licensed under LGPL-2.1. See https://github.com/freemint/libcmini
      set(LIBCMINI_INCLUDE_PATH ${CMAKE_INSTALL_PREFIX}/libcmini/include CACHE STRING "Install path of libcmini headers")
      include_directories(BEFORE SYSTEM ${LIBCMINI_INCLUDE_PATH})
      if (ATARI_TOS_16BIT_INT)
        set(LIBCMINI_LIBRARY_PATH_SUBDIRECTORY "/mshort")
      endif()
      set(LIBCMINI_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/libcmini/lib${LIBCMINI_LIBRARY_PATH_SUBDIRECTORY} CACHE STRING "Install path of libcmini libraries")
      set(LIBCMINI_STARTUP_PATH ${CMAKE_INSTALL_PREFIX}/libcmini/startup CACHE STRING "Install path of libcmini startup")
      set(LIBCMINI_LIBRARIES gcc cmini gcc)

      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_LIBCMINI_ENABLED")
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
  # MLPCE_ENABLED activates LUA_USE_C89 and LUA_32BITS. It also enables some
  # changes to work around compiler issues.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_ENABLED")
endif()

set(MLPCE_REVISION_HEADER_ENABLED OFF CACHE BOOL "Enable revision header")
if (MLPCE_REVISION_HEADER_ENABLED)
  # Revision header contains defines indicating repositories and revisions
  # used to create the build. Adds a global function about() which displays
  # the repository and revision information for the build. The header must
  # be generated from without.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_REVISION_HEADER_ENABLED")
endif()

set(MLPCE_SLINPUT_ENABLED OFF CACHE BOOL "Enable SLINPUT")
if (MLPCE_SLINPUT_ENABLED)
  # Enable input and slinput, a single line input library used for the REPL.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_SLINPUT_ENABLED")
  set(MLPCE_INPUT_LIBRARY_NAME "input")
  set(SLINPUT_LIBRARY_NAME "slinput")
  # Add input
  add_subdirectory(input)
endif()

if (ATARI_TOS_ENABLED)
  # MLPCE_TOS_ENABLED activates specific package.path and directory separator
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_TOS_ENABLED")
  # When building for TOS, it is possible to enable TOSBINDL and the GEMDOS
  # binding.
  set(MLPCE_TOSBINDL_ENABLED OFF CACHE BOOL "Enable TOSBINDL")
  if (MLPCE_TOSBINDL_ENABLED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_TOSBINDL_ENABLED")
    set(TOSBINDL_LIBRARY_NAME "tosbindl")
    set(TBGEMDOS_LIBRARY_NAME "tbgemdos")
  endif()
endif()

# Lua parser. When enabled, parser, dump and undump are available.
# When disabled, only undump is available.
set(MLPCE_PARSER_ENABLED ON CACHE BOOL "Enable parser")
if (MLPCE_PARSER_ENABLED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_PARSER_ENABLED")
endif()

# Make iolib, oslib, mathlib, utf8lib and debuglib optional. This can save
# some executable size. Also when using libcmini iolib and oslib use some
# features which have not been implemented.

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

# Onelua - enable to use onelua.c 
set(MLPCE_ONELUA_ENABLED OFF CACHE BOOL "Enable onelua.c")

# Link directory for installed libs
link_directories("${CMAKE_INSTALL_PREFIX}/lib")

# Name of the generated Lua library
set(LUA_LIBRARY_NAME "lua54")

# Lua source files used when MLPCE_ONELUA_ENABLED=OFF
set(LUA_SRCS
  lapi.c
  lauxlib.c
  lbaselib.c
  lcode.c
  lcorolib.c
  lctype.c
  ldebug.c
  ldo.c
  lfunc.c
  lgc.c
  linit.c
  lmem.c
  loadlib.c
  lobject.c
  lopcodes.c
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

if (MLPCE_PARSER_ENABLED)
  # Add parser and dump implementations
  set(LUA_SRCS ${LUA_SRCS}
    ldump.c
    llex.c
    lparser.c
  )
else()
  # Add parser and dump stubs
  set(LUA_SRCS ${LUA_SRCS} noparser.c)
endif()

# Optional libs.
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

# Lua library target
if (MLPCE_ONELUA_ENABLED)
  # lua library using onelua.c
  add_library(
    ${LUA_LIBRARY_NAME}
    onelua.c
  )
  target_compile_options(${LUA_LIBRARY_NAME} PRIVATE -DMAKE_LIB)
else()
  # lua library using LUA_SRCS
  add_library(${LUA_LIBRARY_NAME} STATIC
    ${LUA_SRCS}
  )
endif()

# For TOS use .TTP executable suffix
set(EXECUTABLE_SUFFIX "")
if (ATARI_TOS_ENABLED)
  set(EXECUTABLE_SUFFIX ".ttp")
endif()

# Names for the executables
set(LUA_EXECUTABLE_NAME "lua${EXECUTABLE_SUFFIX}")
set(LUAC_EXECUTABLE_NAME "luac${EXECUTABLE_SUFFIX}")

# The libraries to link to lua
set(LUA_LINK_LIBRARIES
  ${MLPCE_INPUT_LIBRARY_NAME}
  ${SLINPUT_LIBRARY_NAME}
  ${TBGEMDOS_LIBRARY_NAME}
  ${TOSBINDL_LIBRARY_NAME}
  m${ATARI_TOS_VBCC_LIB_SUFFIX}
  ${LIBCMINI_LIBRARIES}
)

# The libraries to link to luac
set(LUAC_LINK_LIBRARIES
  m${ATARI_TOS_VBCC_LIB_SUFFIX}
  ${LIBCMINI_LIBRARIES}
)

if (MLPCE_ONELUA_ENABLED)
  # lua executable using onelua.c
  add_executable(
    ${LUA_EXECUTABLE_NAME}
    onelua.c
  )
  target_compile_options(${LUA_EXECUTABLE_NAME} PRIVATE -DMAKE_LUA)
  target_link_libraries(
    ${LUA_EXECUTABLE_NAME}
    ${LUA_LINK_LIBRARIES}
  )

  # luac executable using onelua.c
  add_executable(
    ${LUAC_EXECUTABLE_NAME}
    onelua.c
  )
  target_compile_options(${LUAC_EXECUTABLE_NAME} PRIVATE -DMAKE_LUAC)
  target_link_libraries(
    ${LUAC_EXECUTABLE_NAME}
    ${LUAC_LINK_LIBRARIES}
  )
else()
  # lua executable using lua.c and LUA_SRCS
  add_executable(
    ${LUA_EXECUTABLE_NAME}
    lua.c
  )
  target_link_libraries(
    ${LUA_EXECUTABLE_NAME}
    ${LUA_LIBRARY_NAME}
    ${LUA_LINK_LIBRARIES}
  )

  # luac executable using luac.c and LUA_SRCS
  add_executable(
    ${LUAC_EXECUTABLE_NAME}
    luac.c
  )
  target_link_libraries(
    ${LUAC_EXECUTABLE_NAME}
    ${LUA_LIBRARY_NAME}
    ${LUAC_LINK_LIBRARIES}
  )
endif()

set(
  LUA_PUBLIC_HEADERS
  "lua.h" "luaconf.h" "lualib.h" "lauxlib.h"
)

set_target_properties(
  ${LUA_LIBRARY_NAME}
  PROPERTIES PUBLIC_HEADER "${LUA_PUBLIC_HEADERS}"
)

install(
  TARGETS ${LUA_EXECUTABLE_NAME} ${LUAC_EXECUTABLE_NAME}
  RUNTIME DESTINATION bin
)

install(
  TARGETS ${LUA_LIBRARY_NAME}
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)
