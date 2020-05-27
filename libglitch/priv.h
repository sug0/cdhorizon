#ifndef LIBGLITCH_PRIV_H
#define LIBGLITCH_PRIV_H

const int main_ref = 1;
const int srcimg_ref = 2;
const int srccolor_ref = 3;
const int dstcolor_ref = 4;
const int horizon_ref = 5;

struct horizon_Script {
    lua_State *L;
};

#endif
