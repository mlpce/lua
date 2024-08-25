#ifndef EXTEND_HEADER_INCLUDED
#define EXTEND_HEADER_INCLUDED

struct lua_State;
void EXTEND_Init(struct lua_State *L);
int EXTEND_Get(struct lua_State *L, const char *prompt,
  int buffer_len, char *buffer);
void EXTEND_SaveLine(struct lua_State *L, const char *line);

#endif
