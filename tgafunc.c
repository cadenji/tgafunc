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

#include "tgafunc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINT_ERROR(message) fprintf(stderr, "TGAFunc: %s\n", message)

#define MAX_WIDTH_OR_HEIGHT 65535

static tga_image *load_image(FILE *file_ptr);

static uint8_t *get_pixel_pointer(tga_image *image_ptr, int x, int y);

static int save_image(const tga_image *image_ptr, FILE *file_ptr);

tga_image *tga_create(int width, int height, tga_pixel_format format) {
    if (width <= 0 || width > MAX_WIDTH_OR_HEIGHT || height <= 0 ||
        height > MAX_WIDTH_OR_HEIGHT) {
        return NULL;
    }

    uint8_t pixel_size = 0;
    switch (format) {
        case TGA_PIXEL_BW8:
            pixel_size = 1;
            break;
        case TGA_PIXEL_BW16:
        case TGA_PIXEL_RGB555:
            pixel_size = 2;
            break;
        case TGA_PIXEL_RGB24:
            pixel_size = 3;
            break;
        case TGA_PIXEL_ARGB32:
            pixel_size = 4;
            break;
        default:
            return NULL;
    }

    tga_image *image = (tga_image *)malloc(sizeof(tga_image));
    if (image == NULL) return NULL;

    size_t image_data_size = (size_t)width * height * pixel_size;
    image->data = (uint8_t *)malloc(image_data_size);
    if (image->data == NULL) {
        free(image);
        return NULL;
    }

    memset(image->data, 0, image_data_size);

    image->width = (uint16_t)width;
    image->height = (uint16_t)height;
    image->pixel_format = format;
    image->bytes_per_pixel = pixel_size;

    return image;
}

tga_image *tga_load(const char *file_name) {
    FILE *file_ptr = fopen(file_name, "rb");
    if (file_ptr == NULL) {
        PRINT_ERROR("Cannot open file.");
        return NULL;
    }

    tga_image *image_ptr = load_image(file_ptr);
    fclose(file_ptr);
    return image_ptr;
}

int tga_save(const tga_image *image_ptr, const char *file_name) {
    if (image_ptr == NULL || image_ptr->data == NULL) {
        PRINT_ERROR("The image_ptr is a null pointer.");
        return 1;
    }

    // Check if a file with the same name already exists.
    FILE *file_ptr = fopen(file_name, "r");
    if (file_ptr != NULL) {
        PRINT_ERROR("A file with the same name already exists.");
        fclose(file_ptr);
        return 1;
    }

    file_ptr = fopen(file_name, "wb");
    if (file_ptr == NULL) {
        PRINT_ERROR("Cannot create file.");
        return 1;
    }

    int has_error = save_image(image_ptr, file_ptr);
    fclose(file_ptr);
    if (has_error) remove(file_name);

    return has_error;
}

void tga_free(tga_image *image_ptr) {
    if (image_ptr != NULL) {
        if (image_ptr->data != NULL) free(image_ptr->data);

        free(image_ptr);
    }
}

#define IS_OPERABLE_PIXEL_FORMAT(pixel_format) \
    (pixel_format == TGA_PIXEL_RGB24 || pixel_format == TGA_PIXEL_ARGB32)

void tga_get_pixel(tga_image *image_ptr, int x, int y, uint8_t *red,
                   uint8_t *green, uint8_t *blue, uint8_t *alpha) {
    if (image_ptr == NULL || image_ptr->data == NULL ||
        !IS_OPERABLE_PIXEL_FORMAT(image_ptr->pixel_format)) {
        return;
    }

    uint8_t *pixel_ptr = get_pixel_pointer(image_ptr, x, y);

    *blue = pixel_ptr[0];
    *green = pixel_ptr[1];
    *red = pixel_ptr[2];
    if (image_ptr->pixel_format == TGA_PIXEL_ARGB32)
        *alpha = pixel_ptr[3];
    else
        *alpha = 255;
}

void tga_set_pixel(tga_image *image_ptr, int x, int y, uint8_t red,
                   uint8_t green, uint8_t blue, uint8_t alpha) {
    if (image_ptr == NULL || image_ptr->data == NULL ||
        !IS_OPERABLE_PIXEL_FORMAT(image_ptr->pixel_format)) {
        return;
    }

    uint8_t *pixel_ptr = get_pixel_pointer(image_ptr, x, y);

    pixel_ptr[0] = blue;
    pixel_ptr[1] = green;
    pixel_ptr[2] = red;
    if (image_ptr->pixel_format == TGA_PIXEL_ARGB32) pixel_ptr[3] = alpha;
}

