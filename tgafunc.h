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

#ifndef TGAFUNC_H
#define TGAFUNC_H

#include <stdint.h>

// Pixel format used when creating a tga_image.
// Note that the pixel data are all in little-endian.
enum tga_pixel_format {
    // Single channel format represents grayscale, 8-bit integer.
    TGA_PIXEL_BW8,

    // Single channel format represents grayscale, 16-bit integer.
    TGA_PIXEL_BW16,

    // A 16-bit pixel format.
    // The topmost bit is assumed to an attribute bit, usually ignored.
    // Because of little-endian, this format is stored in GGGBBBBB ARRRRRGG.
    TGA_PIXEL_RGB555,

    // RGB color format, 8-bit per channel.
    TGA_PIXEL_RGB24,

    // RGB color with alpha format, 8-bit per channel.
    TGA_PIXEL_ARGB32
};

enum tga_error {
    TGA_NO_ERROR = 0,
    TGA_ERROR_OUT_OF_MEMORY,
    TGA_ERROR_FILE_CANNOT_READ,
    TGA_ERROR_FILE_CANNOT_WRITE,
    TGA_ERROR_NO_DATA,
    TGA_ERROR_UNSUPPORTED_COLOR_MAP_TYPE,
    TGA_ERROR_UNSUPPORTED_IMAGE_TYPE,
    TGA_ERROR_UNSUPPORTED_PIXEL_FORMAT,
    TGA_ERROR_INVALID_IMAGE_DIMENSISN
};

// DO NOT instantiate this object directly.
// Use tga_load() or tga_create() to create.
typedef struct tga_image_s {
    uint16_t width;
    uint16_t height;

    enum tga_pixel_format pixel_format;

    // Means the number of bytes per pixel,
    // this value is based on the pixel format.
    uint8_t bytes_per_pixel;

    uint8_t *data;
} tga_image;

// Create a new empty image.
enum tga_error tga_create(tga_image **image_out, int width, int height,
                          enum tga_pixel_format format);

// Load TGA image from file.
enum tga_error tga_load(tga_image **image_out, const char *file_name);

// Save TGA image to file.
enum tga_error tga_save(const tga_image *image, const char *file_name);

// Release the memory space previously allocated by tga_load() or tga_create().
// If image is a null pointer, the function does nothing.
void tga_free(tga_image *image);

// Returns pixel color at coordinates (x,y).
// The coordinates start at upper left corner.
// If the pixel coordinates are out of bounds (larger than width/height or
// small than 0), they will be clamped.
// This function only works on TGA_PIXEL_RGB24, TGA_PIXEL_ARGB32 pixel formats.
// For other formats SetPixel is ignored.
// If image_ptr is a null pointer, the function does nothing.
void tga_get_pixel(tga_image *image_ptr, int x, int y, uint8_t *red,
                   uint8_t *green, uint8_t *blue, uint8_t *alpha);

// Sets pixel color at coordinates (x,y).
// The coordinates start at upper left corner.
// If the pixel coordinates are out of bounds (larger than width/height or
// small than 0), they will be clamped.
// This function only works on TGA_PIXEL_RGB24, TGA_PIXEL_ARGB32 pixel formats.
// For other formats SetPixel is ignored.
// If image_ptr is a null pointer, the function does nothing.
void tga_set_pixel(tga_image *image_ptr, int x, int y, uint8_t red,
                   uint8_t green, uint8_t blue, uint8_t alpha);

// Flip the image horizontally.
// If image_ptr is a null pointer, the function does nothing.
void tga_image_flip_h(tga_image *image_ptr);

// Flip the image vertically.
// If image_ptr is a null pointer, the function does nothing.
void tga_image_flip_v(tga_image *image_ptr);

#endif
