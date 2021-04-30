# libcdhorizon

C library used to glitch images with the help of [libgoimg](https://github.com/sug0/libgoimg)
and [LuaJIT](https://bitop.luajit.org/).

## Driver program

A driver program is included to play around with the library.
The build script compiles both the library and the driver program.
You can obtain the driver program's usage (from the library's root path) with:

    $ ./driver/cdhorizon

## Dependencies

* libgoimg
* LuaJIT
* libjpeg (optional)
* libpng (optional)

## Building

Building this project requires [tup](http://gittup.org/tup/).
Compile the library with:

    $ ./make build

To change the compilation flags, create a `tup.config` file with the
following content:

    CONFIG_CC=...
    CONFIG_CFLAGS=...
    CONFIG_LDFLAGS=...

If you want libpng and/or libjpeg support, creating this file with the
appropriate `CONFIG_LDFLAGS` is required.

## Lua API

| Identifier         | Arguments         | Return   | Description                                                           |
|--------------------|-------------------|----------|-----------------------------------------------------------------------|
| `horizon.getpixel` | `x`, `y`          | `pixel`  | Returns a table with the RGB color components of the specified pixel. |
| `horizon.setpixel` | `x`, `y`, `pixel` | N/A      | Takes an input table with the RGB components of the pixel to set.     |
| `horizon.width`    | N/A               | `width`  | Constant value of the width of the input image.                       |
| `horizon.height`   | N/A               | `height` | Constant value of the height of the input image.                      |

Some examples can be found at `glitchers/`. The file `glitchers/id.lua` creates
an exact replica of the input image, so it is a fine template for new scripts.