void tga_image_flip_h(tga_image *image_ptr) {
    if (image_ptr == NULL || image_ptr->data == NULL) return;
    // The maximum possible number of bytes for a pixel is 4.
    uint8_t temp[4];
    int flip_num = image_ptr->width / 2;

    for (int i = 0; i < flip_num; ++i) {
        for (int j = 0; j < image_ptr->height; ++j) {
            uint8_t *p1 = get_pixel_pointer(image_ptr, i, j);
            uint8_t *p2 =
                get_pixel_pointer(image_ptr, image_ptr->width - 1 - i, j);
            // Swap two pixels.
            memcpy(temp, p1, image_ptr->bytes_per_pixel);
            memcpy(p1, p2, image_ptr->bytes_per_pixel);
            memcpy(p2, temp, image_ptr->bytes_per_pixel);
        }
    }
}

void tga_image_flip_v(tga_image *image_ptr) {
    if (image_ptr == NULL || image_ptr->data == NULL) return;
    // The maximum possible number of bytes for a pixel is 4.
    uint8_t temp[4];
    int flip_num = image_ptr->height / 2;

    for (int i = 0; i < flip_num; ++i) {
        for (int j = 0; j < image_ptr->width; ++j) {
            uint8_t *p1 = get_pixel_pointer(image_ptr, j, i);
            uint8_t *p2 =
                get_pixel_pointer(image_ptr, j, image_ptr->height - 1 - i);
            // Swap two pixels.
            memcpy(temp, p1, image_ptr->bytes_per_pixel);
            memcpy(p1, p2, image_ptr->bytes_per_pixel);
            memcpy(p2, temp, image_ptr->bytes_per_pixel);
        }
    }
}

enum tga_image_type {
    TGA_TYPE_NO_DATA = 0,
    TGA_TYPE_COLOR_MAPPED = 1,
    TGA_TYPE_TRUE_COLOR = 2,
    TGA_TYPE_GRAYSCALE = 3,
    TGA_TYPE_RLE_COLOR_MAPPED = 9,
    TGA_TYPE_RLE_TRUE_COLOR = 10,
    TGA_TYPE_RLE_GRAYSCALE = 11
};

typedef struct tga_header_s {
    uint8_t id_length;
    uint8_t map_type;
    uint8_t image_type;

    // Color map specification.
    uint16_t map_first_entry;
    uint16_t map_length;
    uint8_t map_entry_size;

    // Image specification.
    uint16_t image_x_origin;
    uint16_t image_y_origin;
    uint16_t image_width;
    uint16_t image_height;
    uint8_t pixel_depth;
    uint8_t image_descriptor;
} tga_header;

typedef struct tga_color_map_s {
    uint16_t first_index;
    uint16_t entry_count;
    uint8_t bytes_per_entry;
    uint8_t *pixels;
} tga_color_map;

// Gets raw pixel data from tga_image for reading or writing.
// If the pixel coordinates are out of bounds (larger than width/height
// or small than 0), they will be clamped.
static uint8_t *get_pixel_pointer(tga_image *image_ptr, int x, int y) {
    if (x < 0)
        x = 0;
    else if (x >= image_ptr->width)
        x = image_ptr->width - 1;

    if (y < 0)
        y = 0;
    else if (y >= image_ptr->height)
        y = image_ptr->height - 1;

    return image_ptr->data +
           (y * image_ptr->width + x) * image_ptr->bytes_per_pixel;
}

static int has_read_file_error = 0;

// Read a 8-bit integer from the file stream.
static uint8_t read_uint8(FILE *file_ptr) {
    uint8_t value;
    if (fread(&value, 1, 1, file_ptr) != 1) {
        has_read_file_error = 1;
        return 0;
    }
    return value;
}

// Get a 16-bit little-endian integer from the file stream.
// It should works on both big-endian and little-endian
// architecture systems.
static uint16_t read_uint16_le(FILE *file_ptr) {
    uint8_t buffer[2];
    if (fread(&buffer, 1, 2, file_ptr) != 2) {
        has_read_file_error = 1;
        return 0;
    }
    return buffer[0] + (((uint16_t)buffer[1]) << 8);
}

