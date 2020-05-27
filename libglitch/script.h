#ifndef LIBGLITCH_SCRIPT_H
#define LIBGLITCH_SCRIPT_H

#include <goimg/goimg.h>

// Represents a new lua script to glitch an image with.
typedef struct horizon_Script horizon_Script;

// Compiles a new script from a reader.
extern int horizon_ScriptCompile(horizon_Script *restrict script, void *restrict src, rfun_t rf);

// Runs a script on the given images. This function shouldn't be called
// directly; consider using horizon_Glitch() instead.
extern int horizon_ScriptRun(Image_t *restrict dst, Image_t *const src, horizon_Script *restrict script);

#endif
