// fivewayplugin.h
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

#ifndef FIVEWAYPLUGIN_H
#define FIVEWAYPLUGIN_H

#include <QMouseDriverPlugin>
#include <QWSMouseHandler>

class FiveWayPlugin : public QMouseDriverPlugin {
    Q_OBJECT
public:
    FiveWayPlugin(QObject* parent = 0);

    QStringList keys() const;
    QWSMouseHandler* create(const QString & key, const QString & device);
};


#endif // FIVEWAYPLUGIN_H