#define IS_SUPPORTED_IMAGE_TYPE(header)                  \
    ((header).image_type == TGA_TYPE_COLOR_MAPPED ||     \
     (header).image_type == TGA_TYPE_TRUE_COLOR ||       \
     (header).image_type == TGA_TYPE_GRAYSCALE ||        \
     (header).image_type == TGA_TYPE_RLE_COLOR_MAPPED || \
     (header).image_type == TGA_TYPE_RLE_TRUE_COLOR ||   \
     (header).image_type == TGA_TYPE_RLE_GRAYSCALE)

#define IS_COLOR_MAPPED(header)                      \
    ((header).image_type == TGA_TYPE_COLOR_MAPPED || \
     (header).image_type == TGA_TYPE_RLE_COLOR_MAPPED)

#define IS_TRUE_COLOR(header)                      \
    ((header).image_type == TGA_TYPE_TRUE_COLOR || \
     (header).image_type == TGA_TYPE_RLE_TRUE_COLOR)

#define IS_GRAYSCALE(header)                      \
    ((header).image_type == TGA_TYPE_GRAYSCALE || \
     (header).image_type == TGA_TYPE_RLE_GRAYSCALE)

#define IS_RLE(header)                                   \
    ((header).image_type == TGA_TYPE_RLE_COLOR_MAPPED || \
     (header).image_type == TGA_TYPE_RLE_TRUE_COLOR ||   \
     (header).image_type == TGA_TYPE_RLE_GRAYSCALE)

// Calculate the pixel format according to the header.
// Return 0 if success, otherwise return nonzero value means
// the header is illegal.
//
// NOTE: if the supported values of map_entry_size and pixel_depth change.
// Will have a huge impact on decode_data(), decode_data_rle() and
// pixel_to_map_index() functions.
static int get_pixel_format(tga_pixel_format *format, tga_header *header_ptr) {
    if (IS_COLOR_MAPPED(*header_ptr)) {
        // If the supported pixel_depth is changed, remember to also change
        // the pixel_to_map_index() function.
        if (header_ptr->pixel_depth == 8) {
            switch (header_ptr->map_entry_size) {
                case 15:
                case 16:
                    *format = TGA_PIXEL_RGB555;
                    return 0;
                case 24:
                    *format = TGA_PIXEL_RGB24;
                    return 0;
                case 32:
                    *format = TGA_PIXEL_ARGB32;
                    return 0;
            }
        }
    } else if (IS_TRUE_COLOR(*header_ptr)) {
        switch (header_ptr->pixel_depth) {
            case 16:
                *format = TGA_PIXEL_RGB555;
                return 0;
            case 24:
                *format = TGA_PIXEL_RGB24;
                return 0;
            case 32:
                *format = TGA_PIXEL_ARGB32;
                return 0;
        }
    } else if (IS_GRAYSCALE(*header_ptr)) {
        switch (header_ptr->pixel_depth) {
            case 8:
                *format = TGA_PIXEL_BW8;
                return 0;
            case 16:
                *format = TGA_PIXEL_BW16;
                return 0;
        }
    }

    return 1;
}

// Load TGA header and pixel format from file stream.
// Return 0 if loaded successfully, nonzero value otherwise.
static int load_header_n_format(tga_header *header_ptr,
                                tga_pixel_format *pixel_format,
                                FILE *file_ptr) {
    has_read_file_error = 0;

    header_ptr->id_length = read_uint8(file_ptr);
    header_ptr->map_type = read_uint8(file_ptr);
    header_ptr->image_type = read_uint8(file_ptr);
    header_ptr->map_first_entry = read_uint16_le(file_ptr);
    header_ptr->map_length = read_uint16_le(file_ptr);
    header_ptr->map_entry_size = read_uint8(file_ptr);
    header_ptr->image_x_origin = read_uint16_le(file_ptr);
    header_ptr->image_y_origin = read_uint16_le(file_ptr);
    header_ptr->image_width = read_uint16_le(file_ptr);
    header_ptr->image_height = read_uint16_le(file_ptr);
    header_ptr->pixel_depth = read_uint8(file_ptr);
    header_ptr->image_descriptor = read_uint8(file_ptr);

    if (has_read_file_error) {
        PRINT_ERROR("Cannot load TGA header.");
        return 1;
    }

    if (header_ptr->map_type > 1) {
        PRINT_ERROR("Unsupported color map type.");
        return 1;
    }

    if (header_ptr->image_type == TGA_TYPE_NO_DATA) {
        PRINT_ERROR("No image data.");
        return 1;
    }

    if (!IS_SUPPORTED_IMAGE_TYPE(*header_ptr)) {
        PRINT_ERROR("Unsupported image type.");
        return 1;
    }

    // No need to check if the image size exceeds MAX_WIDTH_OR_HEIGHT.
    if (header_ptr->image_width <= 0 || header_ptr->image_height <= 0) {
        PRINT_ERROR("Image dimension error.");
        return 1;
    }

    if (get_pixel_format(pixel_format, header_ptr)) {
        PRINT_ERROR("Unsupported pixel format.");
        return 1;
    }

    return 0;
}

