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

/*!
 * The center widget of the \e TextFileEditor class.
 *
 * This class is based on a QTextEdit or a QPlainTextEdit. This depend
 * on the existance of the define QT_PLAINTEXT at the compilation time.
 * The goal is to remove the QTextEdit. QPlainTextEdit is normally more
 * fast.
 * The possibility of use a QTextEdit is keep in the case of probleme.
 */
class TextEditor : public QT_TEXT_EDITOR {
	Q_OBJECT
	Q_PROPERTY( QString selection READ selection WRITE setSelection )
public:
	//! Create the text editor with the parent \e parent
	TextEditor( QWidget * parent = 0 );
	//! Destroy the text editor.
	virtual ~TextEditor();

	//! Return the current column of the cursor
	int currentColumn();
	//! Return the current row of the cursor
	int currentRow();

	/*!
	 * Return the text who is under the cursor.
	 * \param cursor The cursor to use to look the text.
	 * \param deleteWord If true, \e textUnderCursor will remove the text returned.
	 * \param dot If true '.' is considered in a word
	 */
	QString textUnderCursor( const QTextCursor & cursor, bool deleteWord = false, bool dot = true );
	//! Set the syntax highlighter \e highlighter for the text.
	void setHighlighter( SyntaxHighlighter * highlighter );

	//! Set the completer \e completer to the editor
	virtual void setCompleter( QCompleter * completer );
	//! Return the editor setted.
	virtual QCompleter * completer() { return m_completer; };

	//! Return the content of the current selection or all the text if there is no selection.
	QString selection() const;
public slots:
	//! Set the selection to \e text, or replace all the text if there is no selection.
	void setSelection( QString text );

	//! Insert the selection where the cursor is (and replace the selection if any)
	void insertText( const QString & text );

	//! Update the font (when the configuration change)
	void updateFont();
	//! Set the highlight text with \e text. All text equals are highlighted.
	void setMatchingText( QString text );

	/*! In the editor go to the line \e line. */
	void gotoLine( int line );

	/*! Duplicate the current line or the current selection in the editor. */
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
	/*! Replace the selected text by upper case character the parameter. */
	void upperSelectedText();
	/*! Replace the selected text by lower case character. */
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

	/*! Refresh the text highlighter (in case the cursor position change) */
	void refreshTextHighlighter();
	/*! Highlight all text equals of the current word under the cursor */
	void callTextHighlighter();
signals:
	/*! The user press Ctrl+Space and call the snipet given */
	void needInsertSnipet( QString snipet );
	/*! The model is destroyed or created and need to be updated in the content dialog */
	void modelUpdated( QAbstractItemModel * model );

	//! The user (By pressing Ctrl+Click) search a word in the document or another.
	void searchWord( const QString & word );
protected:
	virtual void paintEvent ( QPaintEvent * event );
	virtual void keyPressEvent( QKeyEvent * e );
	virtual void mouseDoubleClickEvent( QMouseEvent * event );
	virtual void mousePressEvent ( QMouseEvent * e );

	virtual void printWhiteSpaces( QPainter & p );

	/*!
	 * Process to do when a user press a key.
	 * This method is called when the editor ask to add some text automatically. (ie:
	 * close a bracket, ...)
	 */
	virtual bool processKeyPress( QKeyEvent * ) { return true; };
	/*!
	 * Process to do when a user press a key
	 * This method is called when the editor ask to process some shortcut.
	 */
	virtual void localKeyPressExecute( QKeyEvent * e );
protected slots:
	/*! Insert the completion based on the QCompleter */
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
