#ifndef LIBGLITCH_GLITCH_H
#define LIBGLITCH_GLITCH_H

#include <goimg/goimg.h>
#include "script.h"

// Glitches a source image into a destiny image, with a given script.
extern int horizon_Glitch(Image_t *dst, Image_t *const src, horizon_Script *restrict script);

#endif