// Convert bits to integer bytes.
#define BITS_TO_BYTES(bit_count) (((bit_count)-1) / 8 + 1)

// Used for color mapped image decode.
static uint16_t pixel_to_map_index(uint8_t *pixel_ptr, uint8_t pixel_bytes) {
    // Because only 8-bit index is supported now, so implemented in this way.
    return pixel_ptr[0];
}

// Get the color of the specified index from the map.
// Return 0 if success, otherwise return nonzero value.
static int try_get_color_from_map(uint8_t *dest, uint16_t index,
                                  tga_color_map *map) {
    index -= map->first_index;
    if (index < 0 && index >= map->entry_count) return 1;

    memcpy(dest, map->pixels + map->bytes_per_entry * index,
           map->bytes_per_entry);
    return 0;
}

// Decode image data from file stream.
// 0​ upon success, nonzero value otherwise.
static int decode_data(tga_image *image_ptr, uint8_t pixel_bytes,
                       int is_color_mapped, tga_color_map *map,
                       FILE *file_ptr) {
    if (is_color_mapped) {
        uint8_t *image_data_ptr = image_ptr->data;
        size_t image_size = (size_t)image_ptr->width * image_ptr->height;
        for (; image_size > 0; --image_size) {
            if (fread(image_data_ptr, 1, pixel_bytes, file_ptr) != pixel_bytes)
                break;
            uint16_t index = pixel_to_map_index(image_data_ptr, pixel_bytes);
            if (try_get_color_from_map(image_data_ptr, index, map)) break;
            image_data_ptr += image_ptr->bytes_per_pixel;
        }

        if (image_size != 0) {
            PRINT_ERROR("Cannot load image data.");
            return 1;
        }
    } else {
        size_t data_size = (size_t)image_ptr->width * image_ptr->height *
                           image_ptr->bytes_per_pixel;
        if (fread(image_ptr->data, 1, data_size, file_ptr) != data_size) {
            PRINT_ERROR("Cannot load image data.");
            return 1;
        }
    }

    return 0;
}

// Decode image data with run-length encoding from file stream.
// 0​ upon success, nonzero value otherwise.
static int decode_data_rle(tga_image *image_ptr, uint8_t pixel_bytes,
                           int is_color_mapped, tga_color_map *map,
                           FILE *file_ptr) {
    uint8_t *image_data_ptr = image_ptr->data;
    size_t image_size = (size_t)image_ptr->width * image_ptr->height;
    uint8_t is_run_length_packet = 0;
    uint8_t packet_count = 0;
    // The buffer size is directly related to the implementation of
    // get_pixel_format() function.
    uint8_t pixel_buffer[4];

    for (; image_size > 0; --image_size) {
        if (packet_count == 0) {
            uint8_t repetition_count_field;
            if (fread(&repetition_count_field, 1, 1, file_ptr) != 1) break;
            is_run_length_packet = repetition_count_field & 0x80;
            packet_count = (repetition_count_field & 0x7F) + 1;
            if (is_run_length_packet) {
                if (fread(pixel_buffer, 1, pixel_bytes, file_ptr) !=
                    pixel_bytes)
                    break;
                if (is_color_mapped) {
                    uint16_t index =
                        pixel_to_map_index(pixel_buffer, pixel_bytes);
                    if (try_get_color_from_map(pixel_buffer, index, map)) break;
                }
            }
        }

        if (is_run_length_packet) {
            memcpy(image_data_ptr, pixel_buffer, image_ptr->bytes_per_pixel);
        } else {
            if (fread(image_data_ptr, 1, pixel_bytes, file_ptr) != pixel_bytes)
                break;
            if (is_color_mapped) {
                uint16_t index =
                    pixel_to_map_index(image_data_ptr, pixel_bytes);
                if (try_get_color_from_map(image_data_ptr, index, map)) {
                    break;
                }
            }
        }

        --packet_count;
        image_data_ptr += image_ptr->bytes_per_pixel;
    }

    if (image_size != 0) {
        PRINT_ERROR("Cannot load image data.");
        return 1;
    }
    return 0;
}

