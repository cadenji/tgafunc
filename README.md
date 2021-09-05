# TGAFunc

Lightweight library written in C for handling the Truevision TGA image format.

For more information about the TGA format, please refer to the
[specification](http://www.dca.fee.unicamp.br/~martino/disciplinas/ea978/tgaffs.pdf).

## Usage

Copy the `tgafunc.h` and `tgafunc.c` files to your porject and include the
`tgafunc.h` in your code. Please make sure that your compiler compliant with
the C99 standard or newer.

### Load an image from a file and get the pixel color:
```c
#include <stdio.h>
#include "tgafunc.h"

int main(int argc, char *argv[])
{
    const char *image_name = "image.tga";

    tga_image *image_ptr = tga_load(image_name);
    if (image_ptr != NULL)
    {
        // Get the color of the pixel with coordinates (0, 0).
        uint8_t r, g, b, a;
        tga_get_pixel(image_ptr, 0, 0, &r, &g, &b, &a);

        tga_free(image_ptr);
    }

    return 0;
}

```

### Create an image, set the pixel color and save it:
```c
#include <stdio.h>
#include "tgafunc.h"

int main(int argc, char *argv[])
{
    int width = 32;
    int height = 32;

    tga_image *image_ptr = tga_create(width, height, TGA_PIXEL_RGB24);
    if (image_ptr != NULL)
    {
        tga_set_pixel(image_ptr, 0, 0, 255, 255, 255, 255);
        tga_save(image_ptr, "new_image.tga");
        tga_free(image_ptr);
    }

    return 0;
}

```

## License

Licensed under the [MIT](LICENSE) license.
