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
#include "globals.h"
#include "xsleditor.h"
#include "editorcompletion.h"
#include "xsllistview.h"
#include "xinxconfig.h"
#include "xslmodelcompleter.h"

// Qt header
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

/* XSLEditor */

#define EOW			"~!@$#%^&*()+{}|\"<>?,/;'[]\\="

#define isEditBalise(value) ((value == XSLEditor::cpEditNodeName) || (value == XSLEditor::cpEditParamName) || (value == XSLEditor::cpEditParamValue))

/* XSLEditor */

XSLEditor::XSLEditor( QWidget * parent ) : XMLEditor( parent ) {
	// Completer
	m_completerValueParamName = "";
	m_completerParamNodeName  = "";
	m_completerValue 		  = 0;

	m_completerNode = new QCompleter( this );
	m_completerNode->setWidget( this );
	m_completerNode->setCompletionMode( QCompleter::PopupCompletion );
	m_completerNode->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerNode->setCompletionRole( Qt::DisplayRole );
	connect( m_completerNode, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );
   
	m_completerParam = new QCompleter( this );
	m_completerParam->setWidget( this );
	m_completerParam->setCompletionMode( QCompleter::PopupCompletion );
	m_completerParam->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerParam->setCompletionRole( Qt::DisplayRole );
	connect( m_completerParam, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );

	m_completerValue = new QCompleter( this );
	m_completerValue->setWidget( this );
	m_completerValue->setCompletionMode( QCompleter::PopupCompletion );
	m_completerValue->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerValue->setCompletionRole( Qt::DisplayRole );
	connect( m_completerValue, SIGNAL(activated(const QModelIndex &)), this, SLOT(insertCompletion(const QModelIndex &)) );
	
	m_modelData = new XSLModelData( NULL );
	m_modelData->loadFromContent( toPlainText() );
	
	m_contentModel = new XSLItemModel( m_modelData, this );

	m_completionValueModel = NULL;
	m_completionValueModel = new XSLValueCompletionModel( m_modelData, this );
	m_completerValue->setModel( m_completionValueModel );

	m_completionParamModel = new XSLParamCompletionModel( this );
	m_completerParam->setModel( m_completionParamModel );

	m_completionBaliseModel = new XSLBaliseCompletionModel( this );
	m_completerNode->setModel( m_completionBaliseModel );
	
	connect( m_modelData, SIGNAL(hasError(QString)), this, SIGNAL(hasError(QString)) );
}

XSLEditor::~XSLEditor() {
	emit modelUpdated( NULL );
}
	

void XSLEditor::insertCompletionValue( QTextCursor & tc, QString node, QString param ) {
	if( global.m_completionContents && global.m_completionContents->balise( node ) && global.m_completionContents->balise( node )->attribute( param ) && global.m_completionContents->balise( node )->attribute( param )->defaultValue() >= 0 ) {
		int defaultValue = global.m_completionContents->balise( node )->attribute( param )->defaultValue();
		tc.insertText( QString("=\"%1\"").arg( global.m_completionContents->balise( node )->attribute( param )->values().at( defaultValue ) ) );
	} else {
		tc.insertText("=\"\"");
		tc.movePosition( QTextCursor::PreviousCharacter );
	}
}

