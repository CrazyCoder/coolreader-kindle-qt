#ifdef USE_KINDLE_CURSOR

#ifndef QKINDLECURSOR_H
#define QKINDLECURSOR_H

// qkindlecursor.h
//
//
// This file is licensed under the LGPL version 2.1, the text of which should
// be in the LICENSE.txt file, or alternately at this location:
// http://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
//
// DISCLAIMER: This software is released AS-IS with ABSOLUTELY NO WARRANTY OF
// ANY KIND.  The use of this software indicates your knowledge and acceptance
// of this.


#include <qscreenlinuxfb_qws.h>
#include <QThread>
#include <QRect>
#include <QMutex>
#include <QRegion>
#include <QScreenCursor>


class QKindleCursor : public QScreenCursor
{
public:
    QKindleCursor(int fd, unsigned char *bufp, int w, int h);
    virtual void set(const QImage &image, int hotx, int hoty);
    virtual void move(int x, int y);
    virtual void show();
    virtual void hide();
private:
    int screen_fd ;
    int curx ;
    int cury ;
    int curw ;
    int curh ;
    bool visible ;
    int screen_fbd ;
    unsigned char *pfb ;
    int dw ;
    int dh ;
};
#endif // QKINDLECURSOR_H

#endif
