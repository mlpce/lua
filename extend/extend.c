#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#ifdef _POSIX_SOURCE
#include <unistd.h>
#endif

#include "lua.h"
#include "extend/extend.h"
#include "include/slinput.h"

#ifndef SLI_CHAR_SIZE
#error SLI_CHAR_SIZE not defined
#endif

#if SLI_CHAR_SIZE > 1
#include <wchar.h>
#endif

static char M_key = 'k';

static int StateGC(struct lua_State *L) {
  SLINPUT_State **ud = lua_touserdata(L, 1);
  SLINPUT_State *state = *ud;
  if (state) {
    SLINPUT_DestroyState(state);
    *ud = NULL;
  }
  return 0;
}

void EXTEND_Init(struct lua_State *L) {
  sli_ushort num_columns = 0;
  sli_ushort cursor_margin = 5;

  /* Configure slinput from extend.slinput.columns and
  extend.slinput.cursor_margin if available. */
  const char *table_name = "extend";
  int type = lua_getglobal(L, table_name);
  SLINPUT_State **ud;
  int is_terminal;

  if (type == LUA_TTABLE) {
    lua_pushstring(L, "slinput");
    type = lua_gettable(L, -2);
    if (type == LUA_TTABLE) {
      int isnum = 0;
      lua_Integer num;
      lua_pushstring(L, "columns");
      lua_gettable(L, -2);
      num = lua_tointegerx(L, -1, &isnum);
      if (isnum && num >= 0 && num <= USHRT_MAX)
        num_columns = (sli_ushort) num;
      lua_pop(L, 1);  /* pop columns value */

      lua_pushstring(L, "cursor_margin");
      lua_gettable(L, -2);
      isnum = 0;
      num = lua_tointegerx(L, -1, &isnum);
      if (isnum && num >= 0 && num <= USHRT_MAX)
        cursor_margin = (sli_ushort) num;
      lua_pop(L, 1);  /* pop cursor_margin value */
    }
    lua_pop(L, 1); /* pop slinput value */
  }
  lua_pop(L, 1); /* pop extend value */

  /* Create a userdata to hold the input line state pointer */
  ud = lua_newuserdata(L, sizeof(SLINPUT_State *));
  /* Initialise the state pointer to null */
  *ud = NULL;

  /* Add a metatable to the userdata with a gc metamethod */
  lua_newtable(L);
  lua_pushcfunction(L, StateGC);
  lua_setfield(L, -2, "__gc");
  lua_setmetatable(L, -2);

  /* Add the userdata to the registry with key address */
  lua_rawsetp(L, LUA_REGISTRYINDEX, (void *) &M_key);

#ifdef _POSIX_SOURCE
  is_terminal = !!isatty(fileno(stdin));
#else
  is_terminal = 1;
#endif

  if (is_terminal && setlocale(LC_CTYPE, "") != NULL) {
    const SLINPUT_AllocInfo alloc_info = { NULL };
    /* Initialise the input line state and store the pointer in the
    userdata. */
    *ud = SLINPUT_CreateState(alloc_info, NULL, NULL);
    if (*ud) {
      SLINPUT_Set_NumColumns(*ud, num_columns);
      SLINPUT_Set_CursorMargin(*ud, cursor_margin);
      SLINPUT_Set_ContinuationCharacterLeft(*ud, '<');
      SLINPUT_Set_ContinuationCharacterRight(*ud, '>');
    }
  }
}

static SLINPUT_State *GetState(struct lua_State *L) {
  SLINPUT_State *state = NULL;
  if (LUA_TUSERDATA == lua_rawgetp(L, LUA_REGISTRYINDEX, (void *) &M_key)) {
    SLINPUT_State ** ud = lua_touserdata(L, -1);
    if (ud != NULL)
      state = *ud;
  }
  lua_pop(L, 1);

  return state;
}

