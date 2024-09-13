cmake_minimum_required(VERSION 3.22)

project(lua VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 90)

include_directories(".")
include_directories("${CMAKE_INSTALL_PREFIX}")

set(MLPCE_EXTEND_ENABLED OFF CACHE BOOL "Build extend")

if (MLPCE_EXTEND_ENABLED)
  # Enable extend and slinput
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLPCE_EXTEND -DMLPCE_EXTEND_SLINPUT")
  set(EXTEND_LIBRARY_NAME "extend")
  set(SLINPUT_LIBRARY_NAME "slinput")
  # Add extend library
  add_subdirectory(extend)
endif()

include_directories("${CMAKE_INSTALL_PREFIX}")
link_directories("${CMAKE_INSTALL_PREFIX}/lib")

set(LUA_LIBRARY_NAME "lua54")

add_library(${LUA_LIBRARY_NAME} STATIC
  lapi.c
  lauxlib.c
  lbaselib.c
  lcode.c
  lcorolib.c
  lctype.c
  ldblib.c
  ldebug.c
  ldo.c
  ldump.c
  lfunc.c
  lgc.c
  linit.c
  liolib.c
  llex.c
  lmathlib.c
  lmem.c
  loadlib.c
  lobject.c
  lopcodes.c
  loslib.c
  lparser.c
  lstate.c
  lstring.c
  lstrlib.c
  ltable.c
  ltablib.c
  ltm.c
  lundump.c
  lutf8lib.c
  lvm.c
  lzio.c
)

if (ATARI_TOS_VBCC_ENABLED)
set(LUA_EXECUTABLE_NAME "lua.ttp")
else()
set(LUA_EXECUTABLE_NAME "lua")
endif()

add_executable(${LUA_EXECUTABLE_NAME} lua.c)
target_link_libraries(${LUA_EXECUTABLE_NAME} ${LUA_LIBRARY_NAME} ${EXTEND_LIBRARY_NAME} ${SLINPUT_LIBRARY_NAME} m)

set(LUA_PUBLIC_HEADERS "lua.h" "luaconf.h" "lualib.h" "lauxlib.h")

set_target_properties(${LUA_LIBRARY_NAME} PROPERTIES PUBLIC_HEADER "${LUA_PUBLIC_HEADERS}")

install(TARGETS ${LUA_EXECUTABLE_NAME} RUNTIME DESTINATION bin)
install(TARGETS ${LUA_LIBRARY_NAME} ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include)
