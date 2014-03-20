#include "device.h"
#include <QFile>

#ifndef i386
#include "../../../drivers/QKindleFb/qkindlefb.h"
#endif

const Device::Properties Device::PROPS[] = {
//    x    y    DPI, KBD    JOY
    {600, 800,  167, false, false},    // UNKNOWN
    {600, 800,  167, true,  true },    // EMULATOR
    {600, 800,  167, true,  true },    // K2
    {824, 1200, 152, true,  true },    // KDX
    {600, 800,  167, true,  true },    // K3
    {600, 800,  167, false, true },    // K4NT
    {600, 800,  167, false, true },    // K4NTB
    {600, 800,  167, false, false},    // KT
    {758, 1024, 212, false, false},    // KPW
    {758, 1024, 212, false, false},    // KPW2
};

Device::Model Device::m_model = UNKNOWN;

Device::Device()
{
#ifdef i386
    m_model = EMULATOR;
    return;
#endif
    QFile file("/proc/usid");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open /proc/usid";
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();

    if (!line.isNull()) {
        bool ok;
        int sn = line.left(4).toInt(&ok, 16);

        if (ok) {
            qDebug("serial: %X", sn);
        } else {
            qDebug() << "unexpected output: " << line;
            file.close();
            return;
        }

        switch(sn) {
        case 0xB002:
        case 0xB003:
            m_model = K2; // may not work as K2 doesn't print SN in cpuinfo
            break;
        case 0xB004:
        case 0xB005:
        case 0xB009:
            m_model = KDX;
            break;
        case 0xB006:
        case 0xB008:
        case 0xB00A:
            m_model = K3;
            break;
        case 0xB00E:
            m_model = K4NT;
            break;
        case 0xB00F:
        case 0xB010:
        case 0xB011:
        case 0xB012: // ???
            m_model = KT;
            break;
        case 0x9023:
        case 0xB023:
            m_model = K4NTB;
            break;
        case 0xB01B:
        case 0xB01C:
        case 0xB01D:
        case 0xB01F:
        case 0xB020:
        case 0xB024:
            m_model = KPW;
            break;
        case 0x90D4:
        case 0xB0D4:
        case 0xB0D5:
        case 0xB0D6:
        case 0x90D7:
        case 0xB0D7:
        case 0xB0D8:
        case 0x905A:
        case 0xB05A:
        case 0xB0F2:
            m_model = KPW2;
            break;
        default:
            qDebug("Unknown model: %X", sn);
        }
    }

    file.close();
}

Device &Device::instance()
{
    static Device instance;
    // qDebug("DEVICE: %d (%d x %d @ %d) [%d|%d]", getModel(), getWidth(), getHeight(), getDpi(), hasKeyboard(), hasFiveWay());
    return instance;
}

void Device::suspendFramework(bool fast)
{
#ifndef i386
    qDebug("- framework");
    if (!isTouch()) {
        // this pause lets CVM handle painting before stopping, or screensaver may not draw
        // on next resume when device is turned off
        sleep(1);
        QProcess::execute("killall -STOP cvm");
    } else {
        if (!fast) QProcess::execute(QString("/bin/sh /var/tmp/ktsuspend.sh"));
        else sleep(1);
    }
    QWSServer::instance()->enablePainting(true);
#endif
}

void Device::resumeFramework(bool fast)
{
#ifndef i386
    qDebug("+ framework");
    QWSServer::instance()->enablePainting(false);
    if (!isTouch()) {
        QProcess::execute("killall -CONT cvm");
    } else if (!fast) {
        QProcess::execute(QString("/bin/sh /var/tmp/ktresume.sh"));
    }
#endif
}

void Device::enableInput(bool enable)
{
    if (enable) {
        isTouch() ? QWSServer::instance()->openMouse() : QWSServer::instance()->openKeyboard();
    } else {
        isTouch() ? QWSServer::instance()->closeMouse() : QWSServer::instance()->closeKeyboard();
    }
}

void Device::setFullScreenUpdateEvery(int n)
{
#ifndef i386
    QKindleFb *pscreen = static_cast<QKindleFb*>(QScreen::instance());
    if (pscreen) pscreen->setFullUpdateEvery(n);
#endif
}

void Device::forceFullScreenUpdate(bool fullScreen)
{
#ifndef i386
    QKindleFb *pscreen = static_cast<QKindleFb*>(QScreen::instance());
    if (pscreen) pscreen->forceFullUpdate(fullScreen);
#endif
}
