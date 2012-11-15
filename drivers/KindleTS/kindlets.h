#ifndef KINDLETS_H
#define KINDLETS_H

#include <QWSMouseHandler>
#include <QSocketNotifier>
#include <QWSServer>

#include <fcntl.h>
#include <linux/input.h>

#include "../../cr3-kindle/src/device.h"

class KindleTS : public QObject, public QWSMouseHandler, public QWSKeyboardHandler
{
    Q_OBJECT

public:
    KindleTS(const QString & driver = QString(), const QString & device = QString(), QObject* parent = 0);
    virtual ~KindleTS();

    virtual void suspend();
    virtual void resume();

private slots:
    void activity(int);
    void kt_activity(int);

private:
    int width, height;

    int _fd;
    QSocketNotifier* _sn;

    int kt_fd;
    QSocketNotifier* kt_sn;

    bool _debug;

    bool touch ;
    bool doubletap ;

    bool newtouch ;
    bool newdoubletap ;

    int oldX, oldY;

    QPoint p, oldP ;

    bool input_captured ;

    void capture_input(void) ;
    void release_input(void) ;

    bool isKT;
};

#define EV_SYN			0x00
#define EV_KEY			0x01
#define EV_REL			0x02
#define EV_ABS			0x03

#define ABS_MT_POSITION_X	0x35	/* Center X ellipse position */
#define ABS_MT_POSITION_Y	0x36	/* Center Y ellipse position */

#define BTN_TOOL_FINGER		0x145
#define BTN_TOUCH		0x14a
#define BTN_TOOL_DOUBLETAP	0x14d

#define KT_HOME 102

#endif // KINDLETS_H
