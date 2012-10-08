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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/fb.h>
#include <sys/ioctl.h>

#include "screenshot.h"

int do_screenshot(char *pdest)
{
    int rc = 0 ;
    int i ;
    int fd ;
    int fd_ss ;
    off_t offset ;

    int xres = SCREENW ;
    int yres = SCREENH ;
    int bpp = BPP ;
    int imgsize = IMGSIZE ;
    int bperrow = IMGSIZE / SCREENH ;

    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;


    const struct bmpfile_magic magichdr =  { { 'B', 'M'} } ;

    struct bmpfile_header file_h ;
    BITMAPINFOHEADER dib_h ;

    const struct rgba_color colortab[NCOLORS] = {
        { 0xff, 0xff, 0xff, 0 },
        { 0xee, 0xee, 0xee, 0 },
        { 0xdd, 0xdd, 0xdd, 0 },
        { 0xcc, 0xcc, 0xcc, 0 },
        { 0xbb, 0xbb, 0xbb, 0 },
        { 0xaa, 0xaa, 0xaa, 0 },
        { 0x99, 0x99, 0x99, 0 },
        { 0x88, 0x88, 0x88, 0 },
        { 0x77, 0x77, 0x77, 0 },
        { 0x66, 0x66, 0x66, 0 },
        { 0x55, 0x55, 0x55, 0 },
        { 0x44, 0x44, 0x44, 0 },
        { 0x33, 0x33, 0x33, 0 },
        { 0x22, 0x22, 0x22, 0 },
        { 0x11, 0x11, 0x11, 0 },
        { 0x00, 0x00, 0x00, 0 },
    } ;

    memset(&file_h, 0, sizeof(file_h)) ;
    file_h.bmp_offset = sizeof(magichdr) + sizeof(file_h) + sizeof(dib_h) + sizeof(colortab) ;
    file_h.filesz = file_h.bmp_offset + IMGSIZE ;



    memset(&dib_h, 0, sizeof(dib_h)) ;

    if ((fd = open(FRAMEBUFFER, O_RDONLY)) != -1)
    {
        if ((fd_ss = open(pdest, O_WRONLY| O_CREAT)) != -1)
        {
            memset(&vinfo, 0, sizeof(vinfo));
            memset(&finfo, 0, sizeof(finfo));

            ////ioctl(fd, FBIOGET_FSCREENINFO, &finfo) ;
            if ((ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)) != -1)
            {
                xres = vinfo.xres ;
                yres = vinfo.yres ;
                bpp =  vinfo.bits_per_pixel ;

                imgsize = ((xres/(8/bpp)) * yres) ;
                bperrow = imgsize / yres ;

                if (xres > SCREENW)
                {
                    dib_h.hres = KDXDPM ;
                    dib_h.vres = KDXDPM ;
                }
            }

            dib_h.header_sz = sizeof(BITMAPINFOHEADER) ;
            dib_h.width = xres ;
            dib_h.height = yres ;
            dib_h.nplanes = 1 ;
            dib_h.bitspp = bpp ;
            dib_h.compress_type = BI_RGB ;
            dib_h.bmp_bytesz = imgsize ;
            // KDX has different screen resolution than the default K3 ...
            dib_h.hres = ((xres > SCREENW) ? KDXDPM : K3DPM) ;
            dib_h.vres = dib_h.hres ;
            dib_h.ncolors = NCOLORS  ;
            dib_h.nimpcolors = 0 ;

            {
                unsigned char rowbuf[bperrow] ;

                write(fd_ss, &magichdr, sizeof(magichdr)) ;
                write(fd_ss, &file_h, sizeof(file_h)) ;
                write(fd_ss, &dib_h, sizeof(dib_h)) ;
                write(fd_ss, &colortab, sizeof(colortab)) ;

                for (i = 0 ; i < yres ; i++)
                {
                    offset = imgsize - (bperrow) * (i -1) ;
                    lseek(fd, offset, SEEK_SET) ;
                    read(fd, &rowbuf, bperrow) ;
                    write(fd_ss, &rowbuf, bperrow) ;
                }
            }

            close(fd_ss) ;
        }
        else
        {
            //fprintf(stderr, "Can't open %s\n",pdest) ;
            close(fd) ;
            rc = 1 ;
        }
    }
    else
    {
        //fprintf(stderr, "Can't open %s\n",FRAMEBUFFER) ;
        rc = 1 ;
    }

    return rc ;
}


