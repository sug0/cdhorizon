# Lua API

| Identifier         | Arguments         | Return   | Description                                                           |
|--------------------|-------------------|----------|-----------------------------------------------------------------------|
| `horizon.getpixel` | `x`, `y`          | `pixel`  | Returns a table with the RGB color components of the specified pixel. |
| `horizon.setpixel` | `x`, `y`, `pixel` | N/A      | Takes an input table with the RGB components of the pixel to set.     |
| `horizon.width`    | N/A               | `width`  | Constant value of the width of the input image.                       |
| `horizon.height`   | N/A               | `height` | Constant value of the height of the input image.                      |

Some examples can be found at `glitchers/`. The file `glitchers/id.lua` creates
an exact replica of the input image, so it is a fine template for new scripts.
