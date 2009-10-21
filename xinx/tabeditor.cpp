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

// Xinx header
#include "tabeditor.h"
#include <project/xinxproject.h>
#include <core/xinxconfig.h>
#include <plugins/xinxpluginsloader.h>
#include <editors/editorfactory.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>

// Qt header
#include <QApplication>
#include <QDir>
#include <QTabBar>
#include <QDragEnterEvent>
#include <QUrl>
#include <QMenu>
#include <QMessageBox>
#include <QDesktopWidget>

// Std header
#include <typeinfo>

//
TabEditor::TabEditor( QWidget * parent ) : QTabWidget( parent ), m_refreshAction(0), m_saveAction(0), m_saveAsAction(0), m_closeAction(0), m_clickedItem( -1 ) { //, m_previous(NULL) {
	setAcceptDrops(true);

	tabBar()->installEventFilter(this);
	tabBar()->setAttribute( Qt::WA_Hover );

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)) );
}
//
TabEditor::~TabEditor() {
}
//

bool TabEditor::isTextFileEditor( AbstractEditor * editor ) {
	return dynamic_cast<TextFileEditor*>(editor) != NULL;
}

AbstractEditor * TabEditor::currentEditor() const {
	return dynamic_cast<AbstractEditor*>( currentWidget() );
}

AbstractEditor * TabEditor::editor( int index ) const {
	return dynamic_cast<AbstractEditor*>( widget( index ) );
}

AbstractEditor * TabEditor::editor( const QString & filename ) const {
	for( int i = 0; i < count(); i++ ) {
		AbstractEditor * ed = dynamic_cast<AbstractEditor*>( editor(i) );
		if( ed && ( ed->lastFileName() == QDir::fromNativeSeparators( filename ) ) )
			return editor(i);
	}
	return NULL;
}

QList<AbstractEditor*> TabEditor::editors() const {
	QList<AbstractEditor*> list;
	for( int i = 0 ; i < count() ; i++ ) {
		list << editor( i );
	}
	return list;
}

int TabEditor::editorsCount() const {
	return count();
}

void TabEditor::changeToNextEditor() {
	setCurrentIndex( ( currentIndex() + 1 ) % count() );
}

void TabEditor::changeToPreviousEditor() {
	setCurrentIndex( ( currentIndex() - 1 + count() ) % count() );
}

void TabEditor::changeToEditor( int index ) {
	setCurrentIndex( index );
}

void TabEditor::addTab( AbstractEditor * editor ) {
	Q_ASSERT( editor );

	connect( editor, SIGNAL(open(QString,int)), this, SLOT(fileEditorOpen(QString,int)) );
	connect( editor, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()) );
	connect( editor, SIGNAL(modificationChanged(bool)), this, SLOT(slotModifiedChange()) );
	connect( editor, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)) );
	connect( editor, SIGNAL(pasteAvailable(bool)), this, SIGNAL(pasteAvailable(bool)) );
	connect( editor, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)) );
	connect( editor, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)) );
	connect( editor, SIGNAL(message(QString,QString,AbstractEditor::LevelMessage)), this, SIGNAL(messageTranslation(QString,QString,AbstractEditor::LevelMessage)) );
	connect( editor, SIGNAL(clearMessage(QString)), this, SIGNAL(clearMessageTranslation(QString)) );

	if( isTextFileEditor( editor ) ) {
		connect( editor, SIGNAL( selectionAvailable(bool) ), this, SIGNAL( hasTextSelection(bool) ) );
		connect( qobject_cast<TextFileEditor*>( editor )->textEdit()->editor(), SIGNAL( cursorPositionChanged() ), this, SLOT( slotCursorPositionChanged() ) );
		if( editor->metaObject()->indexOfSignal( SIGNAL(positionInEditorChanged(QModelIndex)) ) >= 0 )
			connect( editor, SIGNAL(positionInEditorChanged(QModelIndex)), this, SIGNAL( positionChanged(QModelIndex) ) );
	}

	int index = QTabWidget::addTab( editor, QString() );
	updateTabWidget( editor );

	setCurrentIndex( index );
	emit currentChanged( currentIndex() );
}

