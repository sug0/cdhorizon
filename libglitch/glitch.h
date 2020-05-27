#ifndef LIBGLITCH_GLITCH_H
#define LIBGLITCH_GLITCH_H

#include <goimg/goimg.h>

// Glitches a source image into a destiny image, with a given script.
extern void horizon_Glitch(Image_t *restrict dst, Image_t *const src, horizon_Script *retrict script);

#endif
