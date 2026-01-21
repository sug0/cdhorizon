# libcdhorizon

C library used to glitch images with the help of [libgoimg](https://github.com/sug0/libgoimg)
and [LuaJIT](https://bitop.luajit.org/).

## License

This software is licensed under the GNU Affero General Public License v3.0.
You may obtain a copy of the license text [here](https://www.gnu.org/licenses/agpl-3.0.txt).

## Driver program

A driver program is included to play around with the library.
The build script compiles both the library and the driver program.
You can obtain the driver program's usage (from the library's root path) with:

    $ ./driver/cdhorizon

The possible output format strings are:

* farbfeld
* PNG
* JPEG

## Dependencies

* libgoimg
* LuaJIT
* libjpeg (optional)
* libpng (optional)

## Building

Compile the library with:

    $ make libcdhorizon.a

And the driver with:

    $ make

To change the compilation flags, create a `config.mk` file with the
following content:

    CC=...
    CFLAGS=...
    LDFLAGS=...

## Lua API

| Identifier         | Arguments         | Return   | Description                                                           |
|--------------------|-------------------|----------|-----------------------------------------------------------------------|
| `horizon.getpixel` | `x`, `y`          | `pixel`  | Returns a table with the RGB color components of the specified pixel. |
| `horizon.setpixel` | `x`, `y`, `pixel` | N/A      | Takes an input table with the RGB components of the pixel to set.     |
| `horizon.width`    | N/A               | `width`  | Constant value of the width of the input image.                       |
| `horizon.height`   | N/A               | `height` | Constant value of the height of the input image.                      |
| `horizon.params`   | N/A               | `params` | Returns a dynamic table with user defined script param values.        |

Some examples can be found at `glitchers/`. The file `glitchers/id.lua` creates
an exact replica of the input image, so it is a fine template for new scripts.
