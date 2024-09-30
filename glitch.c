#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>
#include "glitch.h"
#include "priv.h"

int lua_getpixel(lua_State *L);
int lua_setpixel(lua_State *L);

extern int horizon_Glitch(Image_t *dst, Image_t *src, horizon_Script *s) {
    return horizon_GlitchCtx(NULL, dst, src, s);
}

extern int horizon_GlitchCtx(horizon_ErrorCtx *ctx, Image_t *dst, Image_t *src, horizon_Script *s) {
    // allocate colors
    Color_t csrc = im_newcolor_from_img(src);
    Color_t cdst = im_newcolor_rgb();

    // set user data stuff
    lua_pushlightuserdata(s->L, &csrc);
    lua_rawseti(s->L, LUA_REGISTRYINDEX, srccolor_ref);

    lua_pushlightuserdata(s->L, &cdst);
    lua_rawseti(s->L, LUA_REGISTRYINDEX, dstcolor_ref);

    lua_pushlightuserdata(s->L, src);
    lua_rawseti(s->L, LUA_REGISTRYINDEX, srcimg_ref);

    lua_pushlightuserdata(s->L, dst);
    lua_rawseti(s->L, LUA_REGISTRYINDEX, dstimg_ref);

    // set constants
    lua_rawgeti(s->L, LUA_REGISTRYINDEX, horizon_ref);

    lua_pushstring(s->L, "getpixel");
    lua_pushcfunction(s->L, lua_getpixel);
    lua_settable(s->L, -3);

    lua_pushstring(s->L, "setpixel");
    lua_pushcfunction(s->L, lua_setpixel);
    lua_settable(s->L, -3);

    lua_pushstring(s->L, "width");
    lua_pushinteger(s->L, src->w);
    lua_settable(s->L, -3);

    lua_pushstring(s->L, "height");
    lua_pushinteger(s->L, src->h);
    lua_settable(s->L, -3);

    // call into lua
    lua_rawgeti(s->L, LUA_REGISTRYINDEX, main_ref);
    int error = lua_pcall(s->L, 0, 0, 0);

    if (error != 0 && ctx) {
        ctx->fn(ctx->data, lua_tostring(s->L, -1));
    }

    lua_settop(s->L, 0);
    im_xfree(im_std_allocator, csrc.color);
    im_xfree(im_std_allocator, cdst.color);

    return error != 0;
}

int lua_setpixel(lua_State *L) {
    if (lua_gettop(L) != 3) {
        return luaL_error(L, "expecting exactly 3 arguments");
    }

    // check types
    luaL_checktype(L, -1, LUA_TTABLE);

    if (lua_objlen(L, -1) != 3) {
        return luaL_error(L, "expecting table with 3 numbers as pixel");
    }

    const int y = luaL_checkinteger(L, -2);
    const int x = luaL_checkinteger(L, -3);

    lua_rawgeti(L, -1, 1);
    lua_rawgeti(L, -2, 2);
    lua_rawgeti(L, -3, 3);

    const int b = (int)(0xff & luaL_checkinteger(L, -1));
    const int g = (int)(0xff & luaL_checkinteger(L, -2));
    const int r = (int)(0xff & luaL_checkinteger(L, -3));

    // fetch image and color
    lua_rawgeti(L, LUA_REGISTRYINDEX, dstimg_ref);
    lua_rawgeti(L, LUA_REGISTRYINDEX, dstcolor_ref);

    Image_t *img = (Image_t *)lua_touserdata(L, -2);
    Color_t *dst = (Color_t *)lua_touserdata(L, -1);

    if (x < 0 || y < 0 || x >= img->w || y >= img->h) {
        return luaL_error(L, "coords out of bounds");
    }

    // set the color
    const int value = r | (g << 8) | (b << 16);
    RGB_t *const value_p = (RGB_t *const)&value;
    *(RGB_t *)dst->color = *value_p;
    img->set(img, x, y, dst);

    // pop off used stuff
    lua_settop(L, 0);

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
    lua_pop(L, 5);

    img->at(img, x, y, src);
    im_colormodel_rgb(dst, src);

    RGB_t *rgb = (RGB_t *)dst->color;

    lua_createtable(L, 3, 0);

    lua_pushinteger(L, rgb->r);
    lua_rawseti(L, -2, 1);

    lua_pushinteger(L, rgb->g);
    lua_rawseti(L, -2, 2);

    lua_pushinteger(L, rgb->b);
    lua_rawseti(L, -2, 3);

    return 1;
}
