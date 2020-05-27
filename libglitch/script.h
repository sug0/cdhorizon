#ifndef LIBGLITCH_SCRIPT_H
#define LIBGLITCH_SCRIPT_H

#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>
#include <goimg/goimg.h>

// Represents a new lua script to glitch an image with.
typedef struct horizon_Script {
    lua_State *L;
} horizon_Script;

// Compiles a new script from a reader.
extern int horizon_ScriptCompile(horizon_Script *restrict script, void *src, rfun_t rf);

// Deallocates a script.
extern void horizon_ScriptClose(horizon_Script *restrict script);

#endif
