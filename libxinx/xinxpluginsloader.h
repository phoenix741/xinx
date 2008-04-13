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
#include "xinxcore.h"
#include <plugininterfaces.h>

// Qt header
#include <QHash>
#include <QDir>
#include <QObject>
#include <QIcon>
#include <QPair>

class RCS;

/*!
 * The class XINX Plugins Loader is used to load all plugins and proposed some facilities method.
 */
class XINX_EXPORT XinxPluginsLoader : public QObject {
	Q_OBJECT
public:
	XinxPluginsLoader();
	~XinxPluginsLoader();
	
	/*!
	 * Load the plugins
	 */
	void loadPlugins();
	/*! 
	 * List all the loaded plugins
	 */
	const QList<XinxPluginElement> & plugins() const;
	/*! 
	 * List all the revision control that can be used. The Result is a list of
	 * QPair. The first element is the key and the second element is the description.
	 */
	QList< QPair<QString,QString> > revisionsControls() const;
	RCS * createRevisionControl( QString revision, QString basePath ) const;
	
	/*!
	 * List all highlighter that can be used.
	 */
	QStringList highlighters() const;
	QString highlighterOfSuffix( const QString & suffix ) const;
	QHash<QString,QTextCharFormat> formatOfHighlighter( const QString & highlighter );
	QString exampleOfHighlighter( const QString & highlighter );
	SyntaxHighlighter * createHighlighter( const QString & highlighter, QObject* parent = NULL, XINXConfig * config = NULL );
	SyntaxHighlighter * createHighlighter( const QString & highlighter, QTextDocument* parent, XINXConfig * config = NULL );
	SyntaxHighlighter * createHighlighter( const QString & highlighter, QTextEdit* parent, XINXConfig * config = NULL );
	
	QIcon iconOfSuffix( const QString & suffix ) const;

	const QHash<QString,QString> & extentions() const;
	QString filter( const QString & suffix ) const;
	const QStringList & filters() const;
	const QStringList & defaultProjectFilter() const;
	QStringList defaultProjectFilter( const QString & name ) const;

	QPair<IPluginPrettyPrint*,QString> prettyPrinterOfSuffix( const QString & suffix ) const;
	QPair<IPluginExtendedEditor*,QString> extendedEditorOfSuffix( const QString & suffix ) const;

	QStringList prettyPrinterOfPlugins() const;
	QStringList extendedEditorOfPlugins() const;
	
	IPluginPrettyPrint* prettyPrinterOfPlugin( const QString & suffix ) const;
	IPluginExtendedEditor* extendedEditorOfPlugin( const QString & suffix ) const;

	static XinxPluginsLoader * self();
private:
	void addPlugin( QObject * plugin, bool staticLoaded = false );
	void addPlugin( QString extention, QObject * plugin );
	
	QDir m_pluginsDir;
	QList< XinxPluginElement > m_plugins;

	QHash<QString,QIcon> m_icons;

	QHash<QString,int> m_filterIndex;
	QStringList m_filters, m_defaultProjectFilter;
	
	QHash<QString,QString> m_libelles;
	
	QHash< QString,QPair<IPluginPrettyPrint*,QString> > m_prettyPlugins;
	QHash< QString,QPair<IPluginExtendedEditor*,QString> > m_extendedEditorPlugins;
	
	QHash< QString,IPluginPrettyPrint* > m_directPrettyPlugins;
	QHash< QString,IPluginExtendedEditor* > m_directExtendedEditorPlugins;
	
	static XinxPluginsLoader * s_self;
};

#endif /*XINXPLUGINSLOADER_H_*/
