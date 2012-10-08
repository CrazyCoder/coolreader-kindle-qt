/*
* Copyright (C) 2010 Andy M. aka h1uke	h1ukeguy @ gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef _SCREENSHOT_H_
#define _SCREENSHOT_H_

#include <stdint.h>

/* Note: the magic number has been removed from the bmpfile_header structure
   since it causes alignment problems
     struct bmpfile_magic should be written/read first
   followed by the
     struct bmpfile_header
   [this avoids compiler-specific alignment pragmas etc.]
*/

struct bmpfile_magic {
    unsigned char magic[2];
};

struct bmpfile_header {
    uint32_t filesz;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;
};

typedef struct {
    uint32_t header_sz;
    int32_t width;
    int32_t height;
    uint16_t nplanes;
    uint16_t bitspp;
    uint32_t compress_type;
    uint32_t bmp_bytesz;
    int32_t hres;
    int32_t vres;
    uint32_t ncolors;
    uint32_t nimpcolors;
} BITMAPINFOHEADER;

typedef enum {
    BI_RGB = 0,
    BI_RLE8,
    BI_RLE4,
    BI_BITFIELDS, //Also Huffman 1D compression for BITMAPCOREHEADER2
    BI_JPEG,      //Also RLE-24 compression for BITMAPCOREHEADER2
    BI_PNG,
} bmp_compression_method_t;

struct rgba_color {
    unsigned char r ;
    unsigned char g ;
    unsigned char b ;
    unsigned char alpha ;
} ;

#define FRAMEBUFFER "/dev/fb/0"
#define BPP	4
#define NCOLORS	16
#define SCREENW 600
#define SCREENH	800

#define INCHPERMETER    39.3700787
// dots per meter for K3 and KDXG
#define K3DPM   ((uint32_t) (166 * INCHPERMETER))
#define KDXDPM   ((uint32_t) (150 * INCHPERMETER))

#define IMGSIZE ((SCREENW/(8/BPP)) * SCREENH)

#ifdef __cplusplus
extern "C" {
#endif

int do_screenshot(char *path) ;

#ifdef __cplusplus
}
#endif

#endif

