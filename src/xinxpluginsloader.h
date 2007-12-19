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
#include "interfaces.h"

// Qt header
#include <QList>
#include <QDir>
#include <QObject>

class XinxPluginsLoader {
public:
	XinxPluginsLoader();
	~XinxPluginsLoader();
	
	void loadPlugins();
	
	const QDir & pluginsDir() const;
	const QStringList & pluginFileNames() const;
	
	const QList<SyntaxHighlighterInterface*> & syntaxPlugins() const; 

	
	QString filter( const QString & suffix );
	QStringList filters();
private:
	void addPlugin( QObject * plugin );
	
	QList<SyntaxHighlighterInterface*> m_syntaxPlugins;
	
	QDir m_pluginsDir;
	QStringList m_pluginFileNames;
};

#endif /*XINXPLUGINSLOADER_H_*/
