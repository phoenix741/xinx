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

#include <QtGui>
#include <QCompleter>
#include <QDomElement>
#include <assert.h>

#include "xmleditor.h"
#include "editorcompletion.h"
#include "xsllistview.h"

/* XMLProcessor */

#define EOW			"~!@#$%^&*()+{}|\"<>?,./;'[]\\="
#define EOWREGEXP	"[~!@#\\$%\\^&\\*\\(\\)\\+\\{\\}|\"<>\\?,\\./;'\\[\\]\\\\=\\s]"

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
	
	void setFiltre(XSLModelData::ElementType filtre) { m_filtre = filtre; refreshList(); };
	void refreshList();
private:
	void refreshRecursive(XSLModelData * data);

	QStringList m_list;
	XSLModelData::ElementType m_filtre;
	XSLModelData* rootItem;
};

XSLValueCompletionModel::XSLValueCompletionModel( XSLModelData * data, QObject *parent ) : QAbstractListModel( parent ) {
	rootItem = data;
	m_filtre = XSLModelData::etVariable;
	refreshList();
	
	connect( rootItem, SIGNAL( childReseted() ), this, SIGNAL( layoutChanged() ) );
}

XSLValueCompletionModel::~XSLValueCompletionModel() {
	disconnect( rootItem, SIGNAL( childReseted() ), this, SIGNAL( layoutChanged() ) );
}

void XSLValueCompletionModel::refreshRecursive(XSLModelData * data) {
	for( int i = 0; i < data->childCount(); i++ ) {
		if( data->child( i )->type() == m_filtre ) {
			m_list.append( data->child( i )->name() );
		}
		if( data->child( i )->type() == XSLModelData::etImport ) {
			refreshRecursive( data->child( i ) );
		}
	}
}

void XSLValueCompletionModel::refreshList() {
	m_list.clear();
	refreshRecursive( rootItem );
	m_list.sort();
	emit layoutChanged();
}
	
QVariant XSLValueCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	QString data = m_list[ index.row() ];
	
	if( role == Qt::DecorationRole ) {
		switch( m_filtre ) {
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
	
	if ( role == Qt::DisplayRole ) 
		return data;
	
	return QVariant();
}

Qt::ItemFlags XSLValueCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLValueCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() )
		return m_list.count();
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
	connect( m_completerValue, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)) );
	
	m_modelData = new XSLModelData( NULL, project );
	m_modelData->loadFromContent( textEdit()->toPlainText() );
	
	m_contentModel = new XSLItemModel( m_modelData, this );
	m_completionValueModel = new XSLValueCompletionModel( m_modelData, this );
}

XMLProcessor::~XMLProcessor() {
	
}
	
XMLProcessor::cursorPosition XMLProcessor::editPosition( const QTextCursor & cursor ) {
	assert( ! cursor.isNull() );
	
	cursorPosition cPosition = cpNone;
	m_nodeName = QString();
	m_paramName = QString();
	
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
	
	QTextCursor cursorBaliseStart ( textEdit()->document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) ); // A balise is'nt a comment
	QTextCursor cursorBaliseEnd ( textEdit()->document()->find ( ">", cursor, QTextDocument::FindBackward ) );

	bool inNode = (! cursorBaliseStart.isNull()) && ( cursorBaliseEnd.isNull() || ( cursorBaliseEnd < cursorBaliseStart ) );
	if( ! inNode ) {
		return cPosition; // = cpNone;
	} // else

	/* Retrieve the name of node */
	QTextCursor cursorSpaceAfterNodeName ( textEdit()->document()->find ( QRegExp( EOWREGEXP ), cursorBaliseStart ) );
	QTextCursor tc = cursor;
	
	tc.setPosition( cursorBaliseStart.position(), QTextCursor::MoveAnchor ) ;
	if( ! cursorSpaceAfterNodeName.isNull() )
		tc.setPosition( cursorSpaceAfterNodeName.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
		
	m_nodeName = tc.selectedText().trimmed();
	
	QTextCursor cursorSpace ( textEdit()->document()->find ( QRegExp("\\s"), cursor, QTextDocument::FindBackward ) );

	bool editBaliseName = cursorSpace.isNull() || ( cursorSpace < cursorBaliseStart );
	if( editBaliseName ) {
		cPosition = cpEditNodeName;
		return cPosition;
	} // else
	
	QTextCursor cursorEgal ( textEdit()->document()->find ( "=", cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorQuote ( textEdit()->document()->find ( "\"", cursor, QTextDocument::FindBackward ) );
	
	/* 
		<Noeud Param1=Value1 Param2="Value 2" Param3="Value3" Param4 Param5="Value 5"/>
		       1   5    10   15   20   25   30   35   40   45   50   55   60   65   70
		       
		1 => Param / Space not null, Egal null, Quote null
		8 => Value / Space < Egal, Quote null
		15 => Param / Egal < Space, Quote null
		22 => Value / Space < Egal, Quote null
		23 => Value / Space < Egal < Quote
		29 => Value / Egal < Quote < Space
		31 => Param / Egal < Space  < Quote	
		35 => Param / Egal < Quote < Space
		
	*/
	
	QTextCursor cursorParam ( textEdit()->document()->find ( QRegExp( "[=\\s]" ), cursorSpace ) );
	tc.setPosition( cursorSpace.position(), QTextCursor::MoveAnchor ) ;
	if( ! cursorParam.isNull() )
		tc.setPosition( cursorParam.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
		
	m_paramName = tc.selectedText().trimmed();

	if( cursorEgal.isNull() || ( ( cursorEgal < cursorQuote ) && ( cursorEgal < cursorSpace ) ) ) {
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
			return m_completerNode;
		case XMLProcessor::cpEditParamName: 
			if(m_nodeName != m_completerParamNodeName) {
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
			if( ( m_completerValueParamName != m_paramName ) || ( m_nodeName != m_completerParamNodeName ) ) {
				m_completerParamNodeName = m_nodeName;
				m_completerValueParamName = m_paramName;
				
				if( m_completerValueParamName == "select" ) {
					m_completionValueModel->setFiltre( XSLModelData::etVariable );
					m_completerValue->setModel( m_completionValueModel );
				} else
					m_completerValue->setModel( NULL );
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
				if( ! name.isEmpty() ) {
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
         c->popup()->hide();
         return;
     }

     if (completionPrefix != c->completionPrefix()) {
         c->setCompletionPrefix(completionPrefix);
         c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
     }

     QRect cr = textEdit()->cursorRect();
     cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
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
