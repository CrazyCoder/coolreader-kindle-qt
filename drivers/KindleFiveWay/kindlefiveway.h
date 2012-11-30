// kindlefiveway.h
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

#ifndef KINDLEFIVEWAY_H
#define KINDLEFIVEWAY_H

#include <unistd.h>

#include <QWSMouseHandler>
#include <QSocketNotifier>

class KindleFiveWay : public QObject, public QWSMouseHandler
{
    Q_OBJECT

public:
    KindleFiveWay(const QString & driver = QString(), const QString & device = QString(), QObject* parent = 0);
    virtual ~KindleFiveWay();

    virtual void suspend();
    virtual void resume();

private slots:
    void activity(int);
    void setKeypadMode(bool keypadMode);

private:
    int _fd;
    QSocketNotifier* _sn;

    bool _debug;
    bool _keypadMode;
    int _up, _down, _left, _right, _button;
};


#endif // KINDLEFIVEWAY_H
