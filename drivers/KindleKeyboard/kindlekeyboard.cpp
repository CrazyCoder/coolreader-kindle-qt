// kindlekeyboard.cpp
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

#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h>
#include "kindlekeyboard.h"
#include <QtCore>
#include <QFile>

#include "keycodes.h"
#include "screenshot/screenshot.h"

static int suspend_cvm()
{
#if 0
    return (system("kill -STOP `pidof cvm`")) ;
#else
    return 0 ;
#endif
}

static int resume_cvm(int is_kindle3)
{
    QString cmdText ;

    if (is_kindle3)
        cmdText = QString("kill -CONT `pidof cvm`; echo send %1 >/proc/keypad").arg(K3_KEY_HOME) ;
    else
        cmdText = QString("kill -CONT `pidof cvm`; echo send %1 >/proc/keypad").arg(KDX_KEY_HOME) ;

    return (system(cmdText.toAscii())) ;
}



void KindleKeyboard::capture_input(void)
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
            if (fiveway_fd != -1)
            {
                    if (ioctl(fiveway_fd, EVIOCGRAB, on)) {
                        if (_debug)
                            qDebug("Capture fw input: error");
                    }
            }
            if (k3_fd != -1)
            {
                if (ioctl(k3_fd, EVIOCGRAB, on)) {
                    if (_debug)
                        qDebug("Capture k3_vol input: error");
                }
            }

            input_captured = true ;
        }
}

void KindleKeyboard::release_input(void)
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
                if (fiveway_fd != -1)
                {
                        if (ioctl(fiveway_fd, EVIOCGRAB, off)) {
                            if (_debug)
                                qDebug("Release fw input: error");
                        }
                }
                if (k3_fd != -1)
                {
                        if (ioctl(k3_fd, EVIOCGRAB, off)) {
                            if (_debug)
                                qDebug("Release k3_vol input: error");
                        }
                }

                input_captured = false ;
        }
}


KindleKeyboard::KindleKeyboard(const QString & driver, const QString & device, QObject* parent) : QObject(parent), QWSKeyboardHandler(device)
{
    _debug = device.contains("debug", Qt::CaseInsensitive);

    if (_debug)
    {
        qDebug("KindleKeyboard(%s, %s)", (const char*)driver.toAscii(), (const char*)device.toAscii());
    }

    _shift = false ;
    _alt = false ;
    _altgray=false ;
    _nshifts=0 ;

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

    fiveway_fd = open("/dev/input/event1", O_RDONLY);

    if (fiveway_fd != -1)
    {
        fiveway_sn = new QSocketNotifier(fiveway_fd, QSocketNotifier::Read);
        connect(fiveway_sn, SIGNAL(activated(int)), this, SLOT(fiveway_activity(int)));
        fiveway_sn->setEnabled(true);
    }


    k3_fd = open("/dev/input/event2", O_RDONLY);

    if (k3_fd != -1)
    {
        k3_sn = new QSocketNotifier(k3_fd, QSocketNotifier::Read);
        connect(k3_sn, SIGNAL(activated(int)), this, SLOT(k3_activity(int)));
        k3_sn->setEnabled(true);
    }

    input_captured = false ;
    alt_just_pressed = false ;
    altshift_just_pressed = false ;
    is_qt_app_frozen = false ;

    capture_input() ;
}

KindleKeyboard::~KindleKeyboard()
{
    if (!is_qt_app_frozen)
    {
        qDebug("...resuming CVM") ;
        release_input() ;
        resume_cvm(k3_fd != -1) ;
    }
    else
        is_qt_app_frozen = false ;


    delete _sn;
    close(_fd);
    delete fiveway_sn ;
    close(fiveway_fd) ;

    if (k3_fd != -1)
    {
        delete k3_sn ;
        close(k3_fd) ;
    }
}

typedef struct input_event input_event_t;

Qt::KeyboardModifiers KindleKeyboard::GetCurModifiers()
{
    Qt::KeyboardModifiers rc = Qt::NoModifier ;

    if (_shift)
        rc = rc | Qt::ShiftModifier ;
    if (_alt)
        rc = rc  | Qt::AltModifier ;
    if (_altgray)
        rc = rc  | Qt::KeypadModifier ;

    return rc ;
}

