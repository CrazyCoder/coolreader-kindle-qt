// keyboardplugin.h
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

#ifndef KEYBOARDPLUGIN_H
#define KEYBOARDPLUGIN_H

#include <QKbdDriverPlugin>
#include <QWSKeyboardHandler>

class KeyboardPlugin : public QKbdDriverPlugin {
    Q_OBJECT
public:
    KeyboardPlugin(QObject* parent = 0);

    QStringList keys() const;
    QWSKeyboardHandler* create(const QString & key, const QString & device);
};


#endif // KEYBOARDPLUGIN_H
