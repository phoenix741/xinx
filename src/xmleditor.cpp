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

#include <QDebug>
#include <QIcon>
#include <QTextEdit>
#include <QStringListModel>
#include <QAbstractItemView>	
#include <QScrollBar>
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>
#include <QDomElement>
#include <assert.h>

#include "xmleditor.h"
#include "editorcompletion.h"
#include "xsllistview.h"

/* XMLProcessor */

#define EOW			"~!@$#%^&*()+{}|\"<>?,/;'[]\\="
#define EOWREGEXP	"[~!@\\$#%\\^&\\*\\(\\)\\+\\{\\}|\"<>\\?,/;'\\[\\]\\\\=\\s]"

#define isEditBalise(value) ((value == XMLProcessor::cpEditNodeName) || (value == XMLProcessor::cpEditParamName) || (value == XMLProcessor::cpEditParamValue))

/* XSLValueCompletionModel */

class XSLValueCompletionModel : public QAbstractListModel {
	Q_OBJECT
public:
	XSLValueCompletionModel( XSLModelData * data, QObject *parent = 0 );
	virtual ~XSLValueCompletionModel();
	
	void setBaliseName( const QString & name, const QString & attribute );
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
public slots:
	void refreshList();
private:
	void refreshRecursive(XSLModelData * data);

	QList<XSLModelData*> m_objList;
	XSLModelData* rootItem;
	
	QString m_baliseName;
	QString m_attributeName;
};

XSLValueCompletionModel::XSLValueCompletionModel( XSLModelData * data, QObject *parent ) : QAbstractListModel( parent ) {
	rootItem = data;
	refreshList();
	
	connect( rootItem, SIGNAL( childAboutToBeReset() ), this, SIGNAL( modelAboutToBeReset() ) );
	connect( rootItem, SIGNAL( childReseted() ), this, SLOT( refreshList() ) );
}

XSLValueCompletionModel::~XSLValueCompletionModel() {
	disconnect( rootItem, SIGNAL( childAboutToBeReset() ), this, SIGNAL( modelAboutToBeReset() ) );
	disconnect( rootItem, SIGNAL( childReseted() ), this, SLOT( refreshList() ) );
}

void XSLValueCompletionModel::refreshRecursive(XSLModelData * data) {
	for( int i = 0; i < data->childCount(); i++ ) {
		if( data->child( i )->type() != XSLModelData::etImport ) {
			m_objList.append( data->child( i ) );
		} else {
			refreshRecursive( data->child( i ) );
		}
	}
}

bool XSLValueCompletionModelObjListSort( XSLModelData * d1, XSLModelData * d2 ) {
	return d1->name() < d2->name();
}

void XSLValueCompletionModel::refreshList() {
	qDebug() << "refreshList()";
	m_objList.clear();
	refreshRecursive( rootItem );
	qSort( m_objList.begin(), m_objList.end(), XSLValueCompletionModelObjListSort );
	reset();
}

void XSLValueCompletionModel::setBaliseName( const QString & name, const QString & attribute ) { 
	int before = m_objList.count(), after  = m_objList.count();
	
	if( completionContents && completionContents->balise( m_baliseName ) && completionContents->balise( m_baliseName )->attribute( m_attributeName ) )
		before += completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
		
	if( completionContents && completionContents->balise( name ) && completionContents->balise( name )->attribute( attribute ) )
		after += completionContents->balise( name )->attribute( attribute )->values().count();
		
	int diff = after - before;

	if( diff > 0 ) 
		beginInsertRows( QModelIndex(), before + 1, after );
	else if( diff < 0 )
		beginRemoveRows( QModelIndex(), after + 1, before );

	m_baliseName = name; 
	m_attributeName = attribute; 

	if( diff > 0 ) {
		emit dataChanged( index( m_objList.count() ), index( before ) );
		endInsertRows();
	} else  if( diff < 0 ) {
		emit dataChanged( index( m_objList.count() ), index( after ) );
		endRemoveRows();
	}
}

	
QVariant XSLValueCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	if( index.row() < m_objList.count() ) {
		XSLModelData * data = m_objList[ index.row() ];
	
		if( role == Qt::DecorationRole ) {
			switch( data->type() ) {
			case XSLModelData::etVariable:
				return QIcon(":/CVpublic_var.png");
				break;
			case XSLModelData::etTemplate:
				return QIcon(":/Chtml_template.png");
				break;
			default:
				return QVariant();
			}
		} 
	
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return data->name();
		
		if( role == Qt::UserRole ) {
			return (int)data->type();
		}
	} else {
		int value_row = index.row() - m_objList.count(); 
		if( completionContents && completionContents->balise( m_baliseName ) && completionContents->balise( m_baliseName )->attribute( m_attributeName ) ) {
			if( role == Qt::DecorationRole ) 
				return QIcon(":/Chtml_value.png");
			if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
				return completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().at( value_row );
			if( role == Qt::UserRole ) {
				return -1;
			}
		}
	}
	
	return QVariant();
}

