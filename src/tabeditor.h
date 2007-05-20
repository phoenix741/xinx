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
class XSLProject;
class WebServices;

typedef QList<WebServices*> WebServicesList;

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
	Editor * newFileEditorTxt( XSLProject * project = NULL );
	Editor * newFileEditorXML( XSLProject * project = NULL );
	Editor * newFileEditorXSL( XSLProject * project = NULL );
	Editor * newFileEditorJS( XSLProject * project = NULL );
	Editor * newFileEditorWS( XSLProject * project = NULL, WebServicesList * services = NULL );
	Editor * loadFileEditor( const QString & fileName, XSLProject * project = NULL, WebServicesList * services = NULL );

public slots:
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

	void complete();	
Q_SIGNALS:
	void undoAvailable( bool available );
	void redoAvailable( bool available );
	void copyAvailable( bool available );
	void pasteAvailable( bool available );

	void textAvailable( bool available ); // For move, duplicate line, complete and select all
	void hasTextSelection( bool selection ); // For Upper/Lower Case ; Comment/Uncomment
	
	void modelCreated();
	void modelDeleted();
	
	void fileDragged();
	
	void closeTab( int );
	void refreshTab( int );
	void saveTab( int );
	void saveAsTab( int );
	
	void setEditorPosition( int, int );

protected:
    bool eventFilter( QObject *obj, QEvent *event );
	void dragEnterEvent( QDragEnterEvent *event );
	void dropEvent( QDropEvent *event );
	void tabRemoved ( int index );
	
private slots:
	void slotCurrentTabChanged( int );
	void slotModifiedChange( bool );
	
	void slotCloseAsked();
	void slotRefreshAsked();
	void slotSaveAsked();
	void slotSaveAsAsked();
	
private:
	int m_clickedItem;

	Editor * previous;
};

#endif