static sli_char *CharToSLICHAR(const char *multibyte_string,
    size_t *num_slichars_out) {
  size_t num_slichars;
  sli_char *slichar_buffer;
#if SLI_CHAR_SIZE > 1
  const char *src_ptr = multibyte_string;
  mbstate_t mbs;
  memset(&mbs, 0, sizeof(mbs));
  num_slichars = mbsrtowcs(NULL, &src_ptr, 0, &mbs);
  if (num_slichars == (size_t) -1)
    return NULL;
#else
  num_slichars = strlen(multibyte_string);
#endif

  slichar_buffer = malloc(sizeof(sli_char)*(num_slichars + 1));
  if (!slichar_buffer)
    return NULL;

#if SLI_CHAR_SIZE > 1
  src_ptr = multibyte_string;
  memset(&mbs, 0, sizeof(mbs));
  if (num_slichars !=
      mbsrtowcs(slichar_buffer, &src_ptr, num_slichars, &mbs)) {
    free(slichar_buffer);
    return NULL;
  }
#else
  memcpy(slichar_buffer, multibyte_string, num_slichars);
#endif

  *num_slichars_out = num_slichars;
  slichar_buffer[num_slichars] = 0;
  return slichar_buffer;
}

static char *SLICHARToChar(const sli_char *slichar_string,
    size_t *num_mchars_out) {
  size_t num_mchars;
  char *multibyte_buffer;
#if SLI_CHAR_SIZE > 1
  const wchar_t *w_ptr = slichar_string;
  mbstate_t mbs;
  memset(&mbs, 0, sizeof(mbs));
  num_mchars = wcsrtombs(NULL, &w_ptr, 0, &mbs);
  if (num_mchars == (size_t) -1)
    return NULL;
#else
  num_mchars = strlen(slichar_string);
#endif

  multibyte_buffer = malloc(num_mchars + 1);
  if (!multibyte_buffer)
    return NULL;

#if SLI_CHAR_SIZE > 1
  w_ptr = slichar_string;
  memset(&mbs, 0, sizeof(mbs));
  if (num_mchars != wcsrtombs(multibyte_buffer, &w_ptr, num_mchars, &mbs)) {
    free(multibyte_buffer);
    return NULL;
  }
#else
  memcpy(multibyte_buffer, slichar_string, num_mchars);
#endif

  *num_mchars_out = num_mchars;
  multibyte_buffer[num_mchars] = 0;
  return multibyte_buffer;
}

static int SingleLineInput_Get(SLINPUT_State *state,
    const char *prompt, int buffer_size, char *buffer) {
  const sli_ushort buffer_size_ushort = (sli_ushort) buffer_size;
  size_t num_slichars = 0;
  int result = 0;
  sli_char *slichar_prompt;
  sli_char *slichar_buffer;
  int get_result;

  if (buffer_size < 1)
    return 0;

  slichar_prompt = CharToSLICHAR(prompt, &num_slichars);
  if (!slichar_prompt)
    return 0;

  slichar_buffer = malloc(sizeof(sli_char) * buffer_size_ushort);
  if (!slichar_buffer) {
    free(slichar_prompt);
    return 0;
  }

  slichar_buffer[0] = 0;
  get_result = SLINPUT_Get(state, slichar_prompt, NULL,
    buffer_size_ushort, slichar_buffer);

  if (get_result > 0) {
    size_t num_mchars = 0;
    char *multibyte_buffer = SLICHARToChar(slichar_buffer, &num_mchars);
    if (multibyte_buffer) {
      if (num_mchars < buffer_size_ushort) {
        memcpy(buffer, multibyte_buffer, num_mchars + 1);
        result = 1;
      }
      free(multibyte_buffer);
    }
  }

  free(slichar_prompt);
  free(slichar_buffer);
  return result;
}

int EXTEND_Get(struct lua_State *L, const char *prompt,
    int buffer_size, char *buffer) {
  SLINPUT_State *state = GetState(L);
  return state ? SingleLineInput_Get(state, prompt, buffer_size, buffer) :
      (fputs(prompt, stdout), fflush(stdout),
        fgets(buffer, buffer_size, stdin) != NULL);
}

void EXTEND_SaveLine(struct lua_State *L, const char *line) {
  SLINPUT_State *state = GetState(L);
  if (state) {
    size_t num_slichars = 0;
    sli_char *slichar_buffer = CharToSLICHAR(line, &num_slichars);
    if (slichar_buffer) {
      SLINPUT_Save(state, slichar_buffer);
      free(slichar_buffer);
    }
  }
}
