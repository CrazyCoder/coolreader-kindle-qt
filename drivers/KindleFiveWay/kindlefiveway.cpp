// kindlefiveway.cpp
//
// Copyright (C) 2010, 2009 Griffin I'Net, Inc. (blog@griffin.net)
//
// This file is licensed under the LGPL version 2.1, the text of which should
// be in the LICENSE.txt file, or alternately at this location:
// http://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
//
// DISCLAIMER: This software is released AS-IS with ABSOLUTELY NO WARRANTY OF
// ANY KIND.  The use of this software indicates your knowledge and acceptance
// of this.

#include <fcntl.h>
#include <linux/input.h>
#include "kindlefiveway.h"
#include <QWSServer>
#include <QScreenCursor>

#include "keycodes.h"

KindleFiveWay::KindleFiveWay(const QString & driver, const QString & device, QObject* parent) : QObject(parent), QWSMouseHandler(driver, device)
{
    _debug = device.contains("debug", Qt::CaseInsensitive);

    if (_debug)
        qDebug("KindleFiveWay(%s, %s)", (const char*)driver.toAscii(), (const char*)device.toAscii());

    _up = 0;
    _down = 0;
    _left = 0;
    _right = 0;
    _button = 0;
    _keypadMode = false;

    if (device.contains("keypad", Qt::CaseInsensitive))
    {
        if (_debug)
            qDebug("KEYPAD mode flag IGNORED");
        ////setKeypadMode(true);
    }
    else if (_debug)
        qDebug("KEYPAD mode OFF (%s)", (const char*)device.toAscii());

    _fd = open("/dev/input/event1", O_RDONLY);

    if (_debug)
    {
        if (_fd == -1)
            qDebug("event1 open FAILED");
        else
            qDebug("event1 open SUCCESS");
    }

    _sn = new QSocketNotifier(_fd, QSocketNotifier::Read);

    connect(_sn, SIGNAL(activated(int)), this, SLOT(activity(int)));

    _sn->setEnabled(true);
}

KindleFiveWay::~KindleFiveWay()
{
    delete _sn;
    close(_fd);
}

void KindleFiveWay::suspend()
{
    _sn->setEnabled(false);
}

void KindleFiveWay::resume()
{
    _sn->setEnabled(true);
}

void KindleFiveWay::setKeypadMode(bool keypadMode)
{
    _keypadMode = false ; //keypadMode;

    if (_keypadMode)
    {
        QScreenCursor::instance()->hide();
        QWSServer::setCursorVisible(false);
        mouseChanged(QPoint(0,1200), 0, 0);
    }
    else
    {
        QScreenCursor::instance()->show();
        QWSServer::setCursorVisible(true);
        mouseChanged(pos(), 0, 0);
    }
}

typedef struct input_event input_event_t;

void KindleFiveWay::activity(int)
{
    _sn->setEnabled(false);

    input_event_t in;

    read(_fd, &in, sizeof(input_event_t));

    if (_debug)
        qDebug("FiveWay: type %d, code %d, value %d", in.type, in.code, in.value);

    if (in.type == 1)
    {
        QPoint p = pos();

        switch(in.code)
        {
        default:
            break;
        case KDX_KEY_5WPRESS:
        case K3_KEY_5WPRESS:
            // button press
            if (_keypadMode)
            {
                QWSServer::sendKeyEvent('\n', Qt::Key_Return, Qt::NoModifier, in.value != 0, in.value == 2);
            }
            else
            {
                if (in.value)
                    _button = !_button;
            }
            break;
        case KDX_KEY_LARROW:
            // left
            if (_keypadMode)
            {
                QWSServer::sendKeyEvent(0, Qt::Key_Left, Qt::NoModifier, in.value != 0, in.value == 2);
            }
            else if (in.value == 0)
            {
                _left = 0;
            }
            else if (in.value == 1)
            {
                _left = 1;
                p.setX(p.x() - 5);
            }
            else
            {
                if (_left < 6)
                    _left++;

                p.setX(p.x() - 5 * _left);
            }
            break;
        case KDX_KEY_RARROW:
            // right
            if (_keypadMode)
            {
                QWSServer::sendKeyEvent(0, Qt::Key_Right, Qt::NoModifier, in.value != 0, in.value == 2);
            }
            else if (in.value == 0)
            {
                _right = 0;
            }
            else if (in.value == 1)
            {
                _right = 1;
                p.setX(p.x() + 5);
            }
            else
            {
                if (_right < 6)
                    _right++;

                p.setX(p.x() + 5 * _right);
            }
            break;
        case KDX_KEY_UPARROW:
        case K3_KEY_UPARROW:
            // up
            if (_keypadMode)
            {
                QWSServer::sendKeyEvent(0, Qt::Key_Up, Qt::NoModifier, in.value != 0, in.value == 2);
            }
            else if (in.value == 0)
            {
                _up = 0;
            }
            else if (in.value == 1)
            {
                _up = 1;
                p.setY(p.y() - 5);
            }
            else
            {
                if (_up < 6)
                    _up++;

                p.setY(p.y() - 5 * _up);
            }
            break;
        case KDX_KEY_DNARROW:
        case K3_KEY_DNARROW:
            // down
            if (_keypadMode)
            {
                QWSServer::sendKeyEvent(0, Qt::Key_Down, Qt::NoModifier, in.value != 0, in.value == 2);
            }
            else if (in.value == 0)
            {
                _down = 0;
            }
            else if (in.value == 1)
            {
                _down = 1;
                p.setY(p.y() + 5);
            }
            else
            {
                if (_down < 6)
                    _down++;

                p.setY(p.y() + 5 * _down);
            }
            break;
        }

        limitToScreen(p);

        if (!_keypadMode)
            mouseChanged(p, _button ? Qt::LeftButton : 0, 0);
    }

    _sn->setEnabled(true);
}

