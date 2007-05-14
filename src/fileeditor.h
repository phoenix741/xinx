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

#ifndef __FILEEDITOR_H__
#define __FILEEDITOR_H__

#include "editor.h"

class NumberBar;
class TextEditor;
class QTextEdit;
class QHBoxLayout;
class QVBoxLayout;

class TextProcessor : public QObject {
	Q_OBJECT
public:
	TextProcessor( QTextEdit * widget, XSLProject * project = NULL, QObject * parent = 0 ) : QObject( parent ), m_widget( widget ), m_project( project ) { };
	virtual ~TextProcessor() {};
	
	virtual void commentSelectedText( bool uncomment ) = 0;
	virtual void complete() = 0;
	virtual void keyPressEvent( QKeyEvent *e ) = 0;
	
	void parentKeyPressEvent( QKeyEvent * e );

	virtual QAbstractItemModel * model() = 0;
	
public slots:
	virtual void updateModel() = 0;

protected:
	QTextEdit * textEdit() const { return m_widget; };
	XSLProject * project() const { return m_project; }
	
private:
	QTextEdit * m_widget;
	XSLProject * m_project;
};

class FileEditor : public Editor {
	Q_OBJECT
public:
	FileEditor( QWidget *parent = 0, XSLProject * project = NULL );
	virtual ~FileEditor();

	const QString & getFileName() const;
	virtual QString getTitle() const;
	virtual bool hasName() const;
	
	virtual void loadFile( const QString &fileName = "" );
	virtual bool saveFile( const QString &fileName = "" );
 
	QTextEdit * textEdit() const;
 
	virtual bool canCopy();
	virtual bool canPaste();
	virtual bool canUndo();
	virtual bool canRedo();
	virtual bool isModified();

	virtual QAbstractItemModel * model();
public Q_SLOTS : 
	virtual void undo();
	virtual void redo();

	virtual void cut();
	virtual void copy();
	virtual void paste();

	virtual void selectAll();
	virtual void duplicateCurrentLine();
	virtual void moveLineUp();
	virtual void moveLineDown();
	virtual void uploSelectedText( bool upper = true );
	virtual void commentSelectedText( bool uncomment = false );
	virtual void indent( bool unindent = false );
	virtual void complete();

	virtual void setModified( bool );

Q_SIGNALS:
	void mouseHover( const QString &word );
	void mouseHover( const QPoint &pos, const QString &word );
	
	void selectionAvailable ( bool yes );
	
protected:
	virtual bool eventFilter( QObject *obj, QEvent *event );

protected:
	void setFileName( const QString & name );
  
	QString m_fileName;

	QVBoxLayout * m_vbox;
	QHBoxLayout * m_hbox;
	NumberBar * m_numbers;
	TextEditor * m_view;
};


#endif // __FILEEDITOR_H__
