# TGAFunc

Lightweight library written in C for handling the Truevision TGA image format.

## How To Use

Just copy the `tgafunc.h` and `tgafunc.c` files to your porject and include the `tgafunc.h` in your code. Please make sure that your compiler supports the C99 standard because `<stdint.h>` is used.

### Usage Samples
```c
#include "tgafunc.h"

int main(int argc, char *argv[])
{
    const char *image_name = "image.tga";

    tga_image *image_ptr = tga_load(image_name);
    if (image_ptr != NULL)
    {
        // Do soming...

        tga_free(image_ptr);
    }

    return 0;
}

```

## Format Details

Offset | Length | Field Name | Description
---- | ------ | ---------- | ----------
0 | 1 byte | Image ID length | The number of bytes in the image ID field, range from 0 to 255.<br><br> 0 means that no image ID field is included.
1 | 1 byte | Color map type | 0 means no color map contains.<br><br> 1 means a color map is included.
2 | 1 byte | Image type | 0 means no image data included.<br><br> 1 means uncompressed color-mapped images. In this case the **Color map type** field must be 1.<br><br> 2 means uncompressed true-color image.<br><br> 3 means uncompressed grayscale image.<br><br> 9 means run-length encoded color-mapped images. In this case the **Color map type** field must be 1<br><br> 10 means run-length encoded true-color image.<br><br> 11 means run-length encoded grayscale image.
3 | 5 bytes | Color map specification | Describes the color map. Ignored if **Color map type** field is 0.<br><br> It has three subfields, see the [Color Map Specification](#colormapspec) table for details.
8 | 10 bytes | Image specification | Image dimensions and format.<br><br> It has six subfields, see the [Image Specification](#imagespec) table for details.
18 | varies | Image ID | Optional field containing identifying information. The length is specified by **Image ID Length** field. Not process the content of this field.
varies | varies | Color map data | If the **Color Map Type** is 0, this field doesn't exist.<br><br> Otherwise, get the entry information according to the **Color map specification** field. Each color map entry is 2, 3, or 4 bytes. Unused bits are assumed to specify attribute bits. For the color channel format represented by different entry byte lengths, see [Color Format](#colorformat).
varies | varies | Image data | If the **Image type** is an unmapped image, this field specifies (width) x (height) pixels. Each pixel specifies a color value, for the color channel format represented by different color byte lengths, see [Color Format](#colorformat).<br><br> If the **Image type** is a color-mapped image, this field specifies (width) x (height) color map indices. Each index is stored as an unsigned integral number of bytes (typically 1 or 2). Index values are stored in little-endian order.<br><br> If the **image type** value is 9, 10 and 11, the image data is processed according to the [Run-length Coding](#rle).

### <span id="colormapspec">Color Map Specification</span>

Offset | Length | Field Name | Description
---- | ------ | ---------- | ----------
3 | 2 bytes | First entry index | Integer (little-endian) index of first color map entry.
5 | 2 bytes | Color map length | Integer (little-endian) count of color map entries.
7 | 1 byte | Color map entry size | Number of bits in color map entry. 16 for the Targa 16, 24 for the Targa 24, 32 for the Targa 32.

### <span id="imagespec">Image Specification</span>

Offset | Length | Field Name | Description
---- | ------ | ---------- | ----------
8 | 2 bytes | X-origin | Integer (little-endian) X coordinate of the lower left corner of the image.<br><br> It will be ignored.
10 | 2 bytes | Y-origin | Integer (little-endian) Y coordinate of the lower left corner of the image.<br><br> It will be ignored.
12 | 2 bytes | Image width | Integer (little-endian) width of the image in pixels.
14 | 2 bytes | Image height | Integer (little-endian) height of the image in pixels.
16 | 1 byte | Pixel depth | Number of bits in a pixel. 16 for the Targa 16, 24 for the Targa 24, 32 for the Targa 32.
17 | 1 byte | Image descriptor | Bits 3-0: number of attribute bits associated with each pixel. For the Targa 16, this would be 0 or 1. For the Targa 24, it should be 0. For Targa 32, it should be 8.<br><br> Bits 5-4: screen origin. 0b00 means origin in lower left corner. 0b01 means origin in lower right corner. 0b10 means origin in upper left corner. 0b11 means origin in upper right corner.<br><br> Bits 7-6: Unused.

### <span id="colorformat">Color Format</span>

- The 2 bytes entry is broken down as follows: ARRRRRGG GGGBBBBB, where each letter represents a bit. But, because of the little-endian storage order, the first byte coming from the file will actually be GGGBBBBB, and the second will be ARRRRRGG. "A" represents an attribute bit.

- The 3 bytes entry contains 1 byte each of blue, green and red.

- The 4 bytes entry contains 1 byte each of blue, green, red and attribute.

### <span id="rle">Run-length Coding</span>

The run-length coding wraps the data in packets. There two types of packets: run-length packets and raw packets. Both types of packets consist of a 1 byte header, identifying the type of packet and specifying a count, followed by a variable-length body.

For the run length packet, the header is followed by a single data , which is assumed to be repeated the number of times specified in the header. For the raw packet, the header is followed by the number of data specified in the header. Data packets cannot cross the scan line.

#### Run-length Packets Header

Field Name | Packets type | Data count
---------- | ------------ | -----
**Field Length** | 1 bit | 7 bits
**Description** | Must be 1 for run-length packets | Repetition count minus 1. Since the maximum value of this field is 127, the largest possible run size would be 128.

#### Raw Packets Header

Field Name | Packets type | Data count
---------- | ------------ | -----
**Field Length** | 1 bit | 7 bits
**Description** | Must be 0 for raw packets | Number of data minus 1. Since the maximum value of this field is 127, there can never be more than 128 data per packet.


## License

Licensed under the [MIT](LICENSE) license.
