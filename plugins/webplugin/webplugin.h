/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

#ifndef WEBPLUGIN_H_
#define WEBPLUGIN_H_

// Xinx header
#include <plugininterfaces.h>

class WebPlugin : public QObject, public IFilePlugin, public IPluginSyntaxHighlighter {
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin)
	Q_INTERFACES(IFilePlugin)
	Q_INTERFACES(IPluginSyntaxHighlighter)
public:
	WebPlugin();
	virtual ~WebPlugin();

	virtual bool initializePlugin( const QString & lang );
	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr );

	virtual QList<IFileTypePlugin*> fileTypes();

	virtual QStringList highlighters();
	virtual QString highlighterOfExtention( const QString & extention );
	virtual QHash<QString,QTextCharFormat> formatOfHighlighter( const QString & highlighter );
	virtual QString exampleOfHighlighter( const QString & highlighter );
	virtual SyntaxHighlighter * createHighlighter( const QString & highlighter, QTextDocument* parent, XINXConfig * config );
private:
	QList<IFileTypePlugin*> m_fileTypes;
};

#endif /* WEBPLUGIN_H_*/
