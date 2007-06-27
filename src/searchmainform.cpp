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

#include <assert.h> 

#include <QTextEdit>
#include <QMessageBox>

#include "xmlvisualstudio.h"
#include "texteditor.h"
#include "fileeditor.h"
 
void XMLVisualStudio::on_m_searchNextAct_triggered() {
	assert( m_tabEditors->currentEditor() );
	
	if( TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		TextEditor * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
		QTextDocument * document = textEdit->document();
	
		bool continuer = true;
	
		while( continuer ) {
			continuer = false;

			m_cursorStart = textEdit->textCursor();
		
			bool selectionOnly = ( m_findOptions.searchExtend == ReplaceDialogImpl::FindOptions::SEARCHSELECTION );
			bool backwardSearch = ( m_findOptions.searchDirection == ReplaceDialogImpl::FindOptions::SEARCHUP ) || m_searchInverse;
			
			if( backwardSearch ) 
				m_cursorStart.setPosition( m_cursorStart.selectionStart() );
			else
				m_cursorStart.setPosition( m_cursorStart.selectionEnd() );
	
			QTextDocument::FindFlags flags;
			if( backwardSearch ) flags ^= QTextDocument::FindBackward;
			if( m_findOptions.matchCase ) flags ^= QTextDocument::FindCaseSensitively;	
			if( m_findOptions.wholeWords ) flags ^= QTextDocument::FindWholeWords;
		
			if( ! m_cursorStart.isNull() ) {
				if( m_findOptions.regularExpression ) {
					m_cursorStart = document->find( QRegExp( m_findExpression ), m_cursorStart, flags );
				} else {
					m_cursorStart = document->find( m_findExpression, m_cursorStart, flags );
				}
			}
	
			if( m_cursorStart.isNull() || ( selectionOnly && ( ( !backwardSearch && m_cursorEnd < m_cursorStart ) || ( backwardSearch && m_cursorStart < m_cursorEnd ) ) ) ) {
				if( m_findOptions.toReplace && m_yesToAllReplace ) {
					QMessageBox::information( this, 
								tr("Search/Replace"), 
								tr("%1 occurences of '%2' replaced.").arg( m_nbFindedText ).arg( m_findExpression ), 
								QMessageBox::Ok);
				} else {
					QMessageBox::StandardButton ret = QMessageBox::warning( this, 
								tr("Search/Replace"), 
								tr("%1 occurences of '%2' %3. Return to the beginning of the document ?").arg( m_nbFindedText ).arg( m_findExpression ).arg( m_findOptions.toReplace ? tr("replaced") : tr("finded") ), 
								QMessageBox::Yes | QMessageBox::No);
							
					if( ret == QMessageBox::Yes ) {
						m_findOptions.searchExtend = ReplaceDialogImpl::FindOptions::SEARCHALL;
						m_cursorStart = textEdit->textCursor();
						if( ! backwardSearch ) 
							m_cursorStart.movePosition( QTextCursor::Start );
						else
							m_cursorStart.movePosition( QTextCursor::End );
						m_cursorEnd = QTextCursor();
						
						continuer = true;
					}
				}
				m_nbFindedText = 0;
			} else {
				m_nbFindedText++;
			
				textEdit->setTextCursor( m_cursorStart );

				if( m_findOptions.toReplace ) {
					QMessageBox::StandardButton ret = QMessageBox::Yes;
			

					if(! m_yesToAllReplace) {
	 					QMessageBox messageBoxQuestion( QMessageBox::Question, tr("Application"), tr("Replace this occurence"), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::Cancel, this );
						messageBoxQuestion.setWindowModality( Qt::NonModal );
						messageBoxQuestion.show();
						while( messageBoxQuestion.isVisible() ) qApp->processEvents();
						ret = messageBoxQuestion.standardButton( messageBoxQuestion.clickedButton() );
					}

					switch(ret) {
					case QMessageBox::Yes: 	
						// Replace chaine
						m_cursorStart.insertText( ReplaceDialogImpl::replaceStr( m_findOptions, m_findExpression, m_replaceExpression, m_cursorStart.selectedText() ) );
		
						continuer = true;
						break;
					case QMessageBox::YesToAll: 	
						m_cursorStart.insertText( ReplaceDialogImpl::replaceStr( m_findOptions, m_findExpression, m_replaceExpression, m_cursorStart.selectedText() ) );
						m_yesToAllReplace = true;
						continuer = true;
						break;
					case QMessageBox::No: 	
						continuer = true;
						break;
					default : // Do nothing else
						;
					}
				}
			}
			if( ! m_cursorStart.isNull() )
				textEdit->setTextCursor( m_cursorStart );
		}
	}
	m_yesToAllReplace = false;
}

void XMLVisualStudio::on_m_searchPreviousAct_triggered() {
	m_searchInverse = true;
	on_m_searchNextAct_triggered();
	m_searchInverse = false;
}

void XMLVisualStudio::findFirst(const QString & chaine, const QString & dest, const struct ReplaceDialogImpl::FindOptions & options) {
	if( ! TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) 
		return; // TODO : Error

	m_findExpression    = chaine;
	m_replaceExpression = dest;
	m_findOptions       = options;
	m_yesToAllReplace   = false;
	m_nbFindedText		= 0;
	m_searchInverse		= false;

	bool selectionOnly = ( m_findOptions.searchExtend == ReplaceDialogImpl::FindOptions::SEARCHSELECTION );
	bool backwardSearch = ( m_findOptions.searchDirection == ReplaceDialogImpl::FindOptions::SEARCHUP );

	TextEditor * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();

	m_cursorStart = textEdit->textCursor();
	m_cursorEnd   = QTextCursor();

	int selectionStart = m_cursorStart.selectionStart(),
	    selectionEnd = m_cursorStart.selectionEnd();

	if( m_findOptions.searchFromStart ) {
		m_cursorStart.movePosition( QTextCursor::Start, QTextCursor::MoveAnchor );
		m_findOptions.searchFromStart = false;
	} else 
	if( selectionOnly && ! backwardSearch ) {
		m_cursorStart.setPosition( selectionStart, QTextCursor::MoveAnchor );
		m_cursorEnd   = m_cursorStart;
		m_cursorEnd.setPosition( selectionEnd, QTextCursor::MoveAnchor );
	} else
	if( selectionOnly && backwardSearch ) {
		m_cursorStart.setPosition( selectionEnd, QTextCursor::MoveAnchor );
		m_cursorEnd   = m_cursorStart;
		m_cursorEnd.setPosition( selectionStart, QTextCursor::MoveAnchor );
	} else
	if( backwardSearch ) {
		m_cursorStart.setPosition( selectionStart, QTextCursor::MoveAnchor );
	}
	
	textEdit->setTextCursor( m_cursorStart );
	
	on_m_searchNextAct_triggered();
}

void XMLVisualStudio::on_m_searchAct_triggered() {
	TextEditor * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(false);
	m_findDialog->exec();
}

void XMLVisualStudio::on_m_replaceAct_triggered() {
	TextEditor * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(true);
	m_findDialog->exec();
}


