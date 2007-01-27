/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#ifndef __XMLEDITOR_H__
#define __XMLEDITOR_H__

#include "fileeditor.h"
#include <QTextCursor>

class QCompleter;
class QTextDocument;
class QTextEdit;

class XMLProcessor : public TextProcessor {
	Q_OBJECT
public:
	XMLProcessor( QTextEdit * widget, QObject * parent = 0 );
	virtual ~XMLProcessor();
	
	virtual void commentSelectedText( bool uncomment );
	virtual void complete();
	virtual void keyPressEvent( QKeyEvent *e );
	
protected slots:
	void insertCompletion(const QString& completion);

private:
	bool isCodeCommented( const QTextCursor & cursor ) const;
	bool isEditBalise( const QTextCursor & cursor ) const;
	bool isEditNode( const QTextCursor & cursor ) const;
	bool isEditParam( const QTextCursor & cursor ) const;
	bool isEditValue( const QTextCursor & cursor ) const;
   
	QString textUnderCursor(const QTextCursor & cursor) const;
	QString nodeName(const QTextCursor & cursor) const;
	
	QCompleter * currentCompleter(const QTextCursor & cursor);


	QCompleter * m_completerNode;
	QString m_completerParamNodeName;
	QCompleter * m_completerParam;
	QCompleter * m_completerValue;
};

#endif // __XMLEDITOR_H__