Qt::ItemFlags XSLValueCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLValueCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() ) {
		int size = m_objList.count();	
		if( completionContents ) {
			if( completionContents->balise( m_baliseName ) ) {
				if( completionContents->balise( m_baliseName )->attribute( m_attributeName ) )
					size += completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
			}
		}
		return size;
	} else
		return 0;
	}

/* XSLParamCompletionModel */

class XSLParamCompletionModel : public QAbstractListModel {
	Q_OBJECT
public:
	XSLParamCompletionModel( QObject *parent = 0 );
	virtual ~XSLParamCompletionModel();
	
	void setBaliseName( const QString & name );
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
private:
	QString m_baliseName;
};

XSLParamCompletionModel::XSLParamCompletionModel( QObject *parent ) : QAbstractListModel( parent ) {
}

XSLParamCompletionModel::~XSLParamCompletionModel() {
}

void XSLParamCompletionModel::setBaliseName( const QString & name ) { 
	emit layoutAboutToBeChanged();
	m_baliseName = name; 
	emit layoutChanged();
}

	
QVariant XSLParamCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	int value_row = index.row(); 
	if( completionContents && completionContents->balise( m_baliseName ) ) {
		if( role == Qt::DecorationRole ) 
			return QIcon(":/CVpublic_meth.png");
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return completionContents->balise( m_baliseName )->attributes().at( value_row )->name();
	}
	
	return QVariant();
}

Qt::ItemFlags XSLParamCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLParamCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() ) {
		int size = 0;	
		if( completionContents && completionContents->balise( m_baliseName ) ) 
			size += completionContents->balise( m_baliseName )->attributes().count();
		return size;
	} else
		return 0;
}

/* XSLBaliseCompletionModel */

class XSLBaliseCompletionModel : public QAbstractListModel {
	Q_OBJECT
public:
	XSLBaliseCompletionModel( QObject *parent = 0 );
	virtual ~XSLBaliseCompletionModel();
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
};

XSLBaliseCompletionModel::XSLBaliseCompletionModel( QObject *parent ) : QAbstractListModel( parent ) {
}

XSLBaliseCompletionModel::~XSLBaliseCompletionModel() {
}

QVariant XSLBaliseCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	int value_row = index.row(); 
	if( completionContents ) {
		if( role == Qt::DecorationRole ) 
			return QIcon(":/Chtml_balise.png");
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return completionContents->xmlBalises().at( value_row )->name();
	}
	
	return QVariant();
}

Qt::ItemFlags XSLBaliseCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLBaliseCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() ) {
		int size = 0;	
		if( completionContents ) 
			size += completionContents->xmlBalises().count();
		return size;
	} else
		return 0;
}


/* XMLProcessor */

XMLProcessor::XMLProcessor( QTextEdit * widget, XSLProject * project, QObject * parent ) : TextProcessor( widget, project, parent ) {
	assert( textEdit() != NULL );

	// Completer
	m_completerValueParamName = "";
	m_completerParamNodeName  = "";
	m_completerValue 		  = 0;

	m_completerNode = new QCompleter( this );
	m_completerNode->setWidget( textEdit() );
	m_completerNode->setCompletionMode( QCompleter::PopupCompletion );
	m_completerNode->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerNode->setCompletionRole( Qt::DisplayRole );
	connect( m_completerNode, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );
   
	m_completerParam = new QCompleter( this );
	m_completerParam->setWidget( textEdit() );
	m_completerParam->setCompletionMode( QCompleter::PopupCompletion );
	m_completerParam->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerParam->setCompletionRole( Qt::DisplayRole );
	connect( m_completerParam, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );

	m_completerValue = new QCompleter( this );
	m_completerValue->setWidget( textEdit() );
	m_completerValue->setCompletionMode( QCompleter::PopupCompletion );
	m_completerValue->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerValue->setCompletionRole( Qt::DisplayRole );
	connect( m_completerValue, SIGNAL(activated(const QModelIndex &)), this, SLOT(insertCompletion(const QModelIndex &)) );
	
	m_modelData = new XSLModelData( NULL, project );
	m_modelData->loadFromContent( textEdit()->toPlainText() );
	
	m_contentModel = new XSLItemModel( m_modelData, this );

	m_completionValueModel = new XSLValueCompletionModel( m_modelData, this );
	m_completerValue->setModel( m_completionValueModel );

	m_completionParamModel = new XSLParamCompletionModel( this );
	m_completerParam->setModel( m_completionParamModel );

	m_completionBaliseModel = new XSLBaliseCompletionModel( this );
	m_completerNode->setModel( m_completionBaliseModel );
}

