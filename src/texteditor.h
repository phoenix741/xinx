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

#ifndef __TEXTEDITOR_H__
#define __TEXTEDITOR_H__

// Qt header
#include <QTextEdit>
#include <QPixmap>

class QAbstractItemModel;
class TextProcessor;
class QSyntaxHighlighter;

class TextEditor : public QTextEdit {
	Q_OBJECT
public:
	TextEditor( QWidget * parent = 0 );
	virtual ~TextEditor();

	int currentColumn();
	int currentRow();
	
	void insertText( const QString & text );
public slots:
	void updateFont();
signals:
	void modelUpdated( QAbstractItemModel *  model );
	void needInsertSnipet( QString snipet );
protected:
    virtual void paintEvent ( QPaintEvent * event );
	virtual void keyPressEvent(QKeyEvent *e);
    virtual void mouseDoubleClickEvent( QMouseEvent * event );
	virtual void printWhiteSpaces( QPainter &p );
	QString textUnderCursor( const QTextCursor & cursor, bool deleteWord = false );
private slots:
	void slotCursorPositionChanged();
private:
	void key_home( bool );
	void key_enter();

    QPixmap m_tabPixmap; 
	QPixmap m_spacePixmap; 	
	QColor m_currentLineColor;
};
#endif // __TEXTEDITOR_H__
