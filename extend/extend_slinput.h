#ifndef EXTEND_HEADER_INCLUDED
#define EXTEND_HEADER_INCLUDED

#include <stdint.h>

struct lua_State;
void EXTEND_Init(struct lua_State *L);
int EXTEND_Get(struct lua_State *L, const char *prompt,
  uint16_t buffer_len, char *buffer);
void EXTEND_SaveLine(struct lua_State *L, const char *line);

#endif
