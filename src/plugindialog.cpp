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

// Xinx header
#include "plugindialog.h"
#include "globals.h"
#include "xinxpluginsloader.h"

// Qt header
#include <QPluginLoader>
#include <QTreeWidgetItem>
#include <QHeaderView>

PluginDialogImpl::PluginDialogImpl(QWidget *parent) : QDialog(parent) {
	setupUi( this );
	m_pluginsListWidget->header()->hide();
	
    interfaceIcon.addPixmap( style()->standardPixmap(QStyle::SP_DirOpenIcon), QIcon::Normal, QIcon::On );
    interfaceIcon.addPixmap( style()->standardPixmap(QStyle::SP_DirClosedIcon), QIcon::Normal, QIcon::Off);
    featureIcon.addPixmap( style()->standardPixmap(QStyle::SP_FileIcon) );
	
	findPlugins( global.m_pluginsLoader->pluginsDir().absolutePath(), global.m_pluginsLoader->pluginFileNames() );
}

PluginDialogImpl::~PluginDialogImpl() {

}

void PluginDialogImpl::addItems( QTreeWidgetItem *pluginItem, const QString & interfaceName, const QStringList &features ) {
    QTreeWidgetItem * interfaceItem = new QTreeWidgetItem( pluginItem );
    interfaceItem->setText( 0, interfaceName );
    interfaceItem->setIcon( 0, interfaceIcon );

    foreach( QString feature, features ) {
        if( feature.endsWith("...") )
            feature.chop(3);
        QTreeWidgetItem * featureItem = new QTreeWidgetItem( interfaceItem );
        featureItem->setText( 0, feature );
        featureItem->setIcon( 0, featureIcon );
    }
}

void PluginDialogImpl::populateTreeWidget( QObject *plugin, const QString &text ) {
	QTreeWidgetItem * pluginItem = new QTreeWidgetItem( m_pluginsListWidget );
	pluginItem->setText( 0, text );
	m_pluginsListWidget->setItemExpanded( pluginItem, true );
	
	QFont boldFont = pluginItem->font( 0 );
	boldFont.setBold( true );
	pluginItem->setFont( 0, boldFont );
	
	if( plugin ) {
		IPluginSyntaxHighlighter * iSyntaxHighlighter = qobject_cast<IPluginSyntaxHighlighter*>( plugin );
		if( iSyntaxHighlighter )
			addItems( pluginItem, tr("IPluginSyntaxHighlighter"), iSyntaxHighlighter->descriptionOfPlugins().values() );
	}
}


void PluginDialogImpl::findPlugins( const QString &path, const QStringList &fileNames ) {
	m_dirLabel->setText( QString( m_dirLabel->text() ).arg( QDir::toNativeSeparators( path ) ) );
	
	const QDir dir( path );
	
	foreach( QObject * plugin, QPluginLoader::staticInstances() ) {
		populateTreeWidget( plugin, tr("%1 (Static Plugin)").arg( plugin->metaObject()->className() ) );
	}
	
	foreach( QString filename, fileNames ) {
		QPluginLoader loader( dir.absoluteFilePath( filename ) );
		QObject * plugin = loader.instance();
		if( plugin )
			populateTreeWidget( plugin, filename );
	}
}
