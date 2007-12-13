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
#include "filetypexsl.h"
#include "private/p_filetypexsl.h"
#include "texteditor.h"
#include "globals.h"
#include "editorcompletion.h"
#include "xinxconfig.h"
#include "exceptions.h"

// Qt header
#include <QDomElement>
#include <QTextBlock>
#include <QAbstractItemView>
#include <QRect>
#include <QScrollBar>

// Define 
#define EOW			"~!@$#%^&*()+{}|\"<>?,/;'[]\\="
#define isEditBalise(value) ((value == FileTypeXsl::cpEditNodeName) || (value == FileTypeXsl::cpEditParamName) || (value == FileTypeXsl::cpEditParamValue))

/* PrivateFileTypeXsl */

PrivateFileTypeXsl::PrivateFileTypeXsl( FileTypeXsl * parent ) : m_parent( parent ) {
	m_completerValueParamName = "";
	m_completerParamNodeName  = "";
	m_completerValue 		  = 0;
	m_modelData 			  = NULL;
	m_contentModel 			  = NULL;
	m_thread 				  = NULL;

	m_completerNode = new QCompleter( this );
	m_completerNode->setWidget( m_parent->textEdit() );
	m_completerNode->setCompletionMode( QCompleter::PopupCompletion );
	m_completerNode->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerNode->setCompletionRole( Qt::DisplayRole );
	connect( m_completerNode, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );
   
	m_completerParam = new QCompleter( this );
	m_completerParam->setWidget( m_parent->textEdit() );
	m_completerParam->setCompletionMode( QCompleter::PopupCompletion );
	m_completerParam->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerParam->setCompletionRole( Qt::DisplayRole );
	connect( m_completerParam, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );

	m_completerValue = new QCompleter( this );
	m_completerValue->setWidget( m_parent->textEdit() );
	m_completerValue->setCompletionMode( QCompleter::PopupCompletion );
	m_completerValue->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerValue->setCompletionRole( Qt::DisplayRole );
	connect( m_completerValue, SIGNAL(activated(const QModelIndex &)), this, SLOT(insertCompletion(const QModelIndex &)) );
	
	m_completionValueModel = NULL;

	m_completionParamModel = new XSLParamCompletionModel( this );
	m_completerParam->setModel( m_completionParamModel );

	m_completionBaliseModel = new XSLBaliseCompletionModel( this );
	m_completerNode->setModel( m_completionBaliseModel );
}

PrivateFileTypeXsl::~PrivateFileTypeXsl() {
	emit m_parent->modelUpdated( NULL );
	delete m_thread; // KILL
}

void PrivateFileTypeXsl::keyPressEvent( QKeyEvent *e ) {
	if( ( global.m_config->config().editor.completionLevel == 0 ) ) {
		m_parent->textEdit()->keyPressExecute( e );
		return;
	}
		
	QCompleter * c = currentCompleter( m_parent->textEdit()->textCursor() );
	
	if (c && c->popup()->isVisible()) {
		// The following keys are forwarded by the completer to the widget
		switch (e->key()) {
		case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
			e->ignore();
			m_parent->textEdit()->keyPressSkip( e );
			return; // let the completer do default behavior
		default:
            break;
        }
     }

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if( !c || !isShortcut )
		m_parent->textEdit()->keyPressExecute( e );
	else
		m_parent->textEdit()->keyPressSkip( e );
				
	if(!e->text().isEmpty()) {
		if( ( e->text().right(1) == ">" ) && ( global.m_config->config().editor.completionLevel >= 2 ) ) {
			QTextCursor tc( m_parent->textEdit()->textCursor() );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
		
			QString selected = tc.selectedText();

			tc.movePosition( QTextCursor::NextCharacter );
	
			if( isEditBalise( m_parent->editPosition( tc ) ) && selected != "/>" ){
				QString name = m_parent->m_nodeName;
				if( ( ! name.isEmpty() ) && ( name.at(0) != '/' ) ) {
					tc.movePosition( QTextCursor::NextCharacter );
					int position = m_parent->textEdit()->textCursor().position();
					int newPosition = insertCompletionBalises( tc, name );
					if( newPosition == -1 ) 
						tc.setPosition(position);
					m_parent->textEdit()->setTextCursor( tc );
				}
			}
			if( c )
				c->popup()->hide();
			emit m_parent->canUpdateModel();
			return;
		} else if( e->text().right(1) == "=" && global.m_config->config().editor.completionLevel >= 2 ) { 
			QTextCursor tc( m_parent->textEdit()->textCursor() );
			QTextCursor tc2( m_parent->textEdit()->textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( m_parent->editPosition( tc2 ) == FileTypeXsl::cpEditParamName ) {
				tc.deletePreviousChar();
				insertCompletionValue( tc, m_parent->m_nodeName, m_parent->m_paramName );
				m_parent->textEdit()->setTextCursor( tc );
			}
			if( c )
				c->popup()->hide();
			return;
		} else if( e->text().right(1) == " " && global.m_config->config().editor.completionLevel >= 3 ) { 
			QTextCursor tc( m_parent->textEdit()->textCursor() );
			QTextCursor tc2( m_parent->textEdit()->textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( m_parent->editPosition( tc2 ) == FileTypeXsl::cpEditNodeName ) {
				tc.deletePreviousChar();
				if( insertCompletionParam( tc, m_parent->m_nodeName ) == -1 )
					tc.insertText( " " );
				m_parent->textEdit()->setTextCursor( tc );
			}
			if( c )
				c->popup()->hide();
			return;
		}
	}

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!c || (ctrlOrShift && e->text().isEmpty()))
		return;

	static QString eow(EOW); // end of word
	bool hasModifier = (e->modifiers() & ( Qt::ControlModifier | Qt::AltModifier ));// && !ctrlOrShift;
	QString completionPrefix = m_parent->textEdit()->textUnderCursor( m_parent->textEdit()->textCursor() );

	if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains(e->text().right(1)))) {
		c->popup()->hide();
		return;
	}

	if (completionPrefix != c->completionPrefix()) {
		c->setCompletionPrefix(completionPrefix);
		c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
	}

	QRect cr = m_parent->textEdit()->cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
	c->complete(cr); // popup it up!
}

