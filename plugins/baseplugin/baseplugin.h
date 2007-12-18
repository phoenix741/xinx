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

#ifndef BASEPLUGIN_H_
#define BASEPLUGIN_H_

// Xinx header
#include <interfaces.h>

class BasePlugin : public QObject, public SyntaxHighlighterInterface {
	Q_OBJECT
	Q_INTERFACES(SyntaxHighlighterInterface)
	
	virtual QStringList highlighters();
	virtual QHash<QString,QString> filterOfHighlighter( const QString & highlighter );
	virtual QHash<QString,QTextCharFormat> formatOfHighlighter( const QString & highlighter );
	virtual QString exampleOfHighlighter( const QString & highlighter );
};

#endif /* BASEPLUGIN_H_*/
