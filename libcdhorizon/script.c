#include <time.h>
//#include <luajit-2.1/luajit.h>
#include "script.h"
#include "priv.h"

struct luareader {
    rfun_t r;
    void *src;
    char buf[4096];
};

const char *luaread(lua_State *L, void *data, size_t *size);

void lua_purgeglobal(lua_State *L, const char *name);
void lua_getmember(lua_State *L, const char *table, const char *member);
void lua_purgemember(lua_State *L, const char *table, const char *member);
void lua_opensandbox(lua_State *L);

void horizon_ScriptClose(horizon_Script *restrict s) {
    if (s->L) {
        lua_close(s->L);
    }
}

int horizon_ScriptCompile(horizon_Script *restrict s, void *src, rfun_t rf) {
    return horizon_ScriptCompileCtx(NULL, s, src, rf);
}

int horizon_ScriptCompileCtx(horizon_ErrorCtx *ctx, horizon_Script *restrict s, void *src, rfun_t rf) {
    s->L = luaL_newstate();

    // open our sandbox env libs
    lua_opensandbox(s->L);

    // compile script
    struct luareader reader = {
        .r = rf,
        .src = src,
    };
    int ok = lua_load(s->L, luaread, &reader, "main");
    if (ok != 0) {
        if (ctx) {
            ctx->fn(ctx->data, lua_tostring(s->L, -1));
        }
        lua_close(s->L);
        s->L = NULL;
        return -1;
    }
    luaL_ref(s->L, LUA_REGISTRYINDEX);

    // create src image user data
    lua_pushlightuserdata(s->L, (void *)1);
    luaL_ref(s->L, LUA_REGISTRYINDEX);

    // create dst image user data
    lua_pushlightuserdata(s->L, (void *)1);
    luaL_ref(s->L, LUA_REGISTRYINDEX);

    // create src color user data
    lua_pushlightuserdata(s->L, (void *)1);
    luaL_ref(s->L, LUA_REGISTRYINDEX);

    // create dst color user data
    lua_pushlightuserdata(s->L, (void *)1);
    luaL_ref(s->L, LUA_REGISTRYINDEX);

    // create horizon table
    lua_createtable(s->L, 0, 4);

    // save horizon table
    luaL_ref(s->L, LUA_REGISTRYINDEX);
    lua_rawgeti(s->L, LUA_REGISTRYINDEX, horizon_ref);
    lua_setglobal(s->L, "horizon");

    return 0;
}

void lua_opensandbox(lua_State *L) {
    // open base lib
    luaopen_base(L);
    //lua_purgeglobal(L, "rawget");
    //lua_purgeglobal(L, "rawset");
    lua_purgeglobal(L, "setfenv");
    lua_purgeglobal(L, "getfenv");
    lua_purgeglobal(L, "loadfile");
    lua_purgeglobal(L, "loadstring");
    lua_purgeglobal(L, "load");
    lua_purgeglobal(L, "dofile");
    lua_purgeglobal(L, "collectgarbage");
    lua_purgeglobal(L, "gcinfo");
    lua_purgeglobal(L, "print");
    lua_purgeglobal(L, "_VERSION");
    lua_purgeglobal(L, "_G");

    // open bit
    luaopen_bit(L);

    // open table
    luaopen_table(L);

    // open math
    luaopen_math(L);
    lua_getmember(L, "math", "randomseed");
    lua_pushinteger(L, time(NULL));
    lua_call(L, 1, 0);
    lua_purgemember(L, "math", "randomseed");

    // set jit mode
    //luaJIT_setmode(L, 0, LUAJIT_MODE_ON);

    lua_settop(L, 0);
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

void lua_purgeglobal(lua_State *L, const char *name) {
    lua_pushnil(L);
    lua_setglobal(L, name);
}

void lua_getmember(lua_State *L, const char *table, const char *member) {
    lua_getglobal(L, table);
    lua_pushstring(L, member);
    lua_gettable(L, -2);
}

void lua_purgemember(lua_State *L, const char *table, const char *member) {
    lua_getglobal(L, table);
    lua_pushstring(L, member);
    lua_pushnil(L);
    lua_settable(L, -3);
}
