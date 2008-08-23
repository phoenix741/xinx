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
#include "xinxpluginelement.h"

// Qt header
#include <QHash>
#include <QDir>
#include <QObject>
#include <QIcon>
#include <QPair>

class RCS;

/*! The class XINX Plugins Loader is used to load all plugins and proposed some facilities method. */
class  XinxPluginsLoader : public QObject {
	Q_OBJECT
public:
	~XinxPluginsLoader();

	/*! Load the plugins */
	void loadPlugins();
	/*! List all the loaded plugins */
	QList<XinxPluginElement*> plugins() const;
	/*! Plugin of name \e name. */
	XinxPluginElement * plugin( const QString & name );
	/*!
	 * List all the revision control that can be used. The Result is a list of
	 * QPair. The first element is the key and the second element is the description.
	 */
	QList< QPair<QString,QString> > revisionsControls() const;
	/*!
	 * Method called by XINX to create a Revision control system.
	 * \param revision The system to use.
	 * \param basePath The path used in the constructor.
	 */
	RCS * createRevisionControl( QString revision, QString basePath ) const;

	/*! List all highlighter that can be used. */
	QStringList highlighters() const;
	/*! Select the highlighter for the corresponding suffix */
	QString highlighterOfSuffix( const QString & suffix ) const;
	/*! Defaut format of the highlighter (used by the configuration manager) */
	QHash<QString,QTextCharFormat> formatOfHighlighter( const QString & highlighter );
	/*! Exemple of highlighter */
	QString exampleOfHighlighter( const QString & highlighter );
	/*! Create a highlighter for the texte document */
	SyntaxHighlighter * createHighlighter( const QString & highlighter, QTextDocument* parent, XINXConfig * config = NULL );

	/*! Return all the file type knew by XINX. */
	QList<IFileTypePlugin*> fileTypes() const;
	/*! Search the file type for the corresponding filename  */
	IFileTypePlugin * matchedFileType( const QString & filename ) const;
	/*! Return the filter for a given file type */
	static QString fileTypeFilter( IFileTypePlugin * fileType );
	//! Return a list of filter that can be used open dialog box
	QStringList openDialogBoxFilters() const;
	//! Return a list of filter
	QStringList managedFilters() const;

	//! Create an element (and also a parser) for the given filename
	FileContentElement * createElement( QString & filename, FileContentElement * parent = 0, int line = -1 );

	/*! Create a plugin loader object */
	static XinxPluginsLoader * self();
private:
	XinxPluginsLoader();

	void addPlugin( QObject * plugin, bool staticLoaded = false );
	QString allManagedFileFilter() const;

	QDir m_pluginsDir;
	QMap<QString, XinxPluginElement*> m_plugins;

	static XinxPluginsLoader * s_self;
};

Q_DECLARE_METATYPE( IFileTypePlugin* );

#endif /*XINXPLUGINSLOADER_H_*/
