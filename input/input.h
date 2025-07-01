#ifndef INPUT_HEADER_INCLUDED
#define INPUT_HEADER_INCLUDED

struct lua_State;
void INPUT_Init(struct lua_State *L);
int INPUT_Get(struct lua_State *L, const char *prompt,
  int buffer_len, char *buffer);
void INPUT_SaveLine(struct lua_State *L, const char *line);

#endif
