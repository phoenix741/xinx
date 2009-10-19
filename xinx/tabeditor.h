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

#ifndef TABEDITOR_H
#define TABEDITOR_H
#pragma once

// Xinx header
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>

// Qt header
#include <QTabWidget>

class AbstractEditor;
class QAbstractItemModel;
class IFileTypePlugin;
class QModelIndex;

class TabEditor : public QTabWidget, public EditorManager {
	Q_OBJECT
public:
	TabEditor( QWidget * parent = 0 );
	virtual ~TabEditor();

	void newEditor( IFileTypePlugin * interface = 0 );
	void openFilename( const QString & filename );

	virtual AbstractEditor * currentEditor() const;
	virtual AbstractEditor * editor( int index ) const;
	virtual AbstractEditor * editor( const QString & filename ) const;
	virtual QList<AbstractEditor*> editors() const;

	virtual int editorsCount() const;

	virtual void changeToNextEditor();
	virtual void changeToPreviousEditor();
	virtual void changeToEditor( int index );

	static bool isTextFileEditor( AbstractEditor * editor );

	int getClickedTab();

	void setRefreshAction( QAction * action );
	void setSaveAction( QAction * action );
	void setSaveAsAction( QAction * action );
	void setCloseAction( QAction * action );
	void setCopyFileNameAction( QAction * action );
	void setCopyPathAction( QAction * action );

	void updateTabWidget( AbstractEditor * editor );

	void addTab( AbstractEditor * editor );
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
	void positionChanged( const QModelIndex & index );

	void fileOpened( const QString & filename );
	void setEditorPosition( int, int );
	void contentChanged();

	void messageTranslation( const QString & filename, const QString & message, AbstractEditor::LevelMessage level );
	void clearMessageTranslation( const QString & filename );
protected:
	bool eventFilter( QObject *obj, QEvent *event );
	virtual void dragEnterEvent( QDragEnterEvent *event );
	virtual void dropEvent( QDropEvent *event );
	virtual void tabRemoved ( int index );
private slots:
	void slotCurrentTabChanged( int );
	void slotModifiedChange();

	void slotCursorPositionChanged();

	void fileEditorOpen( const QString & name, int line );
private:
	void connectEditor( AbstractEditor * editor );

	int tabPositionIcon( QPoint point );
	int tabPosition( QPoint point );

	QAction * m_refreshAction;
	QAction * m_saveAction;
	QAction * m_saveAsAction;
	QAction * m_closeAction;
	QAction * m_copyFilenameAction;
	QAction * m_copyPathAction;

	int m_clickedItem;
};

#endif
