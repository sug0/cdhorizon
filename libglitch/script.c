#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>
#include "script.h"

const int main_ref = 1;
const int srcimg_ref = 2;
const int srccolor_ref = 3;
const int dstcolor_ref = 4;
const int horizon_ref = 5;

struct luareader {
    rfun_t r;
    void *src;
    char buf[4096];
};

struct horizon_Script {
    lua_State *L;
};

const char *luaread(lua_State *L, void *data, size_t *size);

void lua_purgeglobal(lua_State *L, const char *name);
void lua_getmember(lua_State *L, const char *table, const char *member);
void lua_purgemember(lua_State *L, const char *table, const char *member);
void lua_opensandbox(lua_State *L);
int lua_getpixel(lua_State *L);

void horizon_ScriptClose(horizon_Script *restrict s) {
    lua_close(s.L);
}

int horizon_ScriptCompile(horizon_Script *restrict s, void *src, rfun_t rf) {
    s.L = luaL_newstate();

    // open our sandbox env libs
    lua_opensandbox(s.L);

    // compile script
    struct luareader reader = {
        .r = rf,
        .src = src,
    };
    int ok = lua_load(s.L, luaread, &reader, "main");
    if (ok != 0) {
        lua_close(s.L);
        return -1;
    }
    luaL_ref(s.L, LUA_REGISTRYINDEX);

    // create src image user data
    lua_pushlightuserdata(s.L, (void *)1);
    luaL_ref(s.L, LUA_REGISTRYINDEX);

    // create src color user data
    lua_pushlightuserdata(s.L, (void *)1);
    luaL_ref(s.L, LUA_REGISTRYINDEX);

    // create dst color user data
    lua_pushlightuserdata(s.L, (void *)1);
    luaL_ref(s.L, LUA_REGISTRYINDEX);

    // create horizon table
    lua_createtable(s.L, 0, 7);

    lua_pushstring(s.L, "getpixel");
    lua_pushcfunction(L, lua_getpixel);
    lua_settable(s.L, -3);

    lua_pushstring(s.L, "output");
    lua_createtable(s.L, 3, 0);
    lua_settable(s.L, -3);

    lua_pushstring(s.L, "ctx");
    lua_createtable(s.L, 0, 0);
    lua_settable(s.L, -3);

    lua_pushstring(s.L, "width");
    lua_pushinteger(s.L, 0);
    lua_settable(s.L, -3);

    lua_pushstring(s.L, "height");
    lua_pushinteger(s.L, 0);
    lua_settable(s.L, -3);

    lua_pushstring(s.L, "x");
    lua_pushinteger(s.L, 0);
    lua_settable(s.L, -3);

    lua_pushstring(s.L, "y");
    lua_pushinteger(s.L, 0);
    lua_settable(s.L, -3);

    // save horizon table
    luaL_ref(s.L, LUA_REGISTRYINDEX);
    lua_rawgeti(s.L, LUA_REGISTRYINDEX, s.horizon_ref);
    lua_setglobal(s.L, "horizon");

    return 0;
}

int lua_getpixel(lua_State *L) {
    if (lua_gettop(L) != 2) {
        return luaL_error(L, "expecting exactly 2 arguments");
    }

    const int y = luaL_checkinteger(L, -1);
    const int x = luaL_checkinteger(L, -2);

    lua_rawgeti(L, LUA_REGISTRYINDEX, srcimg_ref);
    Image_t *const img = (Image_t *const)lua_touserdata(L, -1);

    if (x < 0 || y < 0 || x >= img->w || y >= img->h) {
        return luaL_error(L, "coords out of bounds");
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, srccolor_ref);
    lua_rawgeti(L, LUA_REGISTRYINDEX, dstcolor_ref);

    Color_t *src = (Color_t *)lua_touserdata(L, -2);
    Color_t *dst = (Color_t *)lua_touserdata(L, -1);

    img->at(img, x, y, src);
    im_colormodel_rgb(dst, src);

    RGB_t *rgb = (RGB_t *)dst.col;

    lua_createtable(L, 3, 0);

    lua_pushinteger(L, rgb->r);
    lua_rawseti(L, -2, 1);

    lua_pushinteger(L, rgb->g);
    lua_rawseti(L, -2, 2);

    lua_pushinteger(L, rgb->b);
    lua_rawseti(L, -2, 3);

    return 1;
}

void lua_opensandbox(lua_State *L) {
    // open base lib
    luaopen_base(L);
    lua_purgeglobal(L, "rawget");
    lua_purgeglobal(L, "rawset");
    lua_purgeglobal(L, "setfenv");
    lua_purgeglobal(L, "getfenv");
    lua_purgeglobal(L, "loadfile");
    lua_purgeglobal(L, "loadstring");
    lua_purgeglobal(L, "load");
    lua_purgeglobal(L, "dofile");
    lua_purgeglobal(L, "collectgarbage");
    lua_purgeglobal(L, "gcinfo");
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