static tga_image *load_image(FILE *file_ptr) {
    tga_header header;
    tga_pixel_format pixel_format;
    if (load_header_n_format(&header, &pixel_format, file_ptr)) return NULL;

    // No need to handle the content of the ID field, so skip directly.
    if (fseek(file_ptr, header.id_length, SEEK_CUR)) {
        PRINT_ERROR("An error occurred while skipping the ID field.");
        return NULL;
    }

    int is_color_mapped = IS_COLOR_MAPPED(header);
    int is_rle = IS_RLE(header);

    // Handle color map field.
    tga_color_map color_map;
    color_map.pixels = NULL;
    size_t map_size = header.map_length * BITS_TO_BYTES(header.map_entry_size);
    if (is_color_mapped) {
        color_map.first_index = header.map_first_entry;
        color_map.entry_count = header.map_length;
        color_map.bytes_per_entry = BITS_TO_BYTES(header.map_entry_size);
        color_map.pixels = (uint8_t *)malloc(map_size);
        if (color_map.pixels == NULL) {
            PRINT_ERROR("Memory allocation error.");
            return NULL;
        }
        if (fread(color_map.pixels, 1, map_size, file_ptr) != map_size) {
            PRINT_ERROR("Cannot load color map data.");
            free(color_map.pixels);
            return NULL;
        }
    } else if (header.map_type == 1) {
        if (fseek(file_ptr, map_size, SEEK_CUR)) {
            PRINT_ERROR("An error occurred while skipping color map data.");
            return NULL;
        }
    }

    tga_image *image_ptr =
        tga_create(header.image_width, header.image_height, pixel_format);
    if (image_ptr == NULL) {
        PRINT_ERROR("Memory allocation error.");
        free(color_map.pixels);
        return NULL;
    }

    // Load image data.
    int has_error;
    if (is_rle)
        has_error =
            decode_data_rle(image_ptr, BITS_TO_BYTES(header.pixel_depth),
                            is_color_mapped, &color_map, file_ptr);
    else
        has_error = decode_data(image_ptr, BITS_TO_BYTES(header.pixel_depth),
                                is_color_mapped, &color_map, file_ptr);

    free(color_map.pixels);
    if (has_error) {
        tga_free(image_ptr);
        image_ptr = NULL;
    }

    // Flip the image if necessary, to keep the origin in upper left corner.
    int flip_h = header.image_descriptor & 0x10;
    int flip_v = !(header.image_descriptor & 0x20);
    if (flip_h) tga_image_flip_h(image_ptr);
    if (flip_v) tga_image_flip_v(image_ptr);

    return image_ptr;
}

#define HEADER_SIZE 18

static int save_image(const tga_image *image_ptr, FILE *file_ptr) {
    uint8_t header[HEADER_SIZE];
    memset(header, 0, HEADER_SIZE);
    if (image_ptr->pixel_format == TGA_PIXEL_BW8 ||
        image_ptr->pixel_format == TGA_PIXEL_BW16) {
        header[2] = (uint8_t)TGA_TYPE_GRAYSCALE;
    } else {
        header[2] = (uint8_t)TGA_TYPE_TRUE_COLOR;
    }
    header[12] = image_ptr->width & 0xFF;
    header[13] = (image_ptr->width >> 8) & 0xFF;
    header[14] = image_ptr->height & 0xFF;
    header[15] = (image_ptr->height >> 8) & 0xFF;
    header[16] = image_ptr->bytes_per_pixel * 8;
    if (image_ptr->pixel_format == TGA_PIXEL_ARGB32) {
        header[17] = 0x28;
    } else {
        header[17] = 0x20;
    }

    if (fwrite(header, 1, HEADER_SIZE, file_ptr) != HEADER_SIZE) {
        PRINT_ERROR("Cannot save TGA header.");
        return 1;
    }

    size_t data_size = (size_t)image_ptr->width * image_ptr->height *
                       image_ptr->bytes_per_pixel;
    if (fwrite(image_ptr->data, 1, data_size, file_ptr) != data_size) {
        PRINT_ERROR("Cannot save image data.");
        return 1;
    }

    return 0;
}