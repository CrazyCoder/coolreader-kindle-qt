#include "device.h"

const Device::Properties Device::PROPS[] = {
//    x    y    KBD    JOY
    {600, 800,  false, false},    // UNKNOWN
    {600, 800,  true,  true },    // EMULATOR
    {600, 800,  true,  true },    // K2
    {824, 1200, true,  true },    // KDX
    {600, 800,  true,  true },    // K3
    {600, 800,  false, true },    // K4NT
    {600, 800,  false, true },    // K4NTB
    {600, 800,  false, false},    // KT
    {758, 1024, false, false},    // KPW
};

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
        case 0xB01D:
        case 0xB024:
            m_model = KPW;
            break;
        default:
            qDebug("Unknown model: %X", sn);
        }
    }
}
