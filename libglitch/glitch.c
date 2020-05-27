#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>
#include "glitch.h"
#include "priv.h"

int lua_getpixel(lua_State *L);

extern int horizon_Glitch(Image_t *dst, Image_t *const src, horizon_Script *restrict s) {
    int i, x, y, ok = 0;
    const char *message;

    // allocate colors
    Color_t csrc = im_newcolor_from_img(src);
    Color_t cdst = im_newcolor_rgb();

    // set user data stuff
    lua_pushlightuserdata(s->L, &csrc);
    lua_rawseti(s->L, LUA_REGISTRYINDEX, srccolor_ref);

    lua_pushlightuserdata(s->L, &cdst);
    lua_rawseti(s->L, LUA_REGISTRYINDEX, dstcolor_ref);

    // set constants
    lua_rawgeti(s->L, LUA_REGISTRYINDEX, horizon_ref);

    lua_pushstring(s->L, "getpixel");
    lua_pushcfunction(s->L, lua_getpixel);
    lua_settable(s->L, -3);

    lua_pushstring(s->L, "output");
    lua_createtable(s->L, 3, 0);
    lua_settable(s->L, -3);

    lua_pushstring(s->L, "ctx");
    lua_createtable(s->L, 0, 0);
    lua_settable(s->L, -3);

    lua_pushstring(s->L, "width");
    lua_pushinteger(s->L, 0);
    lua_settable(s->L, -3);

    lua_pushstring(s->L, "height");
    lua_pushinteger(s->L, 0);
    lua_settable(s->L, -3);

    uint8_t *output = (uint8_t *)cdst.color;

    for (y = 0; y < src->h; y++) {
        for (x = 0; x < src->w; x++) {
            // update coords
            lua_pushstring(s->L, "x");
            lua_pushinteger(s->L, x);
            lua_settable(s->L, -3);

            lua_pushstring(s->L, "y");
            lua_pushinteger(s->L, y);
            lua_settable(s->L, -3);

            // call into lua
            lua_rawgeti(s->L, LUA_REGISTRYINDEX, main_ref);
            ok = lua_pcall(s->L, 0, 0, 0);
            if (!ok) {
                goto done;
            }

            // update colors
            lua_pushstring(s->L, "output");
            lua_gettable(s->L, -2);

            if (!lua_istable(s->L, -1)) {
                message = "output is not a table";
                goto error;
            } else if (lua_objlen(s->L, -1) != 3) {
                message = "output table has to have size 3";
                goto error;
            }

            for (i = 1; i < 4; i++) {
                lua_rawgeti(s->L, -1, i);

                if (!lua_isnumber(s->L, -1)) {
                    message = "not a number";
                    goto error;
                }

                output[i-1] = 0xff & lua_tointeger(s->L, -1);
                dst->set(dst, x, y, &cdst);
            }
        }
    }

done:
    im_xfree(im_std_allocator, csrc.color);
    im_xfree(im_std_allocator, cdst.color);
    return ok;

error:
    im_xfree(im_std_allocator, csrc.color);
    im_xfree(im_std_allocator, cdst.color);
    return luaL_error(s->L, message);;
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
