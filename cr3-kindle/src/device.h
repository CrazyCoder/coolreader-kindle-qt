#ifndef DEVICE_H
#define DEVICE_H

#include <QProcess>
#include <QDebug>
#include <QWSServer>

class Device
{
public:
    class Properties {
    public:
        int width;
        int height;
        bool hasKeyboard;
        bool hasFiveWay;
    };

    static Device& instance()
    {
        static Device instance;
        return instance;
    }

    typedef enum MODEL {
        UNKNOWN = 0,
        EMULATOR, // qvfb qws environment
        K2,       // Kindle 2
        KDX,      // Kindle DX(G)
        K3,       // Kindle Keyboard
        K4NT,     // Kindle Non-Touch Silver 2011
        K4NTB,    // Kindle Non-Touch Black 2012
        KT,       // Kindle Touch
        KPW       // Kindle PaperWhite
    } Model;

    static Model getModel() {
        return m_model;
    }

    static int getWidth() {
        return PROPS[m_model].width;
    }

    static int getHeight() {
        return PROPS[m_model].height;
    }

    static bool hasKeyboard() {
        return PROPS[m_model].hasKeyboard;
    }

    static bool hasFiveWay() {
        return PROPS[m_model].hasFiveWay;
    }

    static bool isTouch() {
        return !hasKeyboard() && !hasFiveWay();
    }

    static bool hasLight() {
        return m_model == KPW;
    }

    static bool isEmulator() { return m_model == EMULATOR; }

    static void suspendFramework(bool fast = false) {
#ifndef i386
        qDebug("- framework");
        if (!isTouch()) {
            // this pause lets CVM handle painting before stopping, or screensaver may not draw
            // on next resume when device is turned off
            sleep(1);
            QProcess::execute("killall -STOP cvm");
        } else {
            QProcess::execute(QString("/bin/sh ./ktsuspend.sh %1").arg(fast ? 1 : 0));
        }
        QWSServer::instance()->enablePainting(true);
#endif
    }

    static void resumeFramework(bool fast = false) {
#ifndef i386
        qDebug("+ framework");
        QWSServer::instance()->enablePainting(false);
        if (!isTouch()) {
            QProcess::execute("killall -CONT cvm");
        } else {
            QProcess::execute(QString("/bin/sh ./ktresume.sh %1").arg(fast ? 1 : 0));
        }
#endif
    }

private:
    static const Properties PROPS[];
    static MODEL m_model;

    Device();
    Device(Device const&);
    void operator=(Device const&);
};

#endif // DEVICE_H
