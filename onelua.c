/*
** Lua core, libraries, and interpreter in a single file.
** Compiling just this file generates a complete Lua stand-alone
** program:
**
** $ gcc -O2 -std=c99 -o lua onelua.c -lm
**
** or
**
** $ gcc -O2 -std=c89 -DLUA_USE_C89 -o lua onelua.c -lm
**
*/

/* default is to build the full interpreter */
#ifndef MAKE_LIB
#ifndef MAKE_LUAC
#ifndef MAKE_LUA
#define MAKE_LUA
#endif
#endif
#endif


/*
** Choose suitable platform-specific features. Default is no
** platform-specific features. Some of these options may need extra
** libraries such as -ldl -lreadline -lncurses
*/
#if 0
#define LUA_USE_LINUX
#define LUA_USE_MACOSX
#define LUA_USE_POSIX
#define LUA_ANSI
#endif


/* no need to change anything below this line ----------------------------- */

#include "lprefix.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#ifndef MLPCE_NOSIGNAL
#include <signal.h>
#endif
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* setup for luaconf.h */
#define LUA_CORE
#define LUA_LIB
#define ltable_c
#define lvm_c
#include "luaconf.h"

/* do not export internal symbols */
#undef LUAI_FUNC
#undef LUAI_DDEC
#undef LUAI_DDEF
#define LUAI_FUNC	static
#define LUAI_DDEC(def)	/* empty */
#define LUAI_DDEF	static

/* core -- used by all */
#include "lzio.c"
#include "lctype.c"
#include "lopcodes.c"
#include "lmem.c"
#include "lundump.c"
#ifndef MLPCE_PARSER_ENABLED
#include "noparser.c"
#endif
#ifdef MLPCE_PARSER_ENABLED
#include "ldump.c"
#endif
#include "lstate.c"
#include "lgc.c"
#ifdef MLPCE_PARSER_ENABLED
#include "llex.c"
#endif
#include "lcode.c"
#ifdef MLPCE_PARSER_ENABLED
#include "lparser.c"
#endif
#include "ldebug.c"
#include "lfunc.c"
#include "lobject.c"
#include "ltm.c"
#include "lstring.c"
#include "ltable.c"
#include "ldo.c"
#include "lvm.c"
#include "lapi.c"

/* auxiliary library -- used by all */
#include "lauxlib.c"

/* standard library  -- not used by luac */
#ifndef MAKE_LUAC
#include "lbaselib.c"
#include "lcorolib.c"
#ifdef MLPCE_DEBUGLIB_ENABLED
#include "ldblib.c"
#endif
#ifdef MLPCE_IOLIB_ENABLED
#include "liolib.c"
#endif
#ifdef MLPCE_MATHLIB_ENABLED
#include "lmathlib.c"
#endif
#include "loadlib.c"
#ifdef MLPCE_OSLIB_ENABLED
#include "loslib.c"
#endif
#include "lstrlib.c"
#include "ltablib.c"
#ifdef MLPCE_UTF8LIB_ENABLED
#include "lutf8lib.c"
#endif
#include "linit.c"
#endif

/* Minimal lua */
#ifdef MLPCE_MAKE_LUAB
#undef MAKE_LUA
#include "luab.c"
#endif

/* lua */
#ifdef MAKE_LUA
#include "lua.c"
#endif

/* luac */
#ifdef MAKE_LUAC
#include "luac.c"
#endif