int XSLEditor::insertCompletionParam( QTextCursor & tc, QString node, bool movePosition ) {
	int position = -1;
	if( global.m_completionContents && global.m_completionContents->balise( node ) ) {
		CompletionXMLBalise* balise = global.m_completionContents->balise( node );
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

int XSLEditor::insertCompletionBalises( QTextCursor & tc, QString node ) {
	QString indent = tc.block().text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );

	int position = -1, cnt = 0;
	
	if( global.m_config->config().editor.completionLevel == 3 && global.m_completionContents && global.m_completionContents->balise( node ) ) {
		CompletionXMLBalise* balise = global.m_completionContents->balise( node );
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
	
	if( global.m_config->config().editor.completionLevel >= 2 ) {
		if( cnt > 0 )
			tc.insertText( "\n" + indent );
		tc.insertText( QString("</%1>").arg( node ) );
		if( position != -1 )
			tc.setPosition( position );
	}
		
	return position;
} 

void XSLEditor::insertCompletionAccolade( QTextCursor & tc, QString node, QString param, QString value, int type ) {
	Q_UNUSED( param );	
	
	QTextCursor tc2( tc );
	tc2.movePosition( QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, value.length() );
	
	bool insertDollard = true, insertAccolade = true;
	QTextCursor tc3( tc2 );
	tc3.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
	if( tc3.selectedText() == "$" ) {
		tc2.movePosition( QTextCursor::PreviousCharacter );
		insertDollard = false;
	}

	QTextCursor accOpen, accClose, debParam;
	accOpen = document()->find( "{", tc2, QTextDocument::FindBackward );
	accClose = document()->find( "}", tc2, QTextDocument::FindBackward );
	debParam = document()->find( "\"", tc2, QTextDocument::FindBackward );
	if( ( accOpen > accClose ) && ( accOpen > debParam ) ) insertAccolade = false;

	if( global.m_completionContents && global.m_completionContents->balise( node ) ) {
		CompletionXMLBalise* balise = global.m_completionContents->balise( node );
		if( insertAccolade && (balise->category() != "stylesheet") && ( ( type == (int)XSLModelData::etVariable ) || ( type == (int)XSLModelData::etTemplate ) )) {
//			QMessageBox::warning(qApp->activeWindow(), "", "");
			if( insertDollard && ( type == (int)XSLModelData::etVariable ) ) 
				tc2.insertText( "{$" );
			else
				tc2.insertText( "{" );
			tc.insertText( "}" );
		} else
		if( insertDollard && ( type == (int)XSLModelData::etVariable ) ) {
			tc2.insertText( "$" );
		}
	}
}

void XSLEditor::insertCompletion( const QModelIndex& index ) {
	QTextCursor tc = textCursor();
	QCompleter * c = currentCompleter( tc );
	QString completion = c->completionModel()->data( index ).toString();
	
	for( int i = 0; i < c->completionPrefix().length(); i++ ) tc.deletePreviousChar();
	tc.insertText( completion );

	XSLEditor::cursorPosition pos = editPosition(tc);
	if( pos == XSLEditor::cpEditParamName ) {
		insertCompletionValue( tc, m_nodeName, completion );
	} else
	if( pos == XSLEditor::cpEditNodeName ) {
		if( ! m_nodeName.isEmpty() )
			insertCompletionParam( tc, completion );
		else
			tc.insertText( ">" );
	} else
	if( pos == XSLEditor::cpEditParamValue ) {
		int type = c->completionModel()->data( index, Qt::UserRole ).toInt();
		insertCompletionAccolade( tc, m_nodeName, m_paramName, completion, type );
	}

	setTextCursor( tc );
}

QCompleter * XSLEditor::currentCompleter( const QTextCursor & cursor ) {
	assert( ! cursor.isNull() );
	if( global.m_config->config().editor.completionLevel == 0 ) 
		return NULL;
	XSLEditor::cursorPosition position = editPosition( cursor );
	switch( position ) {
		case XSLEditor::cpEditNodeName:
			return m_completerNode;
		case XSLEditor::cpEditParamName: 
			if(m_nodeName != m_completerParamNodeName) {
				m_completionParamModel->setBaliseName( m_nodeName );
				m_completerParamNodeName = m_nodeName;
				return m_completerParam;
			}
		
			return m_completerParam;
		case XSLEditor::cpEditParamValue: 
			if( m_completionValueModel && ( ( m_nodeName != m_completerParamNodeName ) || ( m_paramName != m_completerValueParamName ) ) ) {
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

void XSLEditor::complete() {
	QTextCursor cursor = textCursor();
	
	QRect cr = cursorRect();
	QString completionPrefix = textUnderCursor(cursor);

	QCompleter * c = currentCompleter(textCursor());

	if( c ) {
		if( completionPrefix != m_completerNode->completionPrefix() ) {
		    c->setCompletionPrefix( completionPrefix );
			c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
		}
		cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
		c->complete( cr );
	}
}

void XSLEditor::keyPressEvent( QKeyEvent *e ) {
	if( ( global.m_config->config().editor.completionLevel == 0 ) ) {
		// Don't process completion
		TextEditor::keyPressEvent( e );
		return;
	}
	QCompleter * c = currentCompleter( textCursor() );
	
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
		TextEditor::keyPressEvent( e );
		
	if(!e->text().isEmpty()) {
		if( ( e->text().right(1) == ">" ) && ( global.m_config->config().editor.completionLevel >= 2 ) ) {
			QTextCursor tc( textCursor() );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
		
			QString selected = tc.selectedText();

			tc.movePosition( QTextCursor::NextCharacter );
	
			if( isEditBalise( editPosition( tc ) ) && selected != "/>" ){
				QString name = m_nodeName;
				if( ( ! name.isEmpty() ) && ( name.at(0) != '/' ) ) {
					tc.movePosition( QTextCursor::NextCharacter );
		
					int position = textCursor().position();
	         	
					//textCursor().insertText( QString("</%1>").arg(name) );
					int newPosition = insertCompletionBalises( tc, name );
					if( newPosition == -1 ) 
						tc.setPosition(position);
					setTextCursor( tc );
				}
			}
			try {
				updateModel();
			} catch( XMLParserException e ) {
				emit hasError( tr( "Error %1 at %2" ).arg( e.getMessage() ).arg( e.getLine() ) );
			}
		} else if( e->text().right(1) == "=" && global.m_config->config().editor.completionLevel >= 2 ) { 
			QTextCursor tc( textCursor() );
			QTextCursor tc2( textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( editPosition( tc2 ) == XSLEditor::cpEditParamName ) {
				tc.deletePreviousChar();
				insertCompletionValue( tc, m_nodeName, m_paramName );
				setTextCursor( tc );
			}
		} else if( e->text().right(1) == " " && global.m_config->config().editor.completionLevel >= 3 ) { 
			QTextCursor tc( textCursor() );
			QTextCursor tc2( textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( editPosition( tc2 ) == XSLEditor::cpEditNodeName ) {
				tc.deletePreviousChar();
				if( insertCompletionParam( tc, m_nodeName ) == -1 )
					tc.insertText( " " );
				setTextCursor( tc );
			}
		}
	}

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);

	if (!c || (ctrlOrShift && e->text().isEmpty()))
		return;

     static QString eow(EOW); // end of word
     bool hasModifier = (e->modifiers() & ( Qt::ControlModifier | Qt::AltModifier ));// && !ctrlOrShift;
     QString completionPrefix = textUnderCursor( textCursor() );

     if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
     }

     if (completionPrefix != c->completionPrefix()) {
         c->setCompletionPrefix(completionPrefix);
         c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
     }

	QRect cr = cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
	c->complete(cr); // popup it up!
}

void XSLEditor::commentSelectedText( bool uncomment ) {
	QTextCursor cursor( textCursor() );
 
	QTextCursor cursorStart( textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = editPosition( cursorStart ) == XSLEditor::cpEditComment;

	QTextCursor cursorEnd( textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  editPosition( cursorEnd ) == XSLEditor::cpEditComment;

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

QAbstractItemModel * XSLEditor::model() {
	return m_contentModel;
}

void XSLEditor::updateModel() {
	XSLModelData * datas = NULL;
	try {
		datas = new XSLModelData();
		datas->loadFromContent( toPlainText() );	
	
		emit modelUpdated( NULL );
		m_completerValue->setModel( NULL );
	
		delete m_contentModel; m_contentModel = NULL;
		delete m_completionValueModel; m_completionValueModel = NULL;
		delete m_modelData; m_modelData = NULL;
	
		m_modelData = datas;
		m_contentModel  = new XSLItemModel( m_modelData, this );
		m_completionValueModel = new XSLValueCompletionModel( m_modelData, this );
	
		emit modelUpdated( m_contentModel );
		m_completerValue->setModel( m_completionValueModel );
	} catch( XMLParserException e ) {
		delete datas;
		throw e;
	}
}

