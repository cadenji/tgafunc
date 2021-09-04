// Copyright (c) 2021 Caden Ji
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <string.h>
#include "tgafunc.h"

static int image_dimension = 128;

static char image_path[] = "test/images/";

static const char *image_names[] =
{
    // "CBW8.TGA",      /* Not yet supported. */
    // "CCM8.TGA",      /* Not yet supported. */
    // "CTC16.TGA",     /* Not yet supported. */
    // "CTC24.TGA",     /* Not yet supported. */
    // "CTC32.TGA",     /* Not yet supported. */
    "UBW8.TGA",
    // "UCM8.TGA",      /* Not yet supported. */
    "UTC16.TGA",
    "UTC24.TGA",
    "UTC32.TGA"
};

static tga_pixel_format pixel_formats[] =
{
    // TGA_PIXEL_BW8,
    // TGA_PIXEL_RGB555,
    // TGA_PIXEL_RGB555,
    // TGA_PIXEL_RGB24,
    // TGA_PIXEL_ARGB32
    TGA_PIXEL_BW8,
    // TGA_PIXEL_RGB555,
    TGA_PIXEL_RGB555,
    TGA_PIXEL_RGB24,
    TGA_PIXEL_ARGB32
};

int main(int argc, char *argv[])
{
    char image[25];
    int image_count = sizeof(image_names) / sizeof(image_names[0]);
    int loop;

    for (loop = 0; loop < image_count; ++loop)
    {
        // Create file name.
        memcpy(image, image_path, sizeof(image_path));
        strcat(image, image_names[loop]);
        
        // Load the image and check the file information.
        tga_image *image_ptr = tga_load(image);
        if (image_ptr == NULL)
            break;
        if (image_ptr->width != image_dimension ||
            image_ptr->height != image_dimension ||
            image_ptr->pixel_format != pixel_formats[loop])
        {
            tga_free(image_ptr);
            break;
        }
        tga_free(image_ptr);
    }

    if (loop == image_count)
        puts("Load image test passed.");
    else
        puts("Load image test failed.");

    return 0;
}