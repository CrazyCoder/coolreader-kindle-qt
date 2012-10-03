// kindlekeyboard.h
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

#ifndef KINDLEKEYBOARD_H
#define KINDLEKEYBOARD_H

#include <QWSKeyboardHandler>
#include <QSocketNotifier>

class KindleKeyboard : public QObject, public QWSKeyboardHandler
{
    Q_OBJECT

public:
    KindleKeyboard(const QString & driver = QString(), const QString & device = QString(), QObject* parent = 0);
    virtual ~KindleKeyboard();

private slots:
    void activity(int);
    void k3_activity(int);
    void fiveway_activity(int);

private:
    int _fd;
    QSocketNotifier* _sn;

    int fiveway_fd;
    QSocketNotifier* fiveway_sn;

    int k3_fd;
    QSocketNotifier* k3_sn;

    bool _shift;
    bool _alt;
    bool _altgray;
    bool _debug;
    int _nshifts ;

    bool input_captured ;
    bool alt_just_pressed ;
    bool altshift_just_pressed ;
    bool is_qt_app_frozen ;

    Qt::KeyboardModifiers GetCurModifiers() ;

private:
    void capture_input(void) ;
    void release_input(void) ;
};


#endif // KINDLEKEYBOARD_H
