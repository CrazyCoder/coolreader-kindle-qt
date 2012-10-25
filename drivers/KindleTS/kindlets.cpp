#include <fcntl.h>
#include <linux/input.h>
#include "kindlets.h"
#include <QWSServer>
#include <QScreenCursor>

KindleTS::KindleTS(const QString & driver, const QString & device, QObject* parent) : QObject(parent), QWSMouseHandler(driver, device)
{
    _debug = device.contains("debug", Qt::CaseInsensitive);

    if (_debug)
        qDebug("KindleTS(%s, %s)", (const char*)driver.toAscii(), (const char*)device.toAscii());

    _fd = open("/dev/input/event0", O_RDONLY);

    if (_debug)
    {
        if (_fd == -1)
            qDebug("event0 open FAILED");
        else
            qDebug("event0 open SUCCESS");
    }

    _sn = new QSocketNotifier(_fd, QSocketNotifier::Read);

    connect(_sn, SIGNAL(activated(int)), this, SLOT(activity(int)));

    _sn->setEnabled(true);

    p.setX(0); p.setY(0);
    touch = newtouch = false ;
    doubletap = newdoubletap = false ;

    capture_input() ;
}

KindleTS::~KindleTS()
{
    release_input() ;
    delete _sn;
    close(_fd);
}

void KindleTS::suspend()
{
    _sn->setEnabled(false);
}

void KindleTS::resume()
{
    _sn->setEnabled(true);
}

typedef struct input_event input_event_t;

void KindleTS::activity(int)
{
    _sn->setEnabled(false);

    input_event_t in; ssize_t size ;

    size = read(_fd, &in, sizeof(input_event_t));

    if (_debug)
        qDebug("TS data: type %X, code %X, value %d", in.type, in.code, in.value);

    switch(in.type)
    {
    case EV_ABS:
        if (in.code == ABS_MT_POSITION_X)
            p.setX(in.value);
        else if (in.code == ABS_MT_POSITION_Y)
            p.setY(in.value);
        break ;
    case EV_KEY:
        if (in.code == BTN_TOUCH)
        {
            newtouch = (in.value == 0) ? false : true ;
        }
        else if (in.code == BTN_TOOL_DOUBLETAP)
        {
            newdoubletap = (in.value == 0) ? false : true ;
            mouseChanged(p, (newdoubletap) ? Qt::RightButton : 0, 0);
        }
        break ;
    case EV_SYN:
        if (!doubletap && !newdoubletap) mouseChanged(p, (newtouch) ? Qt::LeftButton : 0, 0);
        touch = newtouch ;
        doubletap = newdoubletap ;
        break ;
    default:
        break ;
    }

    if (_debug)
    {
        qDebug("x=%d, y=%d, touch=%d %d, doubletap=%d %d", p.x(), p.y(), touch, newtouch, doubletap, newdoubletap);
        if (in.type == EV_SYN)
            qDebug("________________") ;
    }

    _sn->setEnabled(true);
}

void KindleTS::capture_input(void)
{
    int on = 1 ;

    if (!input_captured )
    {
        if (_debug)
            qDebug("attempting to capture input...");

        if (_fd != -1)
        {
            if (ioctl(_fd, EVIOCGRAB, on)) {
                if (_debug)
                    qDebug("Capture kbd input: error");
            }
        }

        input_captured = true ;
    }
}

void KindleTS::release_input(void)
{
    int off = 0 ;

    if (input_captured )
    {
        if (_debug)
            qDebug("attempting to release input...");
        if (_fd != -1)
        {
            if (ioctl(_fd, EVIOCGRAB, off)) {
                if (_debug)
                    qDebug("Release kbd input: error");
            }
        }

        input_captured = false ;
    }
}