void PrivateFileTypeXsl::insertCompletion( const QModelIndex& index ) {
	QTextCursor tc = m_parent->textEdit()->textCursor();
	QCompleter * c = currentCompleter( tc );
	
	QString completion = c->completionModel()->data( index ).toString(),
			prefix     = c->completionPrefix();
	
	m_parent->textEdit()->textUnderCursor( tc, true );
	//tc.movePosition( QTextCursor::EndOfWord );
	//for( int i = 0 ; i < prefix.length() ; i++ ) tc.deletePreviousChar();
	tc.insertText( completion );

	FileTypeXsl::cursorPosition pos = m_parent->editPosition( tc );
	if( pos == FileTypeXsl::cpEditParamName ) {
		insertCompletionValue( tc, m_parent->m_nodeName, completion );
	} else
	if( pos == FileTypeXsl::cpEditNodeName ) {
		if( ! m_parent->m_nodeName.isEmpty() )
			insertCompletionParam( tc, completion );
		else
			tc.insertText( ">" );
	} else
	if( pos == FileTypeXsl::cpEditParamValue ) {
		QString type = c->completionModel()->data( index, Qt::UserRole ).toString();
		insertCompletionAccolade( tc, m_parent->m_nodeName, m_parent->m_paramName, completion, type );
	}

	m_parent->textEdit()->setTextCursor( tc );
}

void PrivateFileTypeXsl::insertCompletionValue( QTextCursor & tc, QString node, QString param ) {
	if( global.m_completionContents && global.m_completionContents->balise( node ) && global.m_completionContents->balise( node )->attribute( param ) && global.m_completionContents->balise( node )->attribute( param )->defaultValue() >= 0 ) {
		int defaultValue = global.m_completionContents->balise( node )->attribute( param )->defaultValue();
		tc.insertText( QString("=\"%1\"").arg( global.m_completionContents->balise( node )->attribute( param )->values().at( defaultValue ) ) );
	} else {
		tc.insertText("=\"\"");
		tc.movePosition( QTextCursor::PreviousCharacter );
	}
}

