#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>
#include "script.h"

struct luareader {
    rfun_t r;
    void *src;
    char buf[4096];
};

struct horizon_Script {
    lua_State *L;
};

const char *luaread(lua_State *L, void *data, size_t *size);

int horizon_ScriptCompile(horizon_Script *restrict script, void *restrict src, rfun_t rf) {
    lua_State *L = luaL_newstate();
}

const char *luaread(lua_State *L, void *data, size_t *size) {
    struct luareader *reader = (struct luareader *)data;
    int n = reader->r(reader->src, reader->buf, sizeof(reader->buf));
    if (n <= 0) {
        *size = 0;
        return NULL;
    } else {
        *size = n;
        return reader->buf;
    }
}
