#ifndef LIBGLITCH_SCRIPT_H
#define LIBGLITCH_SCRIPT_H

#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>
#include <goimg/goimg.h>

// Represents a parameter for a script.
typedef struct horizon_Param {
    enum {
        HORIZON_PARAM_INT,
        HORIZON_PARAM_DOUBLE,
        HORIZON_PARAM_STRING,
    } kind;
    const char *key;
    union {
        int k_int;
        double k_double;
        char *k_string;
    } value;
} horizon_Param;

// Represents a list of parameters for a script.
typedef struct horizon_Params {
    size_t len;
    horizon_Param *list;
} horizon_Params;

// Represents a new lua script to glitch an image with.
typedef struct horizon_Script {
    lua_State *L;
} horizon_Script;

// Error handler for scripts.
typedef struct horizon_ErrorCtx {
    void *data;
    void (*fn)(void *data, const char *error);
} horizon_ErrorCtx;

// Compiles a new script from a reader.
extern int horizon_ScriptCompile(horizon_Script *restrict script, void *src, rfun_t rf);

// Compiles a new script from a reader. Errors are reported with `ctx`.
extern int horizon_ScriptCompileCtx(
    horizon_ErrorCtx *restrict ctx,
    horizon_Script *restrict script,
    void *src, rfun_t rf);

// Compiles a new script from a reader. Errors are reported with `ctx`.
// This version includes script parameters.
extern int horizon_ScriptCompileCtxParams(
    horizon_Params *restrict params,
    horizon_ErrorCtx *restrict ctx,
    horizon_Script *restrict script,
    void *src, rfun_t rf);

// Deallocates a script.
extern void horizon_ScriptClose(horizon_Script *restrict script);

#endif
