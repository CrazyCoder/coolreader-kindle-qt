/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qkindlefb.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/kd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <signal.h>


// just to make the compiler happy
#include <asm/types.h>
typedef __u8 u8;

#include "linux/fb.h"
#include "linux/einkfb.h"
#include "linux/mxcfb.h"

#include "qkindlecursor.h"

#include <QDebug>

#include <QImage>
#include <QStringList>
#include <QApplication>
#include <QColor>


QT_BEGIN_NAMESPACE

extern int qws_client_id;

class QKindleFbPrivate : public QObject
{
public:
    QKindleFbPrivate();
    ~QKindleFbPrivate();

    int fd;
    int startupw;
    int startuph;
    int startupd;
    bool blank;
    QKindleFb::DriverTypes driverType;

    bool doGraphicsMode;
    int ttyfd;
    long oldKdMode;
    QString ttyDevice;

    QString displaySpec;


    QImage::Format alphaPixmapFormat;
    static QKindleFb *instance;

};

QKindleFb * QKindleFbPrivate::instance = 0 ;

#define DEBUG_OUTPUT

QKindleFbPrivate::QKindleFbPrivate()
    : fd(-1), blank(true), doGraphicsMode(false),
      ttyfd(-1), oldKdMode(KD_TEXT)
{
    /****QWSSignalHandler::instance()->addObject(this);****/
}
QKindleFbPrivate::~QKindleFbPrivate()
{

}


QKindleFb *QKindleFb::instance()
{
    return QKindleFbPrivate::instance;
}




QImage::Format QKindleFb::alphaPixmapFormat() const
{
    return d_ptr->alphaPixmapFormat ;
}

/*!
    \fn QKindleFb::QKindleFb(int displayId)

    Constructs a QKindleFb object. The \a displayId argument
    identifies the Qt for Embedded Linux server to connect to.
*/

QKindleFb::QKindleFb(int display_id)
    : QScreen(display_id, LinuxFBClass), d_ptr(new QKindleFbPrivate)
{
    canaccel=false;
    QKindleFbPrivate::instance = this;

}

/*!
    Destroys this QKindleFb object.
*/

QKindleFb::~QKindleFb()
{
    /*
    if (QKindleFbPrivate::instance == this)
        QKindleFbPrivate::instance = 0 ;
*/
}


/*!
    \reimp

    This is called by \l{Qt for Embedded Linux} clients to map in the framebuffer.
    It should be reimplemented by accelerated drivers to map in
    graphics card registers; those drivers should then call this
    function in order to set up offscreen memory management. The
    device is specified in \a displaySpec; e.g. "/dev/fb".

    \sa disconnect()
*/

//#define DEBUG_VINFO 1

