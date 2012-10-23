#include "tsplugin.h"
#include "kindlets.h"


TSPlugin::TSPlugin(QObject* parent) : QMouseDriverPlugin(parent)
{
    //    qDebug("TSPlugin()");
}

QStringList TSPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("KindleTS");
    return list;
}

QWSMouseHandler* TSPlugin::create(const QString & key, const QString & device)
{
    if (key.toLower() == QLatin1String("kindlets"))
    {
        if (device.contains("debug", Qt::CaseInsensitive))
            qDebug("TSPlugin::create() found!");
        return new KindleTS(key, device);
    }

    return 0;
}

Q_EXPORT_PLUGIN2(KindleFiveWay, TSPlugin)
