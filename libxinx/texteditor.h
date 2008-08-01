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
#include <QPixmap>

class QAbstractItemModel;
class TextProcessor;
class SyntaxHighlighter;
class QCompleter;
class QModelIndex;

#ifdef QT_PLAINTEXT
#	include <QPlainTextEdit>
#	define QT_TEXT_EDITOR QPlainTextEdit
#else
#	include <QTextEdit>
#	define QT_TEXT_EDITOR QTextEdit
#endif // QT_PLAINTEXT

class TextEditor : public QT_TEXT_EDITOR {
	Q_OBJECT
	Q_PROPERTY( QString selection READ selection WRITE setSelection )
public:
	TextEditor( QWidget * parent = 0 );
	virtual ~TextEditor();

	int currentColumn();
	int currentRow();

	QString textUnderCursor( const QTextCursor & cursor, bool deleteWord = false, bool dot = true );
	void setHighlighter( SyntaxHighlighter * highlighter );

	virtual void setCompleter( QCompleter * completer );
	virtual QCompleter * completer() { return m_completer; };

	QString selection() const;
public slots:
	void setSelection( QString text );

	void insertText( const QString & text );

	void updateFont();
	void setMatchingText( QString text );

	/*!
	 * In the editor go to the line \e line.
	 */
	void gotoLine( int line );

	/*!
	 * Duplicate the current line in the editor.
	 * \todo Duplicate more than one line. (duplicate the selected text).
	 */
	void duplicateLines();
	/*!
	 * Move the current line up.
	 * Swap the current line with the line above.
	 * \sa moveLineDown()
	 */
	void moveLineUp();
	/*!
	 * Move the current line down.
	 * Swap the current line with the line after.
	 * \sa moveLineUp()
	 */
	void moveLineDown();
	/*!
	 * Replace the selected text by upper case character the parameter.
	 */
	void upperSelectedText();
	/*!
	 * Replace the selected text by lower case character.
	 */
	void lowerSelectedText();
	/*!
	 * Called when the configuration change and it's necessary to update the highlighter.
	 * If no highlighter is used, this function do nothing.
	 */
	void updateHighlighter();

	/*!
	 * Indent or unindent the selected text depending on the parameter.
	 * \param unindent If false (by default) the text is indented. (the character \\t is added), else the text is unindented.
	 */
	void indent( bool unindent = false );

	/*!
	 * Comment or Uncomment the selected text depending on the parrameter.
	 * If a part of a text is already (un)commented, the balise is moved to comment all the text.
	 * Warning: If you comment code with comment, the comment can be merged with code.
	 * \param uncomment If false (by default) the text is commented, else the text is uncommented
	 */
	virtual void commentSelectedText( bool uncomment = false );

	void refreshTextHighlighter();
	void callTextHighlighter();
signals:
	void needInsertSnipet( QString snipet );
	void modelUpdated( QAbstractItemModel * model );

	void searchWord( const QString & word );
protected:
	virtual void paintEvent ( QPaintEvent * event );
	virtual void keyPressEvent( QKeyEvent * e );
    virtual void mouseDoubleClickEvent( QMouseEvent * event );
	virtual void mousePressEvent ( QMouseEvent * e );

	virtual void printWhiteSpaces( QPainter & p );

	virtual bool processKeyPress( QKeyEvent * ) { return true; };
	virtual void localKeyPressExecute( QKeyEvent * e );
protected slots:
	virtual void insertCompletion( const QModelIndex& index );
private slots:
	void slotCursorPositionChanged();
private:

	void uploSelectedText( bool upper = true );
	void key_home( bool );
	void key_enter();

	SyntaxHighlighter* m_syntaxhighlighter;
	QString m_matchedText;
    QPixmap m_tabPixmap, m_spacePixmap;
	QColor m_currentLineColor;
	QCompleter * m_completer;

	friend class NumberBar;
};

#endif // __TEXTEDITOR_H__