#ifdef DEBUG_VINFO
static void show_info(struct fb_fix_screeninfo *pfinfo, struct fb_var_screeninfo *pvinfo)
{
    struct fb_var_screeninfo vinfo = *pvinfo;
    struct fb_fix_screeninfo finfo = *pfinfo;

    qDebug("finfo: ") ;
    qDebug("   id=%s", (char *)&finfo.id) ;
    qDebug("   smem_start=%08x", (int)finfo.smem_start) ;
    qDebug("   smem_len=%d", finfo.smem_len) ;
    qDebug("   type=%d", finfo.type) ;
    qDebug("   type_aux=%d", finfo.type_aux) ;
    qDebug("   visual=%d", finfo.visual) ;
    qDebug("   xpanstep=%d", finfo.xpanstep) ;
    qDebug("   ypanstep=%d", finfo.ypanstep) ;
    qDebug("   ywrapstep=%d", finfo.ywrapstep) ;
    qDebug("   line_length=%d", finfo.line_length) ;
    qDebug("   mmio_start=%08x", (int)finfo.mmio_start) ;
    qDebug("   mmio_len=%d", finfo.mmio_len) ;
    qDebug("   accel=%d", finfo.accel) ;

    qDebug("vinfo: ") ;
    qDebug("   xres=%d", vinfo.xres) ;
    qDebug("   yres=%d", vinfo.yres) ;
    qDebug("   xres_virt=%d", vinfo.xres_virtual) ;
    qDebug("   yres_virt=%d", vinfo.yres_virtual) ;
    qDebug("   xoffset=%d", vinfo.xoffset) ;
    qDebug("   yoffset=%d", vinfo.yoffset) ;

    qDebug("   bitsperpixel=%d", vinfo.bits_per_pixel) ;
    qDebug("   grayscale=%d", vinfo.grayscale) ;

    qDebug("   nonstd=%d", vinfo.nonstd) ;
    qDebug("   activate=%d", vinfo.activate) ;

    qDebug("   height=%d", vinfo.height) ;
    qDebug("   width=%d", vinfo.width) ;

    qDebug("   flags=%d", vinfo.accel_flags) ;

    /* Timing: All values in pixclocks, except pixclock (of course) */
    qDebug("   pixclk=%d", vinfo.pixclock) ;
    qDebug("   lmargin=%d", vinfo.left_margin) ;
    qDebug("   rmargin=%d", vinfo.right_margin) ;
    qDebug("   umargin=%d", vinfo.upper_margin) ;
    qDebug("   bmargin=%d", vinfo.lower_margin) ;
    qDebug("   hsync_len=%d", vinfo.hsync_len) ;
    qDebug("   vsync_len=%d", vinfo.vsync_len) ;
    qDebug("   sync=%d", vinfo.sync) ;
    qDebug("   vmode=%d", vinfo.vmode) ;
    qDebug("   rotate=%d", vinfo.rotate) ;

    qDebug("Red %d %d %d",vinfo.red.offset,vinfo.red.length,
           vinfo.red.msb_right);
    qDebug("Green %d %d %d",vinfo.green.offset,vinfo.green.length,
           vinfo.green.msb_right);
    qDebug("Blue %d %d %d",vinfo.blue.offset,vinfo.blue.length,
           vinfo.blue.msb_right);
    qDebug("Transparent %d %d %d",vinfo.transp.offset,vinfo.transp.length,
           vinfo.transp.msb_right);
}
#endif

