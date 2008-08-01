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

#ifndef XSLTEXTEDITOR_H_
#define XSLTEXTEDITOR_H_

// Xinx header
#include "xmltexteditor.h"

class XslContentElementList;

class XslTextEditor : public XmlTextEditor {
	Q_OBJECT
public:
	XslTextEditor( QWidget * parent = 0 );
	virtual ~XslTextEditor();

	virtual QCompleter * completer();

	virtual int insertCompletionBalises( QTextCursor & tc, QString node );
	virtual void insertCompletionAccolade( QTextCursor & tc, QString node, QString param, QString value, const QModelIndex & index );

	void setParser( XslContentElementList * parser );
private:
	QString paramValue( const QTextCursor & cursor, const QString & param );
	void getTemplate( const QTextCursor & cursor, QString * name, QString * mode );

	XslContentElementList * m_parser;
};

#endif /*XSLTEXTEDITOR_H_*/
