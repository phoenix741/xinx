/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

#ifndef XMLTEXTEDITOR_H_
#define XMLTEXTEDITOR_H_

// Xinx header
#include <editors/xinxcodeedit.h>

// Qt header
#include <QStringList>
#include <QHash>

/* XPathBalise */

struct XPathBalise {
	QString name;
	QHash<QString,QString> attributes;
};

/* XmlTextEditor */

class XmlTextEditor : public XinxCodeEdit {
	Q_OBJECT
public:
	XmlTextEditor( QWidget * parent = 0 );
	virtual ~XmlTextEditor();

	virtual QCompleter * completer();

	QStringList paramOfNode( const QDocumentCursor & cursor );
	QList<XPathBalise> xpath( const QDocumentCursor & cursor, const QStringList & includeOnly = QStringList(), const QString & prefix = QString(), const  QStringList & attributeName = QStringList() );

	static QString xpathToString( const QList<XPathBalise> & xp );
public slots:
	virtual void commentSelectedText( bool uncomment = false );
protected slots:
	virtual void insertCompletion( const QModelIndex& index );
	virtual void insertCompletionValue( QDocumentCursor & tc, QString node, QString param );
	virtual QDocumentCursor insertCompletionParam( QDocumentCursor & tc, QString node, bool movePosition = true );
	virtual QDocumentCursor insertCompletionBalises( QDocumentCursor & tc, QString node );
	virtual void insertCompletionAccolade( QDocumentCursor & tc, QString node, QString param, QString value, const QModelIndex & index );
protected:
	virtual bool localKeyPressExecute( QKeyEvent * e );
	virtual bool processKeyPress( QKeyEvent * );

	enum cursorPosition {
		cpEditComment, // <!-- XXXXX  -->
		cpEditNodeName, // <XXXXX>
		cpEditParamName, // <..... XXXXX=".." XXXX=.. XXXX/>
		cpEditParamValue, // <..... ....=XXXXX ....="XXXXX XXXXX=XXXX"
		cpNone
	};

	cursorPosition editPosition( const QDocumentCursor & cursor, QString & nodeName, QString & paramName );
private:
	void key_shenter( bool back );
};

/* XslTextEditor */

class XslCompletionNodeModel;

class XslTextEditor : public XmlTextEditor {
	Q_OBJECT
public:
	XslTextEditor( QWidget * parent = 0 );
	virtual ~XslTextEditor();

	virtual QCompleter * completer();

	virtual QDocumentCursor insertCompletionBalises( QDocumentCursor & tc, QString node );
	virtual void insertCompletionAccolade( QDocumentCursor & tc, QString node, QString param, QString value, const QModelIndex & index );

	void setModel( XslCompletionNodeModel * model );

signals:
	void positionInEditorChanged( const QModelIndex & index );

protected:
	virtual bool processKeyPress( QKeyEvent * );
private:
	QString paramValue( const QDocumentCursor & cursor, const QString & param );
	void getTemplate( const QDocumentCursor & cursor, QString * name, QString * mode );

	XslCompletionNodeModel * m_model;
};

#endif /*XMLTEXTEDITOR_H_*/
