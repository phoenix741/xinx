/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

// Xinx header
#include "ui_plugindialog.h"

// Qt header
#include <QDialog>
#include <QIcon>

class QStringList;
class QTreeWidget;
class QTreeWidgetItem;

class PluginDialogImpl : public QDialog, private Ui::PluginDialogClass {
	Q_OBJECT
public:
	PluginDialogImpl( QWidget *parent = 0 );
    ~PluginDialogImpl();
   
private:
	void findPlugins( const QString &path, const QStringList &fileNames );
    void populateTreeWidget( QObject *plugin, const QString &text );
    void addItems( QTreeWidgetItem *pluginItem, const QString & interfaceName, const QStringList &features );
    QIcon interfaceIcon, featureIcon;
};

#endif // PLUGINDIALOG_H