void KindleKeyboard::fiveway_activity(int)
{
    int unicode, keycode ;
    Qt::KeyboardModifiers mods ;
    bool ispressed, isautorpt ;

    fiveway_sn->setEnabled(false);

    input_event_t in;

    read(fiveway_fd, &in, sizeof(input_event_t));

    if (_debug)
    {
        QString debugText = QString("Fiveway: type %1, code %2, value %3").arg(in.type).arg(in.code).arg(in.value) ;

        if (_shift)
            debugText += " Shift" ;
        if (_alt)
            debugText += " Alt" ;
        if (_altgray)
            debugText += " AltGr" ;


        qDebug("%s", (const char*)debugText.toAscii());
    }

    if (in.type == 1)
    {
       switch(in.code)
        {
        case KDX_KEY_5WPRESS:
        case K3_KEY_5WPRESS:
            // button press
            unicode = '\n' ;
            keycode = Qt::Key_Select ;
            mods = GetCurModifiers() ;
            ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
            break;
        case KDX_KEY_LARROW:
            //left
            unicode = 0 ;
            keycode = Qt::Key_Left ;
            mods = GetCurModifiers() ;
            ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
            break;
        case KDX_KEY_RARROW:
            // right
            unicode = 0 ;
            keycode = Qt::Key_Right ;
            mods = GetCurModifiers() ;
            ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
            break;
        case KDX_KEY_UPARROW:
        case K3_KEY_UPARROW:
            // up
            unicode = 0 ;
            keycode = Qt::Key_Up ;
            mods = GetCurModifiers() ;
            ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
            break;
        case KDX_KEY_DNARROW:
        case K3_KEY_DNARROW:
            // down
            unicode = 0 ;
            keycode = Qt::Key_Down ;
            mods = GetCurModifiers() ;
            ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
            break ;
        default:
            unicode = -1 ;
            ispressed = 0 ;
            isautorpt = 0 ;
            keycode = 0 ;
            break;
        }

        if ((is_qt_app_frozen == false) && (unicode != -1))
            processKeyEvent(unicode, keycode, mods, ispressed, isautorpt);
    }

    fiveway_sn->setEnabled(true);
}

void KindleKeyboard::k3_activity(int)
{
    int unicode, keycode ;
    Qt::KeyboardModifiers mods ;
    bool ispressed, isautorpt ;


    k3_sn->setEnabled(false);

    input_event_t in;

    read(k3_fd, &in, sizeof(input_event_t));

    if (in.type == 1)
    {
        if (_debug)
        {
            QString debugText = QString("Keyboard: type %1, code %2, value %3").arg(in.type).arg(in.code).arg(in.value) ;

            if (_shift)
                debugText += " Shift" ;
            if (_alt)
                debugText += " Alt" ;
            if (_altgray)
                debugText += " AltGr" ;


            qDebug("%s", (const char*)debugText.toAscii());
        }

        switch(in.code)
        {
            case K3_KEY_VPLUS: // Vol+
                unicode = '+' ;
                keycode = Qt::Key_Plus ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case K3_KEY_VMINUS: // Vol-
                unicode = '-' ;
                keycode = Qt::Key_Minus ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            default:
                unicode = -1 ;
                keycode = -1 ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                if (_debug)
                {
                    QString debugText = QString("***Unknown: type %1, code %2, value %3").arg(in.type).arg(in.code).arg(in.value);
                    qDebug("%s", (const char*)debugText.toAscii());
                    //logKey(debugText);
                }
                break;
        }

        if ((is_qt_app_frozen == false) && (unicode != -1))
        {
            ////QString debugText = QString("Key:%1, Unicode: %2").arg(unicode).arg(keycode);

            processKeyEvent(unicode, keycode, mods, ispressed, isautorpt);
            ////qDebug("%s", (const char*)debugText.toAscii());
        }
        else
        {
            ////qDebug("Shift, Alt or unknown key.. not sent downstream") ;
        }
    }

    k3_sn->setEnabled(true);
}