int PrivateFileTypeXsl::insertCompletionParam( QTextCursor & tc, QString node, bool movePosition ) {
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

int PrivateFileTypeXsl::insertCompletionBalises( QTextCursor & tc, QString node ) {
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

void PrivateFileTypeXsl::insertCompletionAccolade( QTextCursor & tc, QString node, QString param, QString value, QString type ) {
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
	accOpen = m_parent->textEdit()->document()->find( "{", tc2, QTextDocument::FindBackward );
	accClose = m_parent->textEdit()->document()->find( "}", tc2, QTextDocument::FindBackward );
	debParam = m_parent->textEdit()->document()->find( "\"", tc2, QTextDocument::FindBackward );
	if( ( accOpen > accClose ) && ( accOpen > debParam ) ) insertAccolade = false;

	if( global.m_completionContents && global.m_completionContents->balise( node ) ) {
		CompletionXMLBalise* balise = global.m_completionContents->balise( node );
		if( insertAccolade && (balise->category() != "stylesheet") && ( ( type == "XSLFileContentParams" ) || ( type == "XSLFileContentVariable" ) || ( type == "XSLFileContentTemplate" ) )) {
//			QMessageBox::warning(qApp->activeWindow(), "", "");
			if( insertDollard && ( ( type == "XSLFileContentParams" ) || ( type == "XSLFileContentVariable" ) ) ) 
				tc2.insertText( "{$" );
			else
				tc2.insertText( "{" );
			tc.insertText( "}" );
		} else
		if( insertDollard && ( ( type == "XSLFileContentParams" ) || ( type == "XSLFileContentVariable" ) ) ) {
			tc2.insertText( "$" );
		}
	}
}
	
QCompleter * PrivateFileTypeXsl::currentCompleter( const QTextCursor & cursor ) {
	XINX_ASSERT( ! cursor.isNull() );
	if( global.m_config->config().editor.completionLevel == 0 ) 
		return NULL;
	FileTypeXsl::cursorPosition position = m_parent->editPosition( cursor );
	switch( position ) {
		case FileTypeXsl::cpEditNodeName:
			return m_completerNode;
		case FileTypeXsl::cpEditParamName: 
			if( m_parent->m_nodeName != m_completerParamNodeName ) {
				m_completionParamModel->setBaliseName( m_parent->m_nodeName );
				m_completerParamNodeName = m_parent->m_nodeName;
				return m_completerParam;
			}
		
			return m_completerParam;
		case FileTypeXsl::cpEditParamValue: 
			if( m_completionValueModel && ( ( m_parent->m_nodeName != m_completerParamNodeName ) || ( m_parent->m_paramName != m_completerValueParamName ) ) ) {
				m_completionParamModel->setBaliseName( m_parent->m_nodeName );
				m_completionValueModel->setBaliseName( m_parent->m_nodeName, m_parent->m_paramName );
				m_completerParamNodeName = m_parent->m_nodeName;
				m_completerValueParamName = m_parent->m_paramName;
			}
			return m_completerValue;
		default:
			return NULL;	
	}
}

/* FileTypeXsl */

FileTypeXsl::FileTypeXsl( TextEditor * parent ) : FileTypeXml( parent ) {
	d = new PrivateFileTypeXsl( this );

	connect( parent, SIGNAL(execKeyPressEvent(QKeyEvent*)), d, SLOT(keyPressEvent(QKeyEvent*)) );
}

FileTypeXsl::~FileTypeXsl() {
	delete d;
}

void FileTypeXsl::updateModel() {
	if( d->m_modelData ) {
//		d->m_modelData->loadFromContent( textEdit()->toPlainText() );
		d->m_thread->reloadEditorContent( textEdit()->toPlainText() );
	} else {
		d->m_thread = new EditorThreadXSL( this );
		d->m_thread->start();
		while( ! d->m_thread->parser() ) ;
		
		d->m_modelData = new XSLFileContentParser();
		d->m_contentModel = new FileContentItemModel( dynamic_cast<XSLFileContentParser*>( d->m_thread->parser()), this );
//		d->m_completionValueModel = new XSLValueCompletionModel( dynamic_cast<XSLFileContentParser*>( d->m_thread->parser() ), this );
//		d->m_completerValue->setModel( d->m_completionValueModel );
		
//		d->m_modelData->loadFromContent( textEdit()->toPlainText() );
		d->m_thread->reloadEditorContent( textEdit()->toPlainText() );
	}
}

QAbstractItemModel * FileTypeXsl::model() {
	return d->m_contentModel;
}

void FileTypeXsl::complete() {
	QTextCursor cursor = textEdit()->textCursor();
	
	QRect cr = textEdit()->cursorRect();
	QString completionPrefix = textEdit()->textUnderCursor(cursor);

	QCompleter * c = d->currentCompleter( textEdit()->textCursor() );

	if( c ) {
		if( completionPrefix != d->m_completerNode->completionPrefix() ) {
		    c->setCompletionPrefix( completionPrefix );
			c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
		}
		cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
		c->complete( cr );
	}
}

