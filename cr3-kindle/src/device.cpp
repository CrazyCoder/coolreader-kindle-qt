#include "device.h"

//                                    ???    EMU   K2    KDX   K3    K4NT   K4NTB  KT     KPW
const int  Device::WIDTH[]         = {600,   600,  600,  824,  600,  600,   600,   600,   758};
const int  Device::HEIGHT[]        = {800,   800,  800,  1200, 800,  800,   800,   800,   1024};
const bool Device::KEYBOARD[]      = {false, true, true, true, true, false, false, false, false};
const bool Device::FIVE_WAY[]      = {false, true, true, true, true, true,  true,  false, false};

Device::Model Device::m_model = UNKNOWN;

Device::Device()
{
#ifdef i386
    m_model = EMULATOR;
    return;
#endif
    QStringList list;
    QProcess *myProcess = new QProcess();

    list << "-c" << "grep erial /proc/cpuinfo|cut -c12-15";
    myProcess->start("/bin/sh", list);
    if (myProcess->waitForReadyRead(10000)) {
        QByteArray array = myProcess->readAll();
        array.truncate(array.indexOf("\n"));

        bool ok;
        int sn = QString(array).toInt(&ok, 16);

        if (ok) {
            qDebug("serial: %X", sn);
        } else {
            qDebug() << "unexpected output: " << QString(array);
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
        case 0xB023:
            m_model = K4NTB;
            break;
        case 0xB01B:
        case 0xB024:
            m_model = KPW;
            break;
        default:
            qDebug("Unknown model: %X", sn);
        }
    }
}
