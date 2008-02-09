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

#ifndef XINXPLUGINSLOADER_H_
#define XINXPLUGINSLOADER_H_

// Xinx header
#include <plugininterfaces.h>

// Qt header
#include <QHash>
#include <QDir>
#include <QObject>
#include <QIcon>
#include <QPair>


class XinxPluginsLoader : public QObject {
	Q_OBJECT
public:
	XinxPluginsLoader();
	~XinxPluginsLoader();
	
	static XinxPluginsLoader * self();
	
	void loadPlugins();
	
	const QDir & pluginsDir() const;
	const QStringList & pluginFileNames() const;
	
	QIcon iconOfSuffix( const QString & suffix ) const;

	const QHash<QString,QString> & extentions() const;
	QString filter( const QString & suffix ) const;
	const QStringList & filters() const;
	const QStringList & defaultProjectFilter() const;
	QStringList defaultProjectFilter( const QString & name ) const;

	QPair<IPluginSyntaxHighlighter*,QString> highlighterOfSuffix( const QString & suffix ) const;
	QPair<IPluginPrettyPrint*,QString> prettyPrinterOfSuffix( const QString & suffix ) const;
	QPair<IPluginExtendedEditor*,QString> extendedEditorOfSuffix( const QString & suffix ) const;

	QStringList highlighterOfPlugins() const;
	QStringList prettyPrinterOfPlugins() const;
	QStringList extendedEditorOfPlugins() const;
	
	IPluginSyntaxHighlighter* highlighterOfPlugin( const QString & suffix ) const;
	IPluginPrettyPrint* prettyPrinterOfPlugin( const QString & suffix ) const;
	IPluginExtendedEditor* extendedEditorOfPlugin( const QString & suffix ) const;
private:
	void addPlugin( QObject * plugin );
	void addPlugin( QString extention, QObject * plugin );
	
	QDir m_pluginsDir;
	QStringList m_pluginFileNames;

	QHash<QString,QIcon> m_icons;

	QHash<QString,int> m_filterIndex;
	QStringList m_filters, m_defaultProjectFilter;
	
	QHash<QString,QString> m_libelles;
	
	QHash< QString,QPair<IPluginSyntaxHighlighter*,QString> > m_syntaxPlugins;
	QHash< QString,QPair<IPluginPrettyPrint*,QString> > m_prettyPlugins;
	QHash< QString,QPair<IPluginExtendedEditor*,QString> > m_extendedEditorPlugins;
	
	QHash< QString,IPluginSyntaxHighlighter* > m_directSyntaxPlugins;
	QHash< QString,IPluginPrettyPrint* > m_directPrettyPlugins;
	QHash< QString,IPluginExtendedEditor* > m_directExtendedEditorPlugins;
	
	static XinxPluginsLoader * s_self;
};

#endif /*XINXPLUGINSLOADER_H_*/