void TabEditor::newEditor( IFileTypePlugin * interface ) {
	AbstractEditor * editor = EditorFactory::self()->createEditor( interface );
	addTab( editor );

	setCurrentWidget( editor );
	dynamic_cast<QWidget*>( parent() )->activateWindow();
	emit currentChanged( currentIndex() );
}

void TabEditor::openFilename( const QString & filename ) {
	Q_ASSERT( ! filename.isEmpty() );

	AbstractEditor * editor = TabEditor::editor( filename );
	if( ! editor ) {
		editor = EditorFactory::self()->createEditor( filename );

		addTab( editor );
	}

	setCurrentWidget( editor );
	dynamic_cast<QWidget*>( parent() )->activateWindow();
	emit currentChanged( currentIndex() );
}

void TabEditor::updateTabWidget( AbstractEditor * editor ) {
	Q_ASSERT( indexOf( editor ) != -1 );

	const QFontMetrics & metrics = fontMetrics();

	int index = indexOf( editor );
	int width = qApp->desktop()->screenGeometry().width();

	QString title = editor->getTitle();
	QString longTitle = metrics.elidedText( editor->getLongTitle(), Qt::ElideLeft, width );

	if( editor->isModified() ) title += "*";

	setTabText( index, title );
	setTabToolTip( index, longTitle );
	setTabIcon( index, editor->icon() );
}

void TabEditor::fileEditorOpen( const QString & name, int line ) {
	if( !name.isEmpty() )
		openFilename( name );

	emit fileOpened( name );

	// Deplace to rigth line.
	TextFileEditor * ed = qobject_cast<TextFileEditor*>( currentEditor() );
	ed->textEdit()->gotoLine( line );
	ed->textEdit()->setFocus( Qt::OtherFocusReason );
}

void TabEditor::copy() {
	AbstractEditor * editor = currentEditor();
	if( editor && editor->canCopy() )
		editor->copy();
}

void TabEditor::cut() {
	AbstractEditor * editor = currentEditor();
	if( editor && editor->canCopy() )
		editor->cut();
}

void TabEditor::paste() {
	AbstractEditor * editor = currentEditor();
	if( editor && editor->canPaste() )
		editor->paste();
}

void TabEditor::undo() {
	AbstractEditor * editor = currentEditor();
	if( editor && editor->canUndo() )
		editor->undo();
}

void TabEditor::redo() {
	AbstractEditor * editor = currentEditor();
	if( editor && editor->canRedo() )
		editor->redo();
}

void TabEditor::selectAll() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->selectAll();
	}
}

void TabEditor::duplicateCurrentLine() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->textEdit()->duplicateLines();
	}
}

void TabEditor::moveLineUp() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->textEdit()->moveLineUp();
	}
}

void TabEditor::moveLineDown() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->textEdit()->moveLineDown();
	}
}

void TabEditor::upperSelectedText() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) && currentEditor()->canCopy() ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->textEdit()->upperSelectedText();
	}
}

void TabEditor::lowerSelectedText() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) && currentEditor()->canCopy() ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->textEdit()->lowerSelectedText();
	}
}

void TabEditor::indent() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->textEdit()->indent();
	}
}

void TabEditor::unindent() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->textEdit()->indent( true );
	}
}

void TabEditor::autoindent() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->autoIndent();
	}
}


void TabEditor::complete() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->complete();
	}
}

