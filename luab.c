/* MLPCE: A minimal script runner, intended to be used for a parserless build
that only supports running bytecode undumps. The script to run and its
arguments are passed on the command line. */

#include <stdlib.h>

#ifdef MLPCE_REVISION_HEADER_ENABLED
#include "include/revision.h"
#endif

#include "lprefix.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/*
** Check whether 'status' is not OK and, if so, prints the error
** message on the top of the stack.
*/
static int report (lua_State *L, int status) {
  if (status != LUA_OK) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL)
      msg = "(no err str)";
    lua_writestringerror("%s\n", msg);
    lua_pop(L, 1);  /* remove message */
  }
  return status;
}

/*
** Message handler used to run all chunks
*/
static int msghandler (lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  if (msg == NULL) {  /* is error object not a string? */
    if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
        lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
      return 1;  /* that is the message */
    else
      msg = lua_pushfstring(L, "(err obj is a %s)",
                               luaL_typename(L, 1));
  }
  luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
  return 1;  /* return the traceback */
}

static int docall (lua_State *L, int narg) {
  int status;
  const int base = lua_gettop(L) - narg;  /* function index */
  lua_pushcfunction(L, msghandler);  /* push message handler */
  lua_insert(L, base);  /* put it under function and args */

  status = lua_pcall(L, narg, 1, base);

  lua_remove(L, base);  /* remove message handler from the stack */
  return status;
}

static void createargtable (lua_State *L, char **argv, int argc) {
  int i, narg;
  narg = argc - 1;

  lua_createtable(L, narg, 0);
  for (i = 1; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i);
  }

  lua_setglobal(L, "arg");
}

/*
** Push on the stack the contents of table 'arg' from 1 to #arg
*/
static int pushargs (lua_State *L) {
  int i, n;
  if (lua_getglobal(L, "arg") != LUA_TTABLE)
    luaL_error(L, "'arg' not a table");
  n = (int)luaL_len(L, -1);

  luaL_checkstack(L, n, "too many args");
  for (i = 1; i <= n; i++)
    lua_rawgeti(L, -i, i);

  lua_remove(L, -i);  /* remove table from the stack */
  return n;
}

static int handle_script (lua_State *L, char **argv) {
  int status;
  const char *fname = argv[1];
  status = luaL_loadfile(L, fname);
  if (status == LUA_OK) {
    int n = pushargs(L);  /* push arguments to script */
    status = docall(L, n);
  }
  return report(L, status);
}

/*
** Main body of stand-alone interpreter (to be called in protected mode).
** Reads the options and handles them all.
*/
static int pmain (lua_State *L) {
  int argc = (int)lua_tointeger(L, 1);
  char **argv = (char **)lua_touserdata(L, 2);
#ifdef MLPCE_TOSBINDL_ENABLED
  char **envp = (char **)lua_touserdata(L, 3);
#endif
  luaL_checkversion(L);  /* check that interpreter has correct version */

#ifdef MLPCE_TOSBINDL_ENABLED
  /* Save the gemdos environment pointer in the registry */
  lua_pushlightuserdata(L, envp);
  lua_setfield(L, LUA_REGISTRYINDEX, "gemdos.envp");
#endif

  luaL_openlibs(L);  /* open standard libraries */
  createargtable(L, argv, argc);  /* create table 'arg' */

  lua_gc(L, LUA_GCRESTART);  /* start GC... */
  lua_gc(L, LUA_GCGEN, 0, 0);  /* ...in generational mode */

  if (handle_script(L, argv) != LUA_OK)
    return 0;  /* interrupt in case of error */

  lua_pushboolean(L, 1);  /* signal no errors */
  return 1;
}

#ifdef MLPCE_TOSBINDL_ENABLED
/* Third parameter to main is the gemdos environment */
int main (int argc, char **argv, char **envp) {
#else
int main (int argc, char **argv) {
#endif
  int status, result;
  lua_State *L;
  if (argc < 2) {
#ifdef MLPCE_REVISION_HEADER_ENABLED
    printf(MLPCE_BUILD_VERSION "/%d.\n", (int) (sizeof(int) * 8));
#else
    printf("arg(s) req.\n");
#endif
    return EXIT_FAILURE;
  }

  /* Create the Lua state */
  L = luaL_newstate();
  if (!L)
    return EXIT_FAILURE;

  lua_gc(L, LUA_GCSTOP);  /* stop GC while building state */
  lua_pushcfunction(L, &pmain);  /* to call 'pmain' in protected mode */

  lua_pushinteger(L, argc);  /* 1st argument */
  lua_pushlightuserdata(L, argv); /* 2nd argument */
#ifdef MLPCE_TOSBINDL_ENABLED
  lua_pushlightuserdata(L, envp); /* 3rd argument */
  status = lua_pcall(L, 3, 1, 0);  /* do the call */
#else
  status = lua_pcall(L, 2, 1, 0);  /* do the call */
#endif

  result = lua_toboolean(L, -1);  /* get result */
  report(L, status);
  lua_close(L);
  return (result && status == LUA_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}
