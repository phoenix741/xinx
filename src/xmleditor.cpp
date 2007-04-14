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
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
public slots:
	void refreshList();
private:
	void refreshRecursive(XSLModelData * data);

	QList<XSLModelData*> m_objList;
	XSLModelData* rootItem;
};

XSLValueCompletionModel::XSLValueCompletionModel( XSLModelData * data, QObject *parent ) : QAbstractListModel( parent ) {
	rootItem = data;
	refreshList();
	
	connect( rootItem, SIGNAL( childReseted() ), this, SLOT( refreshList() ) );
}

XSLValueCompletionModel::~XSLValueCompletionModel() {
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
	emit layoutChanged();
}
	
QVariant XSLValueCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	XSLModelData * data = m_objList[ index.row() ];
	
	if( role == Qt::DecorationRole ) {
		switch( data->type() ) {
		case XSLModelData::etVariable:
			return QIcon(":/CVpublic_var.png");
			break;
		case XSLModelData::etTemplate:
			return QIcon(":/CVpublic_meth.png");
			break;
		default:
			return QVariant();
		}
	} 
	
	if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
		return data->name();
	
	return QVariant();
}

Qt::ItemFlags XSLValueCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLValueCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() )
		return m_objList.count();
	else
		return 0;
}

/* XMLProcessor */

XMLProcessor::XMLProcessor( QTextEdit * widget, XSLProject * project, QObject * parent ) : TextProcessor( widget, project, parent ) {
	assert( completionNodeList != NULL );
	assert( textEdit() != NULL );
	
	QStringList wordList;
	for(int i = 0; i < completionNodeList->count(); i++) {
		wordList << completionNodeList->node(i)->name();
	}
	wordList.sort();

	// Completer
	m_completerNode = new QCompleter( wordList, this );
	m_completerNode->setWidget( textEdit() );
	m_completerNode->setCompletionMode( QCompleter::PopupCompletion );
	m_completerNode->setCaseSensitivity( Qt::CaseInsensitive );
	connect( m_completerNode, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)) );
   
	m_completerParam = new QCompleter( this );
	m_completerParam->setWidget( textEdit() );
	m_completerParam->setCompletionMode( QCompleter::PopupCompletion );
	m_completerParam->setCaseSensitivity( Qt::CaseInsensitive );
	connect( m_completerParam, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)) );
	m_completerParamNodeName = "";
	m_completerValue 		 = 0;

	m_completerValue = new QCompleter( this );
	m_completerValue->setWidget( textEdit() );
	m_completerValue->setCompletionMode( QCompleter::PopupCompletion );
	m_completerValue->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerValue->setCompletionRole( Qt::DisplayRole );
	connect( m_completerValue, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)) );
	
	m_modelData = new XSLModelData( NULL, project );
	m_modelData->loadFromContent( textEdit()->toPlainText() );
	
	m_contentModel = new XSLItemModel( m_modelData, this );
	m_completionValueModel = new XSLValueCompletionModel( m_modelData, this );
	
	m_completerValue->setModel( m_completionValueModel );
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

void XMLProcessor::insertCompletion( const QString& completion ) {
	qDebug() << "insertCompletion( " << completion << " )";
	QTextCursor tc = textEdit()->textCursor();
	QCompleter * c = currentCompleter( tc );
	
	int extra = completion.length() - c->completionPrefix().length();
	tc.insertText(completion.right(extra));
	// tc.movePosition( QTextCursor::EndOfWord ); /* Don't need this */

	if( editPosition(tc) == XMLProcessor::cpEditParamName ) {		
		tc.insertText("=\"\"");
		tc.movePosition( QTextCursor::PreviousCharacter );
	}

	textEdit()->setTextCursor( tc );
}

QCompleter * XMLProcessor::currentCompleter( const QTextCursor & cursor ) {
	assert( ! cursor.isNull() );

	XMLProcessor::cursorPosition position = editPosition( cursor );
	switch( position ) {
		case XMLProcessor::cpEditNodeName:
			qDebug() << "currentCompleter() : EditNodeName";
			return m_completerNode;
		case XMLProcessor::cpEditParamName: 
			qDebug() << "currentCompleter() : EditParamName";
			if(m_nodeName != m_completerParamNodeName) {
				qDebug() <<  "currentCompleter() : node change";
				m_completerParamNodeName = m_nodeName;
				if(completionNodeList->node( m_nodeName )) {
					QStringList wordList;
					for(int i = 0; i < completionNodeList->node( m_nodeName )->count(); i++) {
						wordList << completionNodeList->node( m_nodeName )->param(i);
					}
					wordList.sort();
					m_completerParam->setModel(new QStringListModel(wordList, m_completerParam));
				} else 
					return NULL;
			}
		
			return m_completerParam;
		case XMLProcessor::cpEditParamValue: 
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
					int position = textEdit()->textCursor().position();
	         	
					textEdit()->textCursor().insertText( QString("</%1>").arg(name) );
		
					tc.setPosition(position);
					textEdit()->setTextCursor( tc );
				}
			}
			updateModel();
		} else if(e->text().right(1) == "=") {
			QTextCursor tc( textEdit()->textCursor() );
			QTextCursor tc2( textEdit()->textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( editPosition( tc2 ) == XMLProcessor::cpEditParamName ) {
				tc.insertText( "\"\"" );
				tc.movePosition( QTextCursor::PreviousCharacter );
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
		qDebug() << "keyPressEvent() : hide completer";
		qDebug() << "keyPressEvent() :               isShortcut = " << isShortcut;
		qDebug() << "keyPressEvent() :               hasModifier = " << hasModifier;
		qDebug() << "keyPressEvent() :               e->text().isEmpty() = " << e->text().isEmpty();
		qDebug() << "keyPressEvent() :               completionPrefix.length() = " << completionPrefix.length();
		qDebug() << "keyPressEvent() :               eow.contains(e->text().right(1)) = " << eow.contains(e->text().right(1));
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
