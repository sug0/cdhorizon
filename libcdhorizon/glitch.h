#ifndef LIBGLITCH_GLITCH_H
#define LIBGLITCH_GLITCH_H

#include <goimg/goimg.h>
#include "script.h"

typedef struct {
    void *data;
    void (*fn)(void *data, const char *error);
} horizon_ErrorCtx;

// Glitches a source image into a destiny image, with a given script.
extern int horizon_Glitch(Image_t *dst, Image_t *src, horizon_Script *script);

// Glitches a source image into a destiny image, with a given script.
// The given `ctx` is used to handle lua errors.
extern int horizon_GlitchCtx(horizon_ErrorCtx *ctx, Image_t *dst, Image_t *src, horizon_Script *s);

#endif
