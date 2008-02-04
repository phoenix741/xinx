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

#ifndef TABEDITOR_H
#define TABEDITOR_H
//
#include <QTabWidget>
//
class Editor;
class QAbstractItemModel;

class TabEditor : public QTabWidget {
	Q_OBJECT
public:
	TabEditor( QWidget * parent = 0 );
	virtual ~TabEditor();
	
	Editor * currentEditor();
	Editor * editor( int index );
	Editor * editor( const QString & filename );
	
	static bool isFileEditor( Editor * editor );

	void newFileEditor( Editor * editor );

	Editor * newFileEditorTxt( const QString & suffix );
	Editor * newFileEditorWS();
	Editor * loadFileEditor( const QString & fileName );

	int getClickedTab();
	
	void setRefreshAction( QAction * action );
	void setSaveAction( QAction * action );
	void setSaveAsAction( QAction * action );
	void setCloseAction( QAction * action );
	void setCopyFileNameAction( QAction * action );
	void setCopyPathAction( QAction * action );
public slots:
	void bookmark();
	void nextBookmark();
	void previousBookmark();
	void clearAllBookmark();

	void copy();
	void cut();
	void paste();
  
	void undo();
	void redo(); 
  
	void selectAll();
  
	void duplicateCurrentLine();

	void moveLineUp();
	void moveLineDown();

	void upperSelectedText();
	void lowerSelectedText();

	void commentSelectedText();
	void uncommentSelectedText();
	
	void indent();
	void unindent();
	void autoindent();

	void complete();	
	void highlightWord();
signals:
	void undoAvailable( bool available );
	void redoAvailable( bool available );
	void copyAvailable( bool available );
	void pasteAvailable( bool available );

	void textAvailable( bool available ); // For move, duplicate line, complete and select all
	void hasTextSelection( bool selection ); // For Upper/Lower Case ; Comment/Uncomment
	
	void modelChanged( QAbstractItemModel * model );
	
	void fileOpened( const QString & filename );
	void setEditorPosition( int, int );
protected:
    bool eventFilter( QObject *obj, QEvent *event );
	virtual void dragEnterEvent( QDragEnterEvent *event );
	virtual void dropEvent( QDropEvent *event );
	virtual void tabRemoved ( int index );
private slots:
	void slotCurrentTabChanged( int );
	void slotModifiedChange( bool );
	
	void slotCursorPositionChanged();
	
	void slotNeedInsertSnipet( const QString & snipet );
	void fileEditorOpen( const QString & name, int line );
private:
	int tabPositionIcon( QPoint point );
	int tabPosition( QPoint point );
	Editor * newFileEditor( const QString & fileName );

	QAction * m_refreshAction;
	QAction * m_saveAction;
	QAction * m_saveAsAction;
	QAction * m_closeAction;
	QAction * m_copyFilenameAction;
	QAction * m_copyPathAction;
	
	int m_clickedItem;
	Editor * m_previous;
};

#endif
