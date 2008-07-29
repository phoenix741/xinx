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

#ifndef XMLTEXTEDITOR_H_
#define XMLTEXTEDITOR_H_

// Xinx header
#include "texteditor.h"

// Qt header
#include <QStringList>
#include <QHash>

struct XPathBalise {
	QString name;
	QHash<QString,QString> attributes;
};

class XmlTextEditor : public TextEditor {
	Q_OBJECT
public:
	XmlTextEditor( QWidget * parent = 0 );
	virtual ~XmlTextEditor();

	virtual QCompleter * completer();

	QStringList paramOfNode( const QTextCursor & cursor );
	QList<XPathBalise> xpath( const QTextCursor & cursor, const QStringList & includeOnly = QStringList(), const QString & prefix = QString(), const  QStringList & attributeName = QStringList() );

	static QString xpathToString( const QList<XPathBalise> & xp );
public slots:
	virtual void commentSelectedText( bool uncomment = false );
protected slots:
	virtual void insertCompletion( const QModelIndex& index );
	virtual void insertCompletionValue( QTextCursor & tc, QString node, QString param );
	virtual int insertCompletionParam( QTextCursor & tc, QString node, bool movePosition = true );
	virtual int insertCompletionBalises( QTextCursor & tc, QString node );
	virtual void insertCompletionAccolade( QTextCursor & tc, QString node, QString param, QString value, bool isVariable, bool isHtmlOnly );
protected:
	virtual void localKeyPressExecute( QKeyEvent * e );
	virtual bool processKeyPress( QKeyEvent * );

	enum cursorPosition {
		cpEditComment, // <!-- XXXXX  -->
		cpEditNodeName, // <XXXXX>
		cpEditParamName, // <..... XXXXX=".." XXXX=.. XXXX/>
		cpEditParamValue, // <..... ....=XXXXX ....="XXXXX XXXXX=XXXX"
		cpNone
	};

	cursorPosition editPosition( const QTextCursor & cursor, QString & nodeName, QString & paramName );
	cursorPosition editPosition( const QTextCursor & cursor );

	QString m_nodeName, m_paramName;
private:
	void key_shenter( bool back );
};

#endif /*XMLTEXTEDITOR_H_*/
