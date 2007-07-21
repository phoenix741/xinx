/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -p studiointerface com.generix.xmlstudio.xml
 *
 * dbusxml2cpp is Copyright (C) 2006 Trolltech ASA. All rights reserved.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef STUDIOINTERFACE_H_1185032083
#define STUDIOINTERFACE_H_1185032083

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.generix.xmlstudio
 */
class ComGenerixXmlstudioInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.generix.xmlstudio"; }

public:
    ComGenerixXmlstudioInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~ComGenerixXmlstudioInterface();

public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
    void open(const QString &fileName);
    void openNew();
    void openProject(const QString &fileName);
    void saveAll();
};

namespace com {
  namespace generix {
    typedef ::ComGenerixXmlstudioInterface xmlstudio;
  }
}
#endif