bool QKindleFb::connect(const QString &displaySpec)
{
    d_ptr->displaySpec = displaySpec;

    const QStringList args = displaySpec.split(QLatin1Char(':'));

    d_ptr->doGraphicsMode = false;

    if (args.contains(QLatin1String("flashing")))
        flashingUpdates = true ;
    else
        flashingUpdates = false ;

    if (args.contains(QLatin1String("debug")))
        debugMode = true ;
    else
        debugMode = false ;


    if (args.contains(QLatin1String("cursor")))
        hasCursor = true ;
    else
        hasCursor = false ;

    fullUpdateEvery = 1;
    const QStringList updateArg = args.filter(QLatin1String("update="));
    if (updateArg.length() == 1) {
        fullUpdateEvery = updateArg.at(0).section(QLatin1Char('='), 1, 1).toInt();
    }

    d_ptr->ttyDevice = EINK_FRAME_BUFFER ;

    QString dev = QLatin1String(EINK_FRAME_BUFFER);
    QString dev_alt = QLatin1String(EINK_FRAME_BUFFER_ALT_NAME);

    d_ptr->fd = open(dev.toLatin1().constData(), O_RDWR);

    /* try alt frame buffer name */
    if (d_ptr->fd == -1)
        d_ptr->fd = open(dev_alt.toLatin1().constData(), O_RDWR);

    if (d_ptr->fd == -1) {
        if (QApplication::type() == QApplication::GuiServer) {
            perror("QKindleFb::connect");
            qCritical("Error opening framebuffer device %s(%s)", qPrintable(dev), qPrintable(dev_alt));
            return false;
        }
    }

    ::fb_fix_screeninfo finfo;
    ::fb_var_screeninfo vinfo;

    //#######################
    memset(&vinfo, 0, sizeof(vinfo));
    memset(&finfo, 0, sizeof(finfo));
    //#######################

    /* Get fixed screen information */
    if (d_ptr->fd != -1 && ioctl(d_ptr->fd, FBIOGET_FSCREENINFO, &finfo)) {
        perror("QKindleFb::connect");
        qWarning("Error reading fixed information");
        return false;
    }

    d_ptr->driverType = strcmp(finfo.id, "eink_fb") ? GenericDriver : KindleEink ;

    /* Get variable screen information */
    if (d_ptr->fd != -1 && ioctl(d_ptr->fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("QKindleFb::connect");
        qWarning("Error reading variable information");
        return false;
    }


#ifdef DEBUG_VINFO
    show_info(&finfo, &vinfo) ;
#endif

    grayscale = vinfo.grayscale;
    d = vinfo.bits_per_pixel;

    lstep = finfo.line_length;

    isKindle5 = false ;
    isKindleTouch = false;
    if (d == 8)
    {
        isKindle4 = true ;
        if (lstep > 608)
            isKindle5 = true ;
        else if (lstep == 608)
            isKindleTouch = true ;
    }
    else
    {
        isKindle4 = false ;
    }


    int xoff = vinfo.xoffset;
    int yoff = vinfo.yoffset;
    const char* qwssize;
    if((qwssize=::getenv("QWS_SIZE")) && sscanf(qwssize,"%dx%d",&w,&h)==2) {
        if (d_ptr->fd != -1) {
            if ((uint)w > vinfo.xres) w = vinfo.xres;
            if ((uint)h > vinfo.yres) h = vinfo.yres;
        }
        dw=w;
        dh=h;
        int xxoff, yyoff;
        if (sscanf(qwssize, "%*dx%*d+%d+%d", &xxoff, &yyoff) == 2) {
            if (xxoff < 0 || xxoff + w > (int)vinfo.xres)
                xxoff = vinfo.xres - w;
            if (yyoff < 0 || yyoff + h > (int)vinfo.yres)
                yyoff = vinfo.yres - h;
            xoff += xxoff;
            yoff += yyoff;
        } else {
            xoff += (vinfo.xres - w)/2;
            yoff += (vinfo.yres - h)/2;
        }
    } else {
        dw=w=vinfo.xres;
        dh=h=vinfo.yres;
        if (debugMode)
        {
            qDebug(".. dw=%d, dh=%d, lstep=%d", dw, dh, lstep) ;
        }

    }

    if (w == 0 || h == 0) {
        qWarning("QKindleFb::connect(): Unable to find screen geometry, "
                 "will use 600x800.");
        dw = w = 600;
        dh = h = 800;
    }

    // Handle display physical size spec.
    if (vinfo.width != 0 && vinfo.height != 0
            && vinfo.width != UINT_MAX && vinfo.height != UINT_MAX) {
        physWidth = vinfo.width;
        physHeight = vinfo.height;
    } else {
        // the controller didn't report screen physical
        // dimensions. Set them manually:

        double dpi ;
        double mmperinch = 25.4 ;
        if (isKindle5)  // Kindle PaperWhite - 758x1024 @ 212dpi
        {
            dpi = 212 ;
        }
        else if (isKindle4) // Kindle 4 NT & Touch - 600x800 @ 167dpi
        {
            dpi = 167 ;
        }
        else if (lstep == 824)  // Kindle DX - 824x1200 @
        {
            dpi = 152 ;
        }
        else
            dpi = 167 ;         // all others.. (?)

        physWidth = qRound(dw*mmperinch/dpi) ;
        physHeight = qRound(dh*mmperinch/dpi) ;
    }

    if (debugMode)
        qDebug("physW=%d, physH=%d", physWidth, physHeight) ;

    dataoffset = yoff * lstep + xoff * d / 8;

    /* Figure out the size of the screen in bytes */
    size = h * lstep;

    mapsize = finfo.smem_len;

    data = (unsigned char *)-1;
    if (d_ptr->fd != -1)
        data = (unsigned char *)mmap(0, mapsize, PROT_READ | PROT_WRITE,
                                     MAP_SHARED , d_ptr->fd, 0);

    if ((long)data == -1) {
        if (QApplication::type() == QApplication::GuiServer) {
            perror("QKindleFb::connect");
            qWarning("Error: failed to map framebuffer device to memory.");
            return false;
        }
        data = 0;
    } else {
        data += dataoffset;
    }

    canaccel = false ;

    // Now read in palette
    if((vinfo.bits_per_pixel==8) || (vinfo.bits_per_pixel==4)) {
        screencols= (vinfo.bits_per_pixel==8) ? 256 : 16;

        // force screen colors to be 16 if it is K4
        if (isKindle4)
        {
            screencols = 16 ;
        }

        int loopc;
        ::fb_cmap startcmap;
        startcmap.start=0;
        startcmap.len=screencols;
        startcmap.red=(unsigned short int *)
                malloc(sizeof(unsigned short int)*screencols);
        startcmap.green=(unsigned short int *)
                malloc(sizeof(unsigned short int)*screencols);
        startcmap.blue=(unsigned short int *)
                malloc(sizeof(unsigned short int)*screencols);
        startcmap.transp=(unsigned short int *)
                malloc(sizeof(unsigned short int)*screencols);

        createPalette(startcmap, vinfo, finfo);

        int bits_used = 0;
        for(loopc=0;loopc<screencols;loopc++) {
            screenclut[loopc]=qRgb(startcmap.red[loopc] >> 8,
                                   startcmap.green[loopc] >> 8,
                                   startcmap.blue[loopc] >> 8);
            bits_used |= startcmap.red[loopc]
                    | startcmap.green[loopc]
                    | startcmap.blue[loopc];
        }
        // WORKAROUND: Some framebuffer drivers only return 8 bit
        // color values, so we need to not bit shift them..
        if ((bits_used & 0x00ff) && !(bits_used & 0xff00)) {
            for(loopc=0;loopc<screencols;loopc++) {
                screenclut[loopc] = qRgb(startcmap.red[loopc],
                                         startcmap.green[loopc],
                                         startcmap.blue[loopc]);
            }
            qWarning("8 bits cmap returned due to faulty FB driver, colors corrected");
        }
        free(startcmap.red);
        free(startcmap.green);
        free(startcmap.blue);
        free(startcmap.transp);
    } else {
        screencols=0;
    }

    return true;
}

/*!
    \reimp

    This unmaps the framebuffer.

    \sa connect()
*/

void QKindleFb::disconnect()
{
    data -= dataoffset;
    if (data)
        munmap((char*)data,mapsize);
    close(d_ptr->fd);
}

void QKindleFb::createPalette(fb_cmap &cmap, fb_var_screeninfo &vinfo, fb_fix_screeninfo &finfo)
{
    if((vinfo.bits_per_pixel==8) || (vinfo.bits_per_pixel==4)) {
        screencols= (vinfo.bits_per_pixel==8) ? 256 : 16;

        // force screen colors to be 16 if it is K4
        if (isKindle4)
        {
            screencols = 16 ;
        }

        cmap.start=0;
        cmap.len=screencols;
        cmap.red=(unsigned short int *)
                malloc(sizeof(unsigned short int)*screencols);
        cmap.green=(unsigned short int *)
                malloc(sizeof(unsigned short int)*screencols);
        cmap.blue=(unsigned short int *)
                malloc(sizeof(unsigned short int)*screencols);
        cmap.transp=(unsigned short int *)
                malloc(sizeof(unsigned short int)*screencols);

        if (screencols==16) {
            if (finfo.type == FB_TYPE_PACKED_PIXELS) {
                // We'll setup a grayscale cmap for 4bpp linear
                int val = 0;
                for (int idx = 0; idx < 16; ++idx, val += 17) {
                    cmap.red[idx] = (val<<8)|val;
                    cmap.green[idx] = (val<<8)|val;
                    cmap.blue[idx] = (val<<8)|val;
                    screenclut[idx]=qRgb(val, val, val);
                }

            } else {
                // Default 16 colour palette
                // Green is now trolltech green so certain images look nicer
                //                             black  d_gray l_gray white  red  green  blue cyan magenta yellow
                unsigned char reds[16]   = { 0x00, 0x7F, 0xBF, 0xFF, 0xFF, 0xA2, 0x00, 0xFF, 0xFF, 0x00, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x82 };
                unsigned char greens[16] = { 0x00, 0x7F, 0xBF, 0xFF, 0x00, 0xC5, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F };
                unsigned char blues[16]  = { 0x00, 0x7F, 0xBF, 0xFF, 0x00, 0x11, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x7F, 0x7F, 0x7F, 0x00, 0x00 };

                for (int idx = 0; idx < 16; ++idx) {
                    cmap.red[idx] = ((reds[idx]) << 8)|reds[idx];
                    cmap.green[idx] = ((greens[idx]) << 8)|greens[idx];
                    cmap.blue[idx] = ((blues[idx]) << 8)|blues[idx];
                    cmap.transp[idx] = 0;
                    screenclut[idx]=qRgb(reds[idx], greens[idx], blues[idx]);
                }
                if (debugMode)
                    qDebug("!!!!Default 16-color!!") ;
            }
        }
    }
}

/*!
    \reimp

    This is called by the \l{Qt for Embedded Linux} server at startup time.
    It turns off console blinking, sets up the color palette, enables write
    combining on the framebuffer and initialises the off-screen memory
    manager.
*/

bool QKindleFb::initDevice()
{
    fb_var_screeninfo vinfo;
    fb_fix_screeninfo finfo;

    memset(&vinfo, 0, sizeof(vinfo));
    memset(&finfo, 0, sizeof(finfo));

    if (ioctl(d_ptr->fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("QKindleFb::initDevice");
        qFatal("Error reading variable information in card init");
        return false;
    }

    if (ioctl(d_ptr->fd, FBIOGET_FSCREENINFO, &finfo)) {
        perror("QKindleFb::initDevice");
        qCritical("Error reading fixed information in card init");
        // It's not an /error/ as such, though definitely a bad sign
        // so we return true
        return true;
    }

    d_ptr->startupw=/*vinfo.xres*/ vinfo.xres_virtual ;
    d_ptr->startuph=vinfo.yres;
    d_ptr->startupd=vinfo.bits_per_pixel;
    grayscale = vinfo.grayscale;

    if ((vinfo.bits_per_pixel==8) || (vinfo.bits_per_pixel==4) || (finfo.visual==FB_VISUAL_DIRECTCOLOR))
    {
        fb_cmap cmap;
        createPalette(cmap, vinfo, finfo);

        free(cmap.red);
        free(cmap.green);
        free(cmap.blue);
        free(cmap.transp);
    }

    isDirty = 0 ;
    dirtyRect.setRect(-1,-1,0,0);

#ifndef QT_NO_QWS_CURSOR
    if (hasCursor)
    {
#ifdef USE_KINDLE_CURSOR
        qt_screencursor = new QKindleCursor(d_ptr->fd, data, dw, dh) ;
#else
        QScreenCursor::initSoftwareCursor();
#endif
    }
#endif
    blank(false);

    return true;
}

/*!
    \reimp

    This is called by the \l{Qt for Embedded Linux} server when it shuts
    down, and should be inherited if you need to do any card-specific cleanup.
    The default version hides the screen cursor and reenables the blinking
    cursor and screen blanking.
*/

void QKindleFb::shutdownDevice()
{
}

/*!
    \fn void QKindleFb::set(unsigned int index,unsigned int red,unsigned int green,unsigned int blue)

    Sets the specified color \a index to the specified RGB value, (\a
    red, \a green, \a blue), when in paletted graphics modes.
*/

/*!
    \reimp

    Sets the framebuffer to a new resolution and bit depth. The width is
    in \a nw, the height is in \a nh, and the depth is in \a nd. After
    doing this any currently-existing paint engines will be invalid and the
    screen should be completely redrawn. In a multiple-process
    Embedded Qt situation you must signal all other applications to
    call setMode() to the same mode and redraw.
*/

void QKindleFb::setMode(int nw,int nh,int nd)
{
#ifdef DEBUG_OUTPUT
    if (debugMode)
        qDebug("... setting FB mode to %dx%dx%d", nw, nh, nd) ;
#endif

    if (d_ptr->fd == -1)
        return;

    fb_fix_screeninfo finfo;
    fb_var_screeninfo vinfo;

    //#######################
    memset(&vinfo, 0, sizeof(vinfo));
    memset(&finfo, 0, sizeof(finfo));
    //#######################

    if (ioctl(d_ptr->fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("QKindleFb::setMode");
        qFatal("Error reading variable information in mode change");
    }

    vinfo.xres=nw;
    vinfo.yres=nh;
    vinfo.bits_per_pixel=nd;

    if (ioctl(d_ptr->fd, FBIOPUT_VSCREENINFO, &vinfo)) {
        perror("QKindleFb::setMode");
        qCritical("Error writing variable information in mode change");
    }

    if (ioctl(d_ptr->fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("QKindleFb::setMode");
        qFatal("Error reading changed variable information in mode change");
    }

    if (ioctl(d_ptr->fd, FBIOGET_FSCREENINFO, &finfo)) {
        perror("QKindleFb::setMode");
        qFatal("Error reading fixed information");
    }

    disconnect();
    connect(d_ptr->displaySpec);
    exposeRegion(region(), 0);
}

/*!
    \reimp
*/
void QKindleFb::setDirty(const QRect &r)
{
    // mark it dirty ant calculate the resulting bounding rect
    // of dirty area
    isDirty = true ;
    if (dirtyRect.x() == -1)
        dirtyRect = r ;
    else
        dirtyRect = dirtyRect.united(r) ;

#ifdef DEBUG_OUTPUT
    if (debugMode)
        qDebug(">> dirtyRect set to %d,%d %dx%d", dirtyRect.x(), dirtyRect.y(), dirtyRect.width(), dirtyRect.height()) ;
#endif
}

static int partialUpdatesCount = 0;

void QKindleFb::exposeRegion(QRegion region, int changing)
{
    bool doFullUpdate = flashingUpdates  ;

#ifdef DEBUG_OUTPUT
    if (debugMode)
        qDebug("\nExposing ...");
#endif

    QScreen::exposeRegion(region, changing);

    if (isDirty)
    {
        // force flashing update if updating the full screen
        if(dirtyRect.left() == 0 && dirtyRect.top() == 0 && dirtyRect.width() >= dw && dirtyRect.height() == dh) {
            partialUpdatesCount++;
            if (partialUpdatesCount >= fullUpdateEvery) {
                doFullUpdate = true ;
                partialUpdatesCount = 0;
            }
        }

#ifdef DEBUG_OUTPUT
        if (debugMode)
        {
            qDebug("Displaying rect (%d,%d %dx%d), %s screen update, updmode=%d",
                   dirtyRect.x(), dirtyRect.y(), dirtyRect.width(), dirtyRect.height(),
                   (doFullUpdate)? "full" : "partial", fullUpdateEvery);
        }
#endif

        // Kindle e-Ink displays need a trigger to actually show what is
        // in their framebuffer memory. The eink_fb driver does this
        // by sending custom IOCTLs - FBIO_EINK_UPDATE_DISPLAY_AREA ,
        // it takes an argument describing the updated screen area and
        // specifying whether or not to flash the screen while updating.

        if (isKindle5 || isKindleTouch)
        {
            mxcfb_update_data ud ;

            ud.update_region.left = dirtyRect.left();
            ud.update_region.top = dirtyRect.top();
            ud.update_region.width = dirtyRect.width();
            ud.update_region.height = dirtyRect.height();

            ud.waveform_mode = WAVEFORM_MODE_GC16_FAST ;
            ud.update_mode = (doFullUpdate == false) ? UPDATE_MODE_PARTIAL : UPDATE_MODE_FULL ;
            ud.temp = TEMP_USE_PAPYRUS;
            ud.flags = 0;

            ioctl(d_ptr->fd, MXCFB_SEND_UPDATE, &ud);
        }
        else
        {
            update_area_t ua;
            ua.x1 = dirtyRect.left();
            ua.y1 = dirtyRect.top();
            ua.x2 = dirtyRect.right() + 1;
            ua.y2 = dirtyRect.bottom() + 1;
            ua.which_fx = (doFullUpdate == false) ? fx_update_partial : fx_update_full; //fx_invert
            ua.buffer = NULL;

            ioctl(d_ptr->fd, FBIO_EINK_UPDATE_DISPLAY_AREA, &ua);
        }

        dirtyRect.setRect(-1,-1, 0, 0);
        isDirty = false ;

#ifdef DEBUG_OUTPUT
        if (debugMode)
            qDebug("______________") ;
#endif
    }
}

void QKindleFb::invertRect(int x0, int y0, int x1, int y1)
{
    update_area_t ua;
    ua.x1 = x0;
    ua.y1 = y0;
    ua.x2 = x1 + 1;
    ua.y2 = y1 + 1;
    ua.which_fx = fx_invert ;
    ua.buffer = NULL;

    ioctl(d_ptr->fd, FBIO_EINK_UPDATE_DISPLAY_AREA, &ua);
}

void QKindleFb::setFlashingMode(bool flag)
{
    flashingUpdates = flag ;
}

/*!
    \reimp
*/
void QKindleFb::blank(bool on)
{
    if (d_ptr->blank == on)
        return;

    if (d_ptr->fd == -1)
        return;
    // Some old kernel versions don't have this.  These defines should go
    // away eventually
#if defined(FBIOBLANK)
#if defined(VESA_POWERDOWN) && defined(VESA_NO_BLANKING)
    ioctl(d_ptr->fd, FBIOBLANK, on ? VESA_POWERDOWN : VESA_NO_BLANKING);
#else
    ioctl(d_ptr->fd, FBIOBLANK, on ? 1 : 0);
#endif
#endif

    d_ptr->blank = on;
}



///////////////////////////////
void QKindleFb::solidFill(const QColor &color, const QRegion &reg)
{
    int c;

    c = qGray(color.red(), color.green(), color.blue()) >> 4 ; ////alloc(color.red(), color.green(), color.blue());

    const QVector<QRect> rects = (reg & region()).rects();
    for (int i = 0; i < rects.size(); ++i) {
        const QRect r = rects.at(i);
        k4_fillbox(r.left(), r.top(), r.width(), r.height(), c);
    }

    if (debugMode)
        qDebug("solidFill: c=%0X", c) ;

}

void QKindleFb::blit16To4(const QImage &image,
                          const QPoint &topLeft, const QRegion &region)
{
    const int imageStride = image.bytesPerLine() / 2;
    const QVector<QRect> rects = region.rects();
    int scolor ;

    for (int i = 0; i < rects.size(); ++i) {
        const QRect r = rects.at(i).translated(-topLeft);
        int y = r.y();
        const quint16 *s = reinterpret_cast<const quint16*>(image.scanLine(y));

        while (y <= r.bottom()) {
            int x1 = r.x();
            while (x1 <= r.right()) {
                const quint16 c = s[x1];
                int x2 = x1;
                // find span length
                while ((x2+1 < r.right()) && (s[x2+1] == c))
                    ++x2;

                scolor = alloc((c>>11) & 0x1f, (c>>5) & 0x3f, c & 0x1f) ;

                k4_hline(x1 + topLeft.x(), y + topLeft.y(), x2 + topLeft.x(), scolor);

                x1 = x2 + 1;
            }
            s += imageStride;
            ++y;
        }
    }
}

void QKindleFb::blit12To4(const QImage &image,
                          const QPoint &topLeft, const QRegion &region)
{
    const int imageStride = image.bytesPerLine() / 2;
    const QVector<QRect> rects = region.rects();
    int scolor ;

    for (int i = 0; i < rects.size(); ++i) {
        const QRect r = rects.at(i).translated(-topLeft);
        int y = r.y();
        const quint16 *s = reinterpret_cast<const quint16*>(image.scanLine(y));

        while (y <= r.bottom()) {
            int x1 = r.x();
            while (x1 <= r.right()) {
                const quint16 c = s[x1];
                int x2 = x1;
                // find span length
                while ((x2+1 < r.right()) && (s[x2+1] == c))
                    ++x2;

                scolor = alloc((c>>8) & 0x0f, (c>>4) & 0x0f, c & 0x0f) ;

                k4_hline(x1 + topLeft.x(), y + topLeft.y(), x2 + topLeft.x(), scolor);

                x1 = x2 + 1;
            }
            s += imageStride;
            ++y;
        }
    }
}


void QKindleFb::blit32To4(const QImage &image,
                          const QPoint &topLeft, const QRegion &region)
{
    const int imageStride = image.bytesPerLine() / 4;
    const QVector<QRect> rects = region.rects();
    int scolor ;

    for (int i = 0; i < rects.size(); ++i) {
        const QRect r = rects.at(i).translated(-topLeft);
        int y = r.y();
        const quint32 *s = reinterpret_cast<const quint32*>(image.scanLine(y));

        while (y <= r.bottom()) {
            int x1 = r.x();
            while (x1 <= r.right()) {
                const quint32 c = s[x1];
                int x2 = x1;
                // find span length
                while ((x2+1 < r.right()) && (s[x2+1] == c))
                    ++x2;

                scolor = alloc(qRed(c), qGreen(c), qBlue(c)) ;

                k4_hline(x1 + topLeft.x(), y + topLeft.y(), x2 + topLeft.x(), scolor);

                x1 = x2 + 1;
            }
            s += imageStride;
            ++y;
        }
    }
}

void QKindleFb::blit(const QImage& image, const QPoint& topLeft, const QRegion& region)
{
    QImage imageInverted = image;

    if (!isKindle5 && !isKindleTouch)
        imageInverted.invertPixels();

#ifdef DEBUG_OUTPUT
    if (debugMode)
        qDebug(">>>>%s%s blit image %dx%dx%d type=%d", (isKindle4)? "K4":"", (isKindle5)? "K5":"",image.width(),image.height(), image.depth() ,image.format()) ;
#endif

    if ((isKindle4) || (isKindle5))
    {
        blit_K4( imageInverted, topLeft, region);
    }
    else
    {
        QScreen::blit(imageInverted, topLeft, region);
    }
}

void QKindleFb::blit_K4(const QImage &img, const QPoint &topLeft,
                        const QRegion &reg)
{

    switch (img.format()) {
    case QImage::Format_RGB16:
        blit16To4(img, topLeft, reg);
        return;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
        blit32To4(img, topLeft, reg);
        return;
    case QImage::Format_RGB444:
        blit12To4(img, topLeft, reg);
        return;
    default:
        break;
    }

    QScreen::blit(img, topLeft, reg);
}

void QKindleFb::k4_hline(int x1, int y, int x2, int c)
{
    uchar *p = base() + y*linestep() + x1 ;

    c = ((c<<4)+(c & 0x0f)) ;

    for (; x1 <= x2; x1++)
        *p++ = (uchar)  c ;
}

void QKindleFb::k4_fillbox(int left, int top, int width, int height, int c)
{
    uchar *p = base() + top*linestep() + left ;
    uchar *q ;
    int x, y ;

    c = (((c << 4) | (c & 0x0f))) & 0xff ;

    for (y = 0; y < height; y++)
    {
        q = p ;
        for (x = 0; x < width; x++)
            *q++ = (uchar) c ;
        p += linestep() ;
    }

}

void QKindleFb::setFullUpdateEvery(int n)
{
    fullUpdateEvery = n ;
    partialUpdatesCount = n; // to force immediate update after change
}

QT_END_NAMESPACE
