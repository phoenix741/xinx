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
#include "baseplugin.h"

// Qt header
#include <QStringList>

/* XmlPlugin */

QStringList BasePlugin::highlighters() {
	return QStringList() << tr("XML Highlighter") << tr("JS Highlighter") << tr("CSS Highlighter");
}

QString BasePlugin::filterOf( const QString & highlighter ) {
	return QString();
}

SyntaxHighlighter * BasePlugin::newSyntaxHighlighter( QObject* parent, XINXConfig * config ) {
	return NULL;
}

SyntaxHighlighter * BasePlugin::newSyntaxHighlighter( QTextDocument* parent, XINXConfig * config ) {
	return NULL;
}

SyntaxHighlighter * BasePlugin::newSyntaxHighlighter( QTextEdit* parent, XINXConfig * config ) {
	return NULL;
}

Q_EXPORT_PLUGIN2(xinx_baseplugin, BasePlugin)
