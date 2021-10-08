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
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "tgafunc.h"

static void create_test(void);

static void load_test(void);

int main(int argc, char *argv[]) {
    create_test();
    load_test();

    puts("Test cases passed.");
    return 0;
}

static void create_test(void) {
    tga_image *image;
    enum tga_error error_code;
    int oversize = 65535 + 1;

    // image size cannot be less than 1.
    error_code = tga_create(&image, 0, 32, TGA_PIXEL_RGB24);
    assert(error_code == TGA_ERROR_INVALID_IMAGE_DIMENSISN);
    error_code = tga_create(&image, 32, 0, TGA_PIXEL_RGB24);
    assert(error_code == TGA_ERROR_INVALID_IMAGE_DIMENSISN);
    // Image size cannot be greater than 65535.
    error_code = tga_create(&image, oversize, 32, TGA_PIXEL_RGB24);
    assert(error_code == TGA_ERROR_INVALID_IMAGE_DIMENSISN);
    error_code = tga_create(&image, 32, oversize, TGA_PIXEL_RGB24);
    assert(error_code == TGA_ERROR_INVALID_IMAGE_DIMENSISN);
    // Wrong pixel format check.
    error_code = tga_create(&image, 32, 32, (enum tga_pixel_format)100);
    assert(error_code == TGA_ERROR_UNSUPPORTED_PIXEL_FORMAT);

    error_code = tga_create(&image, 128, 128, TGA_PIXEL_RGB24);
    assert(error_code == TGA_NO_ERROR);
    tga_free(image);
}

static void load_test(void) {
    int image_dimension = 128;

    char image_path[] = "images/";

    const char *image_name_list[] = {
        "CBW8.TGA", "CCM8.TGA", "CTC16.TGA", "CTC24.TGA", "CTC32.TGA",
        "UBW8.TGA", "UCM8.TGA", "UTC16.TGA", "UTC24.TGA", "UTC32.TGA"};

    enum tga_pixel_format pixel_formats[] = {
        TGA_PIXEL_BW8,    TGA_PIXEL_RGB555, TGA_PIXEL_RGB555, TGA_PIXEL_RGB24,
        TGA_PIXEL_ARGB32, TGA_PIXEL_BW8,    TGA_PIXEL_RGB555, TGA_PIXEL_RGB555,
        TGA_PIXEL_RGB24,  TGA_PIXEL_ARGB32};

    int image_count = sizeof(image_name_list) / sizeof(image_name_list[0]);
    char image_name[25];
    int loop;

    for (loop = 0; loop < image_count; ++loop) {
        // Create file name.
        memcpy(image_name, image_path, sizeof(image_path));
        strcat(image_name, image_name_list[loop]);

        // Load the image and check the file information.
        tga_image *image;
        enum tga_error error_code = tga_load(&image, image_name);
        assert(error_code == TGA_NO_ERROR);
        if (image->width != image_dimension ||
            image->height != image_dimension ||
            image->pixel_format != pixel_formats[loop]) {
            tga_free(image);
            break;
        }
        tga_free(image);
    }
    assert(loop == image_count);
}
