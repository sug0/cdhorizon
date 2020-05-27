#ifndef LIBGLITCH_SCRIPT_H
#define LIBGLITCH_SCRIPT_H

#include <goimg/goimg.h>

// Represents a new lua script to glitch an image with.
typedef struct horizon_Script horizon_Script;

// Compiles a new script from a reader.
extern int horizon_ScriptCompile(horizon_Script *restrict script, void *restrict src, rfun_t rf);

#endif