void TabEditor::highlightWord() {
	if( currentEditor() && isTextFileEditor( currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( currentEditor() );
		editor->textEdit()->callTextHighlighter();
	}
}

void TabEditor::dragEnterEvent( QDragEnterEvent *event ) {
	const QMimeData *mimeData = event->mimeData();
	if (mimeData->hasUrls()) {
		setBackgroundRole(QPalette::Highlight);
		event->acceptProposedAction();
	}
}

void TabEditor::dropEvent( QDropEvent *event ) {
	const QMimeData *mimeData = event->mimeData();
	const QList<QUrl> & urls = mimeData->urls();
	if (mimeData->hasUrls()) {
		for(int i = 0; i < urls.size(); i++) {
			if((!urls.at(i).toLocalFile().isEmpty()))
				openFilename( urls.at(i).toLocalFile() );
				emit fileOpened( urls.at(i).toLocalFile() );
		}

		setBackgroundRole(QPalette::NoRole);
		event->acceptProposedAction();
	}
}

void TabEditor::tabRemoved ( int index ) {
	Q_UNUSED( index );

	if( count() == 0 ) {
		emit copyAvailable( false );
		emit pasteAvailable( false );
		emit undoAvailable( false );
		emit redoAvailable( false );
		emit textAvailable( false );
		emit hasTextSelection( false );
		emit modelChanged( NULL );
		emit currentChanged( -1 );
	}
}

void TabEditor::slotModifiedChange() {
	Q_ASSERT( currentIndex() >= 0 );
	Q_ASSERT( currentEditor() );

	updateTabWidget( currentEditor() );
}

void TabEditor::slotCursorPositionChanged() {
	AbstractEditor * editor = currentEditor();
	emit setEditorPosition( qobject_cast<TextFileEditor*>( editor )->textEdit()->currentRow(), qobject_cast<TextFileEditor*>( editor )->textEdit()->currentColumn() );
}

void TabEditor::slotCurrentTabChanged( int index ) {
	if( index == -1 ) return;

	AbstractEditor * ed = editor( index );

	emit copyAvailable( ed->canCopy() );
	//	emit pasteAvailable( ed->canPaste() );
	emit pasteAvailable( true );
	emit undoAvailable( ed->canUndo() );
	emit redoAvailable( ed->canRedo() );

	if( isTextFileEditor( ed ) ) {
		emit textAvailable( true );
		emit setEditorPosition( qobject_cast<TextFileEditor*>( ed )->textEdit()->currentRow(), qobject_cast<TextFileEditor*>( ed )->textEdit()->currentColumn() );
	} else {
		emit textAvailable( false );
		emit hasTextSelection( false );
	}

	if( ed )
		emit modelChanged( ed->model() );
	else
		emit modelChanged( 0 );
}

int TabEditor::getClickedTab() {
	int item = m_clickedItem;
	m_clickedItem = -1;
	return item;
}

void TabEditor::setRefreshAction( QAction * action ) {
	m_refreshAction = action;
}

void TabEditor::setSaveAction( QAction * action ) {
	m_saveAction = action;
}

void TabEditor::setSaveAsAction( QAction * action ) {
	m_saveAsAction = action;
}

void TabEditor::setCloseAction( QAction * action ) {
	m_closeAction = action;
}

void TabEditor::setCopyFileNameAction( QAction * action ) {
	m_copyFilenameAction = action;
}

void TabEditor::setCopyPathAction( QAction * action ) {
	m_copyPathAction = action;
}

int TabEditor::tabPositionIcon( QPoint point ) {
	int clickedItem = -1;

	for ( int i = 0; i < tabBar()->count(); i++ ) {
		QRect rect = tabBar()->tabRect( i );
		rect.moveTo( QPoint( 5 + rect.left(), 5 + rect.top() ) );
		rect.setSize( tabBar()->iconSize() );
		if ( rect.contains( point ) ) {
			clickedItem = i;
			break;
		}
	}

	return clickedItem;
}

int TabEditor::tabPosition( QPoint point ) {
	int clickedItem = -1;

	for ( int i = 0; i < tabBar()->count(); i++ ) {
		if ( tabBar()->tabRect( i ).contains( point ) ) {
			clickedItem = i;
			break;
		}
	}

	return clickedItem;
}

bool TabEditor::eventFilter( QObject *obj, QEvent *event ) {
	if ( obj==tabBar() ) {
        if( event->type() == QEvent::Leave ) {
        	for( int i = 0 ; i < tabBar()->count(); i++ )
        		setTabIcon( i, editor( i )->icon() );

			return true;
		} else
		if( ( event->type() == QEvent::HoverMove ) && XINXConfig::self()->config().editor.closeButtonOnEachTab ) {
        	for( int i = 0 ; i < tabBar()->count(); i++ )
        		setTabIcon( i, editor( i )->icon() );

			QHoverEvent *mouseEvent = static_cast<QHoverEvent *>(event);
        	m_clickedItem = tabPositionIcon( mouseEvent->pos() );
			if( m_clickedItem == -1 ) return QTabWidget::eventFilter( obj, event );

			setTabIcon( m_clickedItem, QIcon( ":/images/fileclose.png" ) );

			return true;
		} else
		if( ( event->type() == QEvent::MouseButtonPress ) && ( static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton ) && XINXConfig::self()->config().editor.closeButtonOnEachTab ) {
        	QMouseEvent * mouseEvent = static_cast<QMouseEvent *>( event );
        	m_clickedItem = tabPositionIcon( mouseEvent->pos() );
			if( m_clickedItem == -1 ) return QTabWidget::eventFilter( obj, event );

			m_closeAction->trigger();
		} else
 		if ( ( ( event->type() == QEvent::MouseButtonPress ) && ( static_cast<QMouseEvent *>(event)->button() == Qt::RightButton ) ) || ( event->type() == QEvent::MouseButtonDblClick ) ) {
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			m_clickedItem = tabPosition( mouseEvent->pos() );
			if( m_clickedItem == -1 ) return QTabWidget::eventFilter( obj, event );

			if ( ( event->type() == QEvent::MouseButtonPress ) && ( mouseEvent->button() == Qt::RightButton ) ) {
				QMenu *menu = new QMenu( this );
				menu->addAction( m_refreshAction );
				menu->addSeparator();
				menu->addAction( m_saveAction );
				menu->addAction( m_saveAsAction );
				menu->addSeparator();
				menu->addAction( m_closeAction );
				menu->addSeparator();
				menu->addAction( m_copyFilenameAction );
				menu->addAction( m_copyPathAction );
				menu->exec(mouseEvent->globalPos());
				delete menu;
			} else
			if ( ( event->type() == QEvent::MouseButtonDblClick ) && ( mouseEvent->button() == Qt::LeftButton ) ) {
				m_closeAction->trigger();
			}
			return true;
		}
	}
	return QTabWidget::eventFilter( obj, event );
}

void TabEditor::bookmark() {
	currentEditor()->bookmarkInterface()->toogledBookmark();
}

void TabEditor::nextBookmark() {
	if( ( ! currentEditor()->bookmarkInterface()->nextBookmark() ) && ( currentIndex() < count() ) ) {
		int index = currentIndex();
		do { index ++ ; } while( ( index < count() ) && ( editor( index )->bookmarkInterface()->bookmarkCount() == 0 ) );
		if( index < count() ) {
			setCurrentIndex( index );
			currentEditor()->bookmarkInterface()->firstBookmark();
		}
	}
}

void TabEditor::previousBookmark() {
	if( ( ! currentEditor()->bookmarkInterface()->previousBookmark() ) && ( currentIndex() > 0 ) ) {
		int index = currentIndex();
		do { index -- ; } while( ( index >= 0 ) && ( editor( index )->bookmarkInterface()->bookmarkCount() == 0 ) );
		if( index >= 0 ) {
			setCurrentIndex( index );
			currentEditor()->bookmarkInterface()->lastBookmark();
		}
	}
}

void TabEditor::clearAllBookmark() {
	for( int i = 0; i < count() ; i++ ) {
		editor( i )->bookmarkInterface()->clearAllBookmark();
	}
}

