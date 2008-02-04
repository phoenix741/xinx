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

#ifndef XMLCOMPLETER_H_
#define XMLCOMPLETER_H_

// Xinx header
#include "iextendededitor.h"

// Qt header
#include <QTextCursor>
#include <QString>
#include <QTextCursor>
#include <QModelIndex>

class QCompleter;
class QKeyEvent;
class XSLValueCompletionModel;
class XSLParamCompletionModel;
class XSLBaliseCompletionModel;

class XmlCompleter : public QObject {
	Q_OBJECT
public:
	XmlCompleter( IXinxExtendedEditor * editor, bool onlyHtml = false );
	~XmlCompleter();
	
	static void commentSelectedText( IXinxExtendedEditor * editor, bool uncomment );

	bool keyPressEvent( QKeyEvent *e );
	QCompleter * currentCompleter( const QTextCursor & cursor );
	
	QPair<QString,int> searchWord( const QString & word );

	FileContentElementList * list();
private slots:
	void insertCompletion( const QModelIndex& index );
private:
	enum cursorPosition {
		cpEditComment, // <!-- XXXXX  -->
		cpEditNodeName, // <XXXXX>
		cpEditParamName, // <..... XXXXX=".." XXXX=.. XXXX/>
		cpEditParamValue, // <..... ....=XXXXX ....="XXXXX XXXXX=XXXX"
		cpNone
	};
	
	static cursorPosition editPosition( const QTextEdit * textEdit, const QTextCursor & cursor, QString & nodeName, QString & paramName );
	cursorPosition editPosition( const QTextCursor & cursor );
	
	void insertCompletionValue( QTextCursor & tc, QString node, QString param );
	int insertCompletionParam( QTextCursor & tc, QString node, bool movePosition = true );
	int insertCompletionBalises( QTextCursor & tc, QString node );
	void insertCompletionAccolade( QTextCursor & tc, QString node, QString param, QString value, QString type );
	
	QCompleter * m_completerNode;
	QString m_completerParamNodeName, m_completerValueParamName;
	QCompleter * m_completerParam;
	QCompleter * m_completerValue;

	XSLValueCompletionModel * m_completionValueModel;
	XSLParamCompletionModel * m_completionParamModel;
	XSLBaliseCompletionModel * m_completionBaliseModel;
	
	FileContentElementList * m_list;

	QString m_nodeName, m_paramName;
	IXinxExtendedEditor * m_editor;
};

#endif /*XMLCOMPLETER_H_*/
