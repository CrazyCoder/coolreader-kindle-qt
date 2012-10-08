#ifdef USE_KINDLE_CURSOR

// qkindlecursor.cpp
//
//
// This file is licensed under the LGPL version 2.1, the text of which should
// be in the LICENSE.txt file, or alternately at this location:
// http://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
//
// DISCLAIMER: This software is released AS-IS with ABSOLUTELY NO WARRANTY OF
// ANY KIND.  The use of this software indicates your knowledge and acceptance
// of this.

#include <fcntl.h>
#include <asm/types.h>
typedef __u8 u8;
#include <linux/einkfb.h>
#include <sys/ioctl.h>
#include "qkindlefb.h"

#include "qkindlecursor.h"

/***
void QKindleCursor::set ( const QImage & image, int hotx, int hoty )
{
    QScreenCursor::set(image, hotx, hoty) ;
    if (visible)
    {
        hide() ;
        curw = image.width() ;
        curx = image.height() ;
        show() ;
    }
    else
    {
        curw = image.width() ;
        curx = image.height() ;
    }
}
***/
/***
void QKindleCursor::setBounds(int x0, int y0, int w, int h)
{
    if (visible)
    {
        hide() ;
        curx = x0 ;
        cury = y0 ;
        curw = w ;
        curx = h ;
        show() ;
    }
    else
    {
        curx = x0 ;
        cury = y0 ;
        curw = w ;
        curx = h ;
    }
}

QRect boundingRect () const
{
    return this->QScreenCursor::boundingRect() ;
}

QImage 	image () const
{
    return this->QScreenCursor::image() ;
}
***/
/////////////////////////////////////////////////////////////

QKindleCursor::QKindleCursor(int fd, unsigned char *bufp, int w, int h)
{
    enable = false;
    hwaccel = false;
    supportsAlpha = false;

    screen_fd = fd ;
    pfb = bufp ;
    dw = w ;
    dh = h ;

    curx = cury = 0 ;
    curw = 16 ; curh = 16;   // try how narrow and bold 'underline' will be looking...
    visible = 0 ;
}


void QKindleCursor::move(int x, int y)
{
    if (visible)
    {
        hide() ;
        curx = x -((x & 1)? 1 : 0)  ;
        cury = y ;
        show() ;
    }
    else
    {
        curx = x - ((x & 1)? 1 : 0) ;
        cury = y ;
    }
}

void QKindleCursor::hide()
{
    if (visible)
    {
        unsigned char *p = pfb ;
        unsigned char *pend ;
        unsigned char *ptmp ;
        int i ;
        int bperline = dw/2 ;

        ptmp = (unsigned char *) (new unsigned char[(curw/2) * curh]) ;

        update_area_t ua;

        ua.buffer = ptmp  ;

        ua.x1 = curx ;
        ua.y1 = cury ;
        ua.x2 = curx + curw + 1 ;
        ua.y2 = cury + curh + 1 ;
        ua.which_fx = fx_update_partial ;

        //
        p += (cury * bperline) + curx/2 ;

        for (i=0; i < curh; i++)
        {
            pend = p + (curw/2) ;
            while (p < pend)
            {
                *ptmp = ~(*p) ;
                ++p ;
                ++ptmp ;
            }
            p += (bperline - (curw/2)) ;
        }

        qDebug("hiding visible cursor @ %d %d %d %d", curx, cury, curw, curh) ;

        ioctl(screen_fbd, FBIO_EINK_UPDATE_DISPLAY_AREA, &ua);
        visible = false;
        delete ptmp ;
    }

    if (enable) {
        enable = false;

    }
}

void QKindleCursor::show()
{
    if (!enable) {
        enable = true;

        if (!visible)
        {
            unsigned char *p = pfb ;
            unsigned char *pend ;
            unsigned char *ptmp ;
            int i ;
            int bperline = dw/2 ;

            ptmp = (unsigned char *) (new unsigned char[(curw/2) * curh]) ;

            update_area_t ua;

            ua.buffer = ptmp  ;

            ua.x1 = curx ;
            ua.y1 = cury ;
            ua.x2 = curx + curw + 1 ;
            ua.y2 = cury + curh + 1 ;
            ua.which_fx = fx_update_partial ;

            //
            p += (cury * bperline) + curx/2 ;

            for (i=0; i < curh; i++)
            {
                pend = p + (curw/2) ;
                while (p < pend)
                {
                    *ptmp = ~(*p) ;
                    ++p ;
                    ++ptmp ;
                }
                p += (bperline - (curw/2)) ;
            }

            qDebug("showing invisible cursor @ %d %d %d %d", curx, cury, curw, curh) ;

            ioctl(screen_fd, FBIO_EINK_UPDATE_DISPLAY_AREA, &ua);
            visible = true;
        }
    }
}

void QKindleCursor::set(const QImage &image, int hotx, int hoty)
{
    QKindleFb *screen = 0 ;
    /***
    QKindleFb *screen = QKindleFb::instance();
    if (!screen)
    ***/    return;

    if (image.isNull()) {
        cursor = QImage();
        hide();
    } else {
        cursor = image.convertToFormat(screen->alphaPixmapFormat());
        size = cursor.size();
        hotspot = QPoint(hotx, hoty);
        show();
    }

}

#endif
