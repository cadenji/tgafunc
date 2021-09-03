// Copyright (c) 2021 Caden Ji
//
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file or at https://opensource.org/licenses/MIT.

#ifndef TGAFUNC_H
#define TGAFUNC_H

#include <stdint.h>

// Pixel format used when creating a tga_image.
// Note that the pixel data are all in little-endian.
typedef enum tga_pixel_format_e
{
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
} tga_pixel_format;

// DO NOT instantiate this object directly.
// Use tga_load() or tga_create() to create.
typedef struct tga_image_s
{
    uint16_t width;
    uint16_t height;

    tga_pixel_format pixel_format;

    // Means the number of bytes per pixel,
    // this value is based on the pixel format.
    uint8_t bytes_per_pixel;

    uint8_t *data;
} tga_image;

// Create a new empty image.
// Reture tga_image pointer if success, return NULL if allocation failure.
tga_image *tga_create(int width, int height, tga_pixel_format format);

// Load TGA image from file.
// Reture tga_image pointer if success, return NULL if read file fail or
// if the image is invalid.
tga_image *tga_load(const char *file_name);

// Save TGA image to file.
// Return 0 if success, otherwise return nonzero value.
int tga_save(const tga_image* image_ptr, const char *file_name);

// Release the memory space previously allocated by tga_load() or tga_create().
// If image_ptr is a null pointer, the function does nothing.
void tga_free(tga_image *image_ptr);

// Flip the image horizontally.
// If image_ptr is a null pointer, the function does nothing.
void tga_image_flip_h(tga_image *image_ptr);

// Flip the image vertically.
// If image_ptr is a null pointer, the function does nothing.
void tga_image_flip_v(tga_image *image_ptr);

#endif