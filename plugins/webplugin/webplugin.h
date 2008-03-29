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

class WebPlugin : public QObject, public IPluginSyntaxHighlighter, public IPluginPrettyPrint, public IPluginExtendedEditor {
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin)
	Q_INTERFACES(IFilePlugin)
	Q_INTERFACES(IPluginSyntaxHighlighter)
	Q_INTERFACES(IPluginPrettyPrint)
	Q_INTERFACES(IPluginExtendedEditor)
public:
	WebPlugin();
	
	virtual bool initializePlugin( const QString & lang );
	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr );
	
	virtual QStringList extentions();
	virtual QHash<QString,QString> extentionsDescription();
	virtual QIcon icon( const QString & extention );
	
	virtual QStringList highlighters();
	virtual QString highlighterOfExtention( const QString & extention );
	virtual QHash<QString,QTextCharFormat> formatOfHighlighter( const QString & highlighter );
	virtual QString exampleOfHighlighter( const QString & highlighter );
	virtual void highlightBlock( const QString & highlighter, const QHash<QString,QTextCharFormat> & formats, IXinxSyntaxHighlighter * i, const QString& text );

	virtual QStringList prettyPrinters();
	virtual QString prettyPrinterOfExtention( const QString & extention );
	virtual QString prettyPrint( const QString & plugin, const QString & text, QString * errorStr, int * line, int * column );
	
	virtual QStringList extendedEditors();
	virtual QString extendedEditorOfExtention( const QString & extention );
	virtual void commentSelectedText( const QString & plugin, IXinxExtendedEditor * editor, bool uncomment );
	virtual FileContentElement * createModelData( const QString & plugin, IXinxExtendedEditor * editor, FileContentElement * parent, const QString & filename, int line );
	virtual void createCompleter( const QString & plugin, IXinxExtendedEditor * editor );
	virtual QCompleter * completer( const QString & plugin, IXinxExtendedEditor * editor );
	virtual bool keyPress( const QString & plugin, IXinxExtendedEditor * editor, QKeyEvent * event );
	virtual QPair<QString,int> searchWord( const QString & plugin, IXinxExtendedEditor * editor, const QString & word );
};

#endif /* WEBPLUGIN_H_*/