void KindleKeyboard::activity(int)
{
    int unicode, keycode ;
    Qt::KeyboardModifiers mods ;
    bool ispressed, isautorpt ;


    _sn->setEnabled(false);

    input_event_t in;

    read(_fd, &in, sizeof(input_event_t));

    if (in.type == 1)
    {
        if (_debug)
        {
            QString debugText = QString("Keyboard: type %1, code %2, value %3").arg(in.type).arg(in.code).arg(in.value) ;

            if (_shift)
                debugText += " Shift" ;
            if (_alt)
                debugText += " Alt" ;


            qDebug("%s", (const char*)debugText.toAscii());
        }

        // special alt/shift and tripleShift processing
        if (in.value == 1)  // key pressed
        {
            if (in.code != KDX_KEY_SHIFT)
                _nshifts = 0 ;

            if (in.code == KDX_KEY_ALT)
            {
                alt_just_pressed = true ;
                altshift_just_pressed = false ;
            } else if (in.code == KDX_KEY_SHIFT)
            {
                if (alt_just_pressed)
                {
                    altshift_just_pressed = true ;
                }
                else
                    alt_just_pressed = false ;
            } else
            {
                alt_just_pressed = false ;
                altshift_just_pressed = false ;
            }
        } else if (in.value == 0)   // key released
        {
            if (in.code == KDX_KEY_SHIFT)
            {
                if (!altshift_just_pressed)
                {
                    alt_just_pressed = false ;
                    altshift_just_pressed = false ;
                }
            }
            else if (in.code == KDX_KEY_ALT)
            {
                if (altshift_just_pressed) // full Alt/Shift combination, go and switch virt screens
                {
                    if (_debug)
                        qDebug("--Alt/Shift") ;

                    if (is_qt_app_frozen)
                    {
                        capture_input() ;
                        is_qt_app_frozen = false ;

                        _shift = false ;
                        _alt = false ;
                        _altgray = false ;

                        unicode = 0 ;
                        keycode = Qt::Key_WakeUp ;
                        mods = GetCurModifiers() ;
                        ispressed = 1 ;
                        isautorpt = 0 ;

                        ///system("kill -STP `pidof cvm`") ;
                        suspend_cvm() ;

                        if (_debug)
                            qDebug("---unfreeze Qt App") ;
                    }
                    else
                    {
                        release_input() ;
                        is_qt_app_frozen = true ;

                        unicode = 0 ;
                        keycode = Qt::Key_Sleep ;
                        mods = GetCurModifiers() ;
                        ispressed = 1 ;
                        isautorpt = 0 ;

                        resume_cvm(k3_fd != -1) ;
                        /*
                        QString cmdText ;

                        if (k3_fd != -1)
                            cmdText = QString("kill -CONT `pidof cvm`; echo send %1 >/proc/keypad").arg(K3_KEY_HOME) ;
                        else
                            cmdText = QString("kill -CONT `pidof cvm`; echo send %1 >/proc/keypad").arg(KDX_KEY_HOME) ;

                        system(cmdText.toAscii()) ;
*/
                        if (_debug)
                            qDebug("---freeze Qt App") ;
                    }

                    processKeyEvent(0, in.code, 0, 0, 0);
                    processKeyEvent(unicode, keycode, mods, ispressed, isautorpt);

                    _sn->setEnabled(true);

                    return ;
                }
            } else
            {
                altshift_just_pressed = false ;
                alt_just_pressed = false ;
            }
        }
        else
        {
            altshift_just_pressed = false ;
            alt_just_pressed = false ;
        }

        switch(in.code)
        {
            // Alt+Row1
            case KDX_KEY_1:
            case KDX_KEY_2:
            case KDX_KEY_3:
            case KDX_KEY_4:
            case KDX_KEY_5:
            case KDX_KEY_6:
            case KDX_KEY_7:
            case KDX_KEY_8:
            case KDX_KEY_9:
                unicode = '0' - 1 + in.code ;
                keycode =  Qt::Key_0 - 1 + in.code ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_0:
                unicode = '0';
                keycode =  Qt::Key_0 ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
            break;

            // Row1
            case KDX_KEY_Q:
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'Q' : 'q' ;
                keycode = Qt::Key_Q ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_W:
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'W' : 'w' ;
                keycode = Qt::Key_W ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_E:
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'E' : 'e' ;
                keycode = Qt::Key_E ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_R:
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'R' : 'r' ;
                keycode = Qt::Key_R ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_T:
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'T' : 't' ;
                keycode = Qt::Key_T ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_Y:
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'Y' : 'y' ;
                keycode = Qt::Key_Y ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_U:
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'U' : 'u' ;
                keycode = Qt::Key_U ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_I:
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'I' : 'i' ;
                keycode = Qt::Key_I ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_O:
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'O' : 'o' ;
                keycode = Qt::Key_O ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_P:
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'P' : 'p' ;
                keycode = Qt::Key_P ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;

            // Row2
            case KDX_KEY_A: // row 2
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'A' : 'a' ;
                keycode = Qt::Key_A ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_S: // row 2
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'S' : 's' ;
                keycode = Qt::Key_S ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_D: // row 2
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'D' : 'd' ;
                keycode = Qt::Key_D ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_F: // row 2
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'F' : 'f' ;
                keycode = Qt::Key_F ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_G: // row 2
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'G' : 'g' ;
                keycode = Qt::Key_G ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_H: // row 2
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'H' : 'h' ;
                keycode = Qt::Key_H ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_J: // row 2
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'J' : 'j' ;
                keycode = Qt::Key_J ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_K: // row 2
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'K' : 'k' ;
                keycode = Qt::Key_K ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_L: // row 2
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'L' : 'l' ;
                keycode = Qt::Key_L ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_DEL: // row 2
                unicode = 0 ;
                keycode = Qt::Key_Backspace ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;

            // Row3
            case KDX_KEY_Z: // row 3
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'Z' : 'z' ;
                keycode = Qt::Key_Z ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_X: // row 3
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'X' : 'x' ;
                keycode = Qt::Key_X ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_C: // row 3
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'C' : 'c' ;
                keycode = Qt::Key_C ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_V: // row 3
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'V' : 'v' ;
                keycode = Qt::Key_V ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_B: // row 3
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'B' : 'b' ;
                keycode = Qt::Key_B ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_N: // row 3
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'N' : 'n' ;
                keycode = Qt::Key_N ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_M: // row 3
                unicode = ((_shift==TRUE) || (_alt==TRUE)) ? 'M' : 'm' ;
                keycode = Qt::Key_M ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_DOT: // row 3
                unicode = '.' ;
                keycode = Qt::Key_Period ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_SLASH: // row 3
                unicode = '/' ;
                keycode = Qt::Key_Slash ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_ENTER: // row 3
                unicode = '\n' ;
                keycode = Qt::Key_Return ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;

            // Row4
            case KDX_KEY_SHIFT:
                ++_nshifts ;
                unicode = 0 ;
                keycode = Qt::Key_Shift ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;

                if (_nshifts >= 3 )
                {
                    do_screenshot((char *) "/mnt/us/screenshot.bmp") ;
                    _nshifts = 0 ;
                    _shift = false ;
                    break ;
                }
                _shift = (in.value != 0) ;
                mods = GetCurModifiers() ;
                //processKeyEvent(0, Qt::Key_Shift, _alt ? Qt::AltModifier : Qt::NoModifier, in.value != 0, in.value == 2);
                break;
            case KDX_KEY_ALT:
                _alt = (in.value != 0);
                unicode = 0 ;
                keycode = Qt::Key_Alt ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_SPACE: // space
                unicode = ' ' ;
                keycode = Qt::Key_Space ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_AA: // Aa -- might be used to switch between charsets...
            case K3_KEY_AA:
                unicode = 0 ;
                keycode = Qt::Key_Mode_switch;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_SYM: // SYM
            case K3_KEY_SYM:
            case K4_KEY_KBD:
                _altgray = (in.value != 0);
                unicode = 0 ;
                keycode = Qt::Key_AltGr ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
            break;

            // Edge
            case KDX_KEY_VPLUS: // Vol+
                unicode = '+' ;
                keycode = Qt::Key_Plus ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_VMINUS: // Vol-
                unicode = '-' ;
                keycode = Qt::Key_Minus ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_HOME: // home
            case K3_KEY_HOME:
                unicode = 0 ;
                keycode = Qt::Key_Home ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_PAGEUP: // prev
            case K3_KEY_LPAGEUP:
                unicode = 0 ;
                keycode = Qt::Key_PageUp ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_PAGEDN: // next
            case K3_KEY_PAGEDN:
            case K3_KEY_LPAGEDN: // next on left side of kindle 2
                unicode = 0 ;
                keycode = Qt::Key_PageDown ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_MENU: // menu
                unicode = 0 ;
                keycode = Qt::Key_Menu ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;
            case KDX_KEY_BACK: // back
            case K3_KEY_BACK:
                unicode = 0 ;
                keycode = Qt::Key_Escape ;
                mods = GetCurModifiers() ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                break;

            default:
                unicode = -1 ;
                keycode = -1 ;
                ispressed = (in.value !=  0) ; isautorpt = (in.value == 2) ;
                if (_debug)
                {
                    QString debugText = QString("***Unknown: type %1, code %2, value %3").arg(in.type).arg(in.code).arg(in.value);
                    qDebug("%s", (const char*)debugText.toAscii());
                    //logKey(debugText);
                }
                break;
        }

        if ((is_qt_app_frozen == false) && (unicode != -1))
        {
            if (_debug)
            {
                QString debugText = QString("Key:%1, Unicode: %2").arg(keycode).arg(unicode);
                QString smods = "" ;
                qDebug("%s", (const char*)debugText.toAscii());

                if (mods == Qt::NoModifier)
                    smods = "none" ;
                else
                {
                    if (mods & Qt::AltModifier)
                        smods = smods + "Alt" ;
                    if (mods & Qt::ShiftModifier)
                        smods = smods + " Shift" ;
                }

                debugText = QString("***Mods:%1, pressed: %2, autorpt: %3").arg(smods).arg(ispressed).arg(isautorpt);
                qDebug("%s", (const char*)debugText.toAscii());
            }

            processKeyEvent(unicode, keycode, mods, ispressed, isautorpt);
        }
        else
        {
            ////qDebug("Shift, Alt or unknown key.. not sent downstream") ;
        }
/***
        {
        }
***/
    }

    _sn->setEnabled(true);
}

