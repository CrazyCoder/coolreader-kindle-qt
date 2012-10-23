#ifndef TSPLUGIN_H
#define TSPLUGIN_H

#include <QMouseDriverPlugin>
#include <QWSMouseHandler>

class TSPlugin : public QMouseDriverPlugin {
    Q_OBJECT
public:
    TSPlugin(QObject* parent = 0);

    QStringList keys() const;
    QWSMouseHandler* create(const QString & key, const QString & device);
};

#endif // TSPLUGIN_H
