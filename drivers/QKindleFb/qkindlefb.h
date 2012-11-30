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

#ifndef QKINDLEFB_H
#define QKINDLEFB_H

#include <QScreen>
#include <device.h>

struct fb_cmap;
struct fb_var_screeninfo;
struct fb_fix_screeninfo;


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QKindleFbPrivate;

class Q_GUI_EXPORT QKindleFb : public QScreen
{
public:
    explicit QKindleFb(int display_id);
    virtual ~QKindleFb();

    virtual bool initDevice();
    virtual bool connect(const QString &displaySpec);

    enum DriverTypes { GenericDriver, EInk8Track, KindleEink };

    virtual void disconnect();
    virtual void shutdownDevice();
    virtual void blank(bool on);
    virtual void setMode(int,int,int);

    virtual void setDirty(const QRect&);
    virtual void exposeRegion(QRegion region, int changing) ;
    virtual void blit(const QImage& image, const QPoint& topLeft, const QRegion& region);
    void solidFill(const QColor &color, const QRegion &region);

    virtual void invertRect(int x0, int y0, int x1, int y1) ;
    virtual void setFlashingMode(bool flag) ;


    QImage::Format alphaPixmapFormat() const ;
    static QKindleFb *instance() ;

    virtual void setFullUpdateEvery(int n) ;
    virtual void forceFullUpdate(bool fullScreen = false);

protected:
    bool canaccel;
    int dataoffset;

private:
    void k4_hline(int x1, int y, int x2, int c) ;
    void k4_fillbox(int left, int top, int width, int height, int c);
    void blit_K4(const QImage& image, const QPoint& topLeft, const QRegion& region);
    void blit12To4(const QImage &image, const QPoint &topLeft, const QRegion &region);
    void blit16To4(const QImage &image, const QPoint &topLeft, const QRegion &region);
    void blit32To4(const QImage &image, const QPoint &topLeft, const QRegion &region);
    void createPalette(fb_cmap &cmap, fb_var_screeninfo &vinfo, fb_fix_screeninfo &finfo);
    void setPixelFormat(struct fb_var_screeninfo);

    QKindleFbPrivate *d_ptr;

    bool isKindle4 ;
    bool isKindle5 ;
    bool isKindleTouch ;
    bool flashingUpdates ;
    bool isDirty ;
    QRect dirtyRect ;

    bool debugMode ;
    bool hasCursor ;

    int fullUpdateEvery;
    bool isFullUpdateForced;
    bool isFullScreenForced;
    QRect customRect;
};


QT_END_NAMESPACE

QT_END_HEADER

#endif // QKINDLEFB_H