XMLProcessor::~XMLProcessor() {
	
}
	
XMLProcessor::cursorPosition XMLProcessor::editPosition( const QTextCursor & cursor ) {
	assert( ! cursor.isNull() );
	
	cursorPosition cPosition = cpNone;
	m_nodeName = QString();
	m_paramName = QString();
	
	/* is a Comment ? */
	QTextCursor cursorCommentStart ( textEdit()->document()->find ( "<!--", cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorCommentEnd ( textEdit()->document()->find ( "-->", cursor, QTextDocument::FindBackward ) );

	bool inComment = ! ( 
			cursorCommentStart.isNull() // No comment before where i am
		||	( (! cursorCommentEnd.isNull()) && ( cursorCommentStart < cursorCommentEnd ) ) // There is a end before, and the last is the end balise
	 );

	if( inComment ) {
		cPosition = cpEditComment;
		return cPosition;
	} // else
	
	/* or simple text */
	QTextCursor cursorBaliseStart ( textEdit()->document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) ); // A balise is'nt a comment
	QTextCursor cursorBaliseEnd ( textEdit()->document()->find ( ">", cursor, QTextDocument::FindBackward ) );

	bool inNode = (! cursorBaliseStart.isNull()) && ( cursorBaliseEnd.isNull() || ( cursorBaliseEnd < cursorBaliseStart ) );
	if( ! inNode ) {
		return cPosition; // = cpNone;
	} // else

	/* Retrieve the name of node */ /* Space ? or other ? */
	QTextCursor cursorSpaceAfterNodeName ( textEdit()->document()->find ( QRegExp( EOWREGEXP ), cursorBaliseStart ) );
	QTextCursor tc = cursor;
	
	tc.setPosition( cursorBaliseStart.position(), QTextCursor::MoveAnchor ) ;
	if( ! cursorSpaceAfterNodeName.isNull() )
		tc.setPosition( cursorSpaceAfterNodeName.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
		
	m_nodeName = tc.selectedText().trimmed();
	
	/* i'm editing a balise name ? */
	QTextCursor cursorSpace ( textEdit()->document()->find ( QRegExp("\\s"), cursor, QTextDocument::FindBackward ) );
	if( cursorSpace < cursorBaliseStart ) 
		cursorSpace = QTextCursor();

	bool editBaliseName = cursorSpace.isNull();
	if( editBaliseName ) {
		cPosition = cpEditNodeName;
		return cPosition;
	} // else
	
	QTextCursor cursorEgal ( textEdit()->document()->find ( "=", cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorQuote ( textEdit()->document()->find ( "\"", cursor, QTextDocument::FindBackward ) );
	if( cursorEgal < cursorBaliseStart ) 
		cursorEgal = QTextCursor();
	if( cursorQuote < cursorBaliseStart ) 
		cursorQuote = QTextCursor();

	int quoteCount = 0;
	if( ! cursorQuote.isNull() ) {
		quoteCount ++;
		QTextCursor cursorPreviousQuote( cursorQuote );

		cursorPreviousQuote.movePosition( QTextCursor::PreviousCharacter );			
		cursorPreviousQuote = textEdit()->document()->find ( "\"", cursorPreviousQuote, QTextDocument::FindBackward );
		while( ( ! cursorPreviousQuote.isNull() ) &&  ( cursorBaliseStart < cursorPreviousQuote ) ) {
			quoteCount ++;

			cursorPreviousQuote.movePosition( QTextCursor::PreviousCharacter );			
			cursorPreviousQuote = textEdit()->document()->find ( "\"", cursorPreviousQuote, QTextDocument::FindBackward );
		}
	}
	
	/* 
		<Noeud Param1=Value1 Param2="Value 2" Param3="Value3" Param4 Param5="Value 5"/>
		       1   5    10   15   20   25   30   35   40   45   50   55   60   65   70
		       
		1 => Param / Space not null, Egal null, Quote null / 0
		8 => Value / Space < Egal, Quote null / 0
		15 => Param / Egal < Space, Quote null / 0
		22 => Value / Space < Egal, Quote null / 1
		23 => Value / Space < Egal < Quote / 1
		29 => Value / Egal < Quote < Space / 1
		31 => Param / Egal < Space  < Quote	 / 2
		35 => Param / Egal < Quote < Space / 2
	
		Compter le nombre de guillement	
	*/
	
	QTextCursor cursorParam ( textEdit()->document()->find ( QRegExp( "[=\\s]" ), cursorSpace ) );
	tc.setPosition( cursorSpace.position(), QTextCursor::MoveAnchor ) ;
	if( ! cursorParam.isNull() )
		tc.setPosition( cursorParam.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
		
	m_paramName = tc.selectedText().trimmed();

	if( cursorEgal.isNull() || ( ( cursorEgal < cursorQuote ) && ( cursorEgal < cursorSpace ) && ( ( quoteCount % 2 ) == 0 ) ) ) {
		cPosition = cpEditParamName;
		return cPosition;
	}
	
	cPosition = cpEditParamValue;
	return cPosition;
}

QString XMLProcessor::textUnderCursor( const QTextCursor & cursor ) const {
	assert( ! cursor.isNull() );

	QTextCursor before ( textEdit()->document()->find ( QRegExp( EOWREGEXP ), cursor, QTextDocument::FindBackward ) );
	QTextCursor after ( textEdit()->document()->find ( QRegExp( EOWREGEXP ), cursor ) );

	QTextCursor tc = cursor;
	
	if( ! before.isNull() )
		tc.setPosition( before.position(), QTextCursor::MoveAnchor );
	else
		tc.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor ) ;
		
	if( ! after.isNull() )
		tc.setPosition( after.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
		
	return tc.selectedText().trimmed();
}

void XMLProcessor::insertCompletionValue( QTextCursor & tc, QString node, QString param ) {
	if( completionContents && completionContents->balise( node ) && completionContents->balise( node )->attribute( param ) && completionContents->balise( node )->attribute( param )->defaultValue() >= 0 ) {
		int defaultValue = completionContents->balise( node )->attribute( param )->defaultValue();
		tc.insertText( QString("=\"%1\"").arg( completionContents->balise( node )->attribute( param )->values().at( defaultValue ) ) );
	} else {
		tc.insertText("=\"\"");
		tc.movePosition( QTextCursor::PreviousCharacter );
	}
}

int XMLProcessor::insertCompletionParam( QTextCursor & tc, QString node, bool movePosition ) {
	int position = -1;
	if( completionContents && completionContents->balise( node ) ) {
		CompletionXMLBalise* balise = completionContents->balise( node );
		foreach( CompletionXMLAttribute* attr, balise->attributes() ) {
			if( attr->isDefault() ) {
				int defaultValue = attr->defaultValue();
				QString defaultValueString;
				if( defaultValue >= 0 )
					defaultValueString = attr->values().at( defaultValue );
				tc.insertText( QString(" %1=\"%2\"").arg( attr->name() ).arg( defaultValueString ) );
				if( ( position == -1 ) && defaultValueString.isEmpty() ) {
					tc.movePosition( QTextCursor::PreviousCharacter );
					position = tc.position();
					tc.movePosition( QTextCursor::NextCharacter );
				}
			}
		}	
	}
	if( ( position != -1 ) && ( movePosition ) )
		tc.setPosition( position );
	return position;
}

int XMLProcessor::insertCompletionBalises( QTextCursor & tc, QString node ) {
	QString indent = tc.block().text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );

	int position = -1, cnt = 0;
	
	if( completionContents && completionContents->balise( node ) ) {
		CompletionXMLBalise* balise = completionContents->balise( node );
		foreach( CompletionXMLBalise* b, balise->balises() ) {
			if( b->isDefault() ) {
				tc.insertText( "\n" + indent + "\t" );
				tc.insertText( "<" + b->name() );
				int insertPosition = insertCompletionParam( tc, b->name(), false );
				if( position == -1 ) position = insertPosition;
				tc.insertText( ">" );
				insertCompletionBalises( tc, b->name() );
				cnt++;
			}
		}
	}
	if( cnt > 0 )
		tc.insertText( "\n" + indent );
	tc.insertText( QString("</%1>").arg( node ) );
	if( position != -1 )
		tc.setPosition( position );
		
	return position;
} 

void XMLProcessor::insertCompletionAccolade( QTextCursor & tc, QString node, QString param, QString value, int type ) {
	Q_UNUSED( param );	
	
	QTextCursor tc2( tc );
	tc2.movePosition( QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, value.length() );
	
	bool insertDollard = true;
	QTextCursor tc3( tc2 );
	tc3.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
	if( tc3.selectedText() == "$" ) {
		tc2.movePosition( QTextCursor::PreviousCharacter );
		insertDollard = false;
	}

	if( completionContents && completionContents->balise( node ) ) {
		CompletionXMLBalise* balise = completionContents->balise( node );
		if( (balise->category() != "stylesheet") && ( ( type == (int)XSLModelData::etVariable ) || ( type == (int)XSLModelData::etTemplate ) )) {
//			QMessageBox::warning(qApp->activeWindow(), "", "");
			if( insertDollard && ( type == (int)XSLModelData::etVariable ) ) 
				tc2.insertText( "{$" );
			else
				tc2.insertText( "{" );
			tc.insertText( "}" );
		} else
		if( insertDollard && (balise->category() == "stylesheet") && ( type == (int)XSLModelData::etVariable ) ) {
			tc2.insertText( "$" );
		}
	}
}

void XMLProcessor::insertCompletion( const QModelIndex& index ) {
	QTextCursor tc = textEdit()->textCursor();
	QCompleter * c = currentCompleter( tc );
	QString completion = c->completionModel()->data( index ).toString();
	qDebug() << "insertCompletion( " << completion << " )";
	
	for( int i = 0; i < c->completionPrefix().length(); i++ ) tc.deletePreviousChar();
	tc.insertText( completion );

	XMLProcessor::cursorPosition pos = editPosition(tc);
	if( pos == XMLProcessor::cpEditParamName ) {
		insertCompletionValue( tc, m_nodeName, completion );
	} else
	if( pos == XMLProcessor::cpEditNodeName ) {
		insertCompletionParam( tc, completion );
	} else
	if( pos == XMLProcessor::cpEditParamValue ) {
		int type = c->completionModel()->data( index, Qt::UserRole ).toInt();
		insertCompletionAccolade( tc, m_nodeName, m_paramName, completion, type );
	}

	textEdit()->setTextCursor( tc );
}

QCompleter * XMLProcessor::currentCompleter( const QTextCursor & cursor ) {
	assert( ! cursor.isNull() );

	XMLProcessor::cursorPosition position = editPosition( cursor );
	switch( position ) {
		case XMLProcessor::cpEditNodeName:
			qDebug() << "currentCompleter() : cpEditNodeName";
			return m_completerNode;
		case XMLProcessor::cpEditParamName: 
			qDebug() << "currentCompleter() : cpEditParamName";
			if(m_nodeName != m_completerParamNodeName) {
				qDebug() <<  "currentCompleter() : node change";
				m_completionParamModel->setBaliseName( m_nodeName );
				m_completerParamNodeName = m_nodeName;
				return m_completerParam;
			}
		
			return m_completerParam;
		case XMLProcessor::cpEditParamValue: 
			qDebug() << "currentCompleter() : cpEditParamValue";
			if( ( m_nodeName != m_completerParamNodeName ) || ( m_paramName != m_completerValueParamName ) ) {
				m_completionParamModel->setBaliseName( m_nodeName );
				m_completionValueModel->setBaliseName( m_nodeName, m_paramName );
				m_completerParamNodeName = m_nodeName;
				m_completerValueParamName = m_paramName;
			}
			return m_completerValue;
		default:
			return NULL;	
	}
}

void XMLProcessor::complete() {
	QTextCursor cursor = textEdit()->textCursor();
	
	QRect cr = textEdit()->cursorRect();
	QString completionPrefix = textUnderCursor(cursor);

	QCompleter * c = currentCompleter(textEdit()->textCursor());

	if( c ) {
		if( completionPrefix != m_completerNode->completionPrefix() ) {
		    c->setCompletionPrefix( completionPrefix );
			c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
		}
		cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
		c->complete( cr );
	}
}

void XMLProcessor::keyPressEvent( QKeyEvent *e ) {
	QCompleter * c = currentCompleter( textEdit()->textCursor() );
	
	if (c && c->popup()->isVisible()) {
		qDebug() << "keyPressEvent() : completer is visible";
		// The following keys are forwarded by the completer to the widget
		switch (e->key()) {
		case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
			e->ignore();
			return; // let the completer do default behavior
		default:
            break;
        }
     }

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if (!c || !isShortcut) // dont process the shortcut when we have a completer
		parentKeyPressEvent( e );
		
	if(!e->text().isEmpty()) {
		if(e->text().right(1) == ">") {
			QTextCursor tc( textEdit()->textCursor() );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
		
			QString selected = tc.selectedText();

			tc.movePosition( QTextCursor::NextCharacter );
	
			if( isEditBalise( editPosition( tc ) ) && selected != "/>" ){
				QString name = m_nodeName;
				if( ( ! name.isEmpty() ) && ( name.at(0) != '/' ) ) {
					tc.movePosition( QTextCursor::NextCharacter );
		
					int position = textEdit()->textCursor().position();
	         	
					/*textEdit()->textCursor().insertText( QString("</%1>").arg(name) );*/
					int newPosition = insertCompletionBalises( tc, name );
					if( newPosition == -1 ) 
						tc.setPosition(position);
					textEdit()->setTextCursor( tc );
				}
			}
			updateModel();
		} else if( e->text().right(1) == "=" ) { 
			QTextCursor tc( textEdit()->textCursor() );
			QTextCursor tc2( textEdit()->textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( editPosition( tc2 ) == XMLProcessor::cpEditParamName ) {
				tc.deletePreviousChar();
				insertCompletionValue( tc, m_nodeName, m_paramName );
				textEdit()->setTextCursor( tc );
			}
		} else if( e->text().right(1) == " " ) { 
			QTextCursor tc( textEdit()->textCursor() );
			QTextCursor tc2( textEdit()->textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( editPosition( tc2 ) == XMLProcessor::cpEditNodeName ) {
				tc.deletePreviousChar();
				if( insertCompletionParam( tc, m_nodeName ) == -1 )
					tc.insertText( " " );
				textEdit()->setTextCursor( tc );
			}
		}
	}

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);

	if( ( ! ctrlOrShift ) && ( ( e->key() == Qt::Key_Enter ) || ( e->key() == Qt::Key_Return ) ) ) {
		QTextCursor tc( textEdit()->textCursor() );
		QTextBlock previous = tc.block().previous();	
		QString indent = previous.text();
		
		indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );
		
		tc.movePosition( QTextCursor::StartOfLine );
		
		tc.insertText( indent );
		
		textEdit()->setTextCursor( tc );
		
		return;
	}

	if (!c || (ctrlOrShift && e->text().isEmpty()))
		return;

     static QString eow(EOW); // end of word
     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor( textEdit()->textCursor() );

     if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
     }

     if (completionPrefix != c->completionPrefix()) {
         c->setCompletionPrefix(completionPrefix);
         c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
     }

	QRect cr = textEdit()->cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
	qDebug() << "keyPressEvent() : show completer";
	c->complete(cr); // popup it up!
}

void XMLProcessor::commentSelectedText( bool uncomment ) {
	QTextCursor cursor( textEdit()->textCursor() );
 
	QTextCursor cursorStart( textEdit()->textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = editPosition( cursorStart ) == XMLProcessor::cpEditComment;

	QTextCursor cursorEnd( textEdit()->textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  editPosition( cursorEnd ) == XMLProcessor::cpEditComment;

	QString text = cursor.selectedText();
	text = text.replace( "<!--", "" );
	text = text.replace( "-->", "" );
  
	cursor.beginEditBlock();

	cursor.removeSelectedText();
	if(! ( isStartCommented ^ uncomment ) ) {
		// Comment  	
		if(! uncomment)
			cursor.insertText("<!--");  
		else
			cursor.insertText("-->");  
	}
	cursor.insertText(text);  
	if(! ( isEndCommented ^ uncomment )) {
		// End the comment  	
		if(! uncomment)
			cursor.insertText("-->");  
		else
			cursor.insertText("<!--");  
	}

	cursor.endEditBlock();
}

QAbstractItemModel * XMLProcessor::model() {
	/* Est-ce anti performant ? */
	updateModel();
	return m_contentModel;
}

void XMLProcessor::updateModel() {
	m_modelData->loadFromContent( textEdit()->toPlainText() );
}

#include "xmleditor.moc"
