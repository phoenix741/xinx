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

#ifndef _XINXCODEEDIT_H_
#define _XINXCODEEDIT_H_

// Qt header
#include <QWidget>

// QCodeEdit header
#include <qdocumentcursor.h>
#include <qeditor.h>

class QDocumentSearch;
class QAbstractItemModel;
class QCodeEdit;
class QCompleter;
class QModelIndex;
class XINXConfig;

/*!
 * XinxCodeEdit is a wrapper of a QCodeEdit editor. This editor redefine different function
 * used in QTextEdit and TextEditor.
 *
 * \todo add method : undo(), redo(), cut(), copy(), paste()
 * \todo delete \e find method, we use QDocumentSearch.
 */
class XinxCodeEdit : public QWidget, private QEditor::InputBinding {
	Q_OBJECT
	Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly );
	Q_PROPERTY( QString selection READ selection WRITE setSelection  )
	Q_PROPERTY( QString plainText READ toPlainText WRITE setPlainText )
	Q_PROPERTY( int row READ currentRow )
	Q_PROPERTY( int column READ currentColumn )
	Q_PROPERTY( int tabStopWidth READ tabStopWidth WRITE setTabStopWidth )
	Q_PROPERTY( bool modified WRITE setModified )
public:
	//! XinxCodeEdit::FindFlag is used in find function.
	enum FindFlag { FindBackward = 0x01, FindCaseSensitively = 0x02, FindWholeWords = 0x04 };
	Q_DECLARE_FLAGS( FindFlags, FindFlag );

	/*!
	 * Create a XinxCodeEdit object.
	 *
	 * This create the QCodeEdit object with some default option.
	 */
	XinxCodeEdit( QWidget * parent = 0 );

	/*!
	 * Create a XinxCodeEdit object and active action for the editor.
	 *
	 * This create the QCodeEdit object with some default option.
	 */
	XinxCodeEdit( bool action, QWidget * parent = 0 );

	//! Destroy the object
	virtual ~XinxCodeEdit();

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
	QString textUnderCursor( const QDocumentCursor & cursor, bool deleteWord = false, bool dot = true );

	//! Return the content of the current selection or all the text if there is no selection.
	QString selection() const;

	//! Return the content of the editor
	QString toPlainText() const;

	//! Return the QCodeEdit editor
	QEditor * editor() const;

	//! Returns a pointer to the underlying document.
	QDocument * document() const;

	//! Set the completer \e completer to the editor
	virtual void setCompleter( QCompleter * completer );
	//! Return the editor setted.
	virtual QCompleter * completer() { return m_completer; };

	/*!
	 * Set the number of space for a tabulation.
	 * Warning: This value is global in the application. This is a limitation of QCodeEdit.
	 *
	 * \sa tabStopWidth()
	 */
	void setTabStopWidth( int width );
	/*!
	 * Get the number of space for a tabulation.
	 * Warning: This value is global in the application. This is a limitation of QCodeEdit.
	 *
	 * \sa setTabStopWidth()
	 */
	int tabStopWidth() const;

	//! Returns whether text can be pasted from the clipboard into the textedit.
	bool canPaste();

	//! Finds the next occurrence of the string \e exp, using the given \e options. Returns true if \e exp was found and changes the cursor to select the match; otherwise returns false.
	bool find( const QString & exp, FindFlags options = 0 );

	/*!
	 * Finds the next occurrence of the string, \e subString, in the document. The search starts at
	 * the position of the given \e cursor, and proceeds forwards through the document unless
	 * specified otherwise in the search options. The \e options control the type of search performed.
	 *
	 * Returns a cursor with the match selected if \e subString was found; otherwise returns a
	 * null cursor.
	 *
	 * If the given cursor has a selection, the search begins after the selection; otherwise it
	 * begins at the cursor's position.
	 *
	 * By default the search is case-sensitive, and can match text anywhere in the document.
	 */
	QDocumentCursor find ( const QString & subString, const QDocumentCursor & cursor, FindFlags options = 0 ) const;
	/*!
	 * Finds the next occurrence, matching the regular expression \e expr, in the document.
	 *
	 * The search starts at the position of the given \e cursor, and proceeds forwards through
	 * the document unless specified otherwise in the search \e options. The options control
	 * the type of search performed. The FindCaseSensitively option is ignored for this overload,
	 * use QRegExp::caseSensitivity instead.
	 *
	 * Returns a cursor with the match selected if a match was found; otherwise returns a null
	 * cursor.
	 *
	 * If the given cursor has a selection, the search begins after the selection; otherwise
	 * it begins at the cursor's position.
	 *
	 * By default the search is case-sensitive, and can match text anywhere in the document.
	 */
	QDocumentCursor find ( const QRegExp & expr, const QDocumentCursor & cursor, FindFlags options = 0 ) const;

	/*!
	 * Moves the cursor by performing the given operation.
	 *
	 * If mode is QTextCursor::KeepAnchor, the cursor selects the text it moves over.
	 * This is the same effect that the user achieves when they hold down the Shift key
	 * and move the cursor with the cursor keys.
	 */
	void moveCursor( QDocumentCursor::MoveOperation operation, QDocumentCursor::MoveMode mode = QDocumentCursor::MoveAnchor );
	/*!
	 * Convenience function to print the text edit's document to the given printer.
	 * This is equivalent to calling the print method on the document directly except
	 * that this function also supports QPrinter::Selection as print range.
	 *
	 * \todo not implemented
	 */
	void print( QPrinter * printer ) const;
	/*!
	 * Sets the visible cursor.
	 * \sa textCursor()
	 */
	void setTextCursor( const QDocumentCursor & cursor );
	/*!
	 * Returns a copy of the QTextCursor that represents the currently visible cursor.
	 * Note that changes on the returned cursor do not affect QTextEdit's cursor;
	 * use setTextCursor() to update the visible cursor.
	 *
	 * \sa setTextCursor()
	 */
	QDocumentCursor textCursor() const;

	/*! Clear all bookrmark */
	void clearBookmark();

	/*! Previous mark */
	bool previousBookmark();

	/*! Next bookmark */
	bool nextBookmark();

	/*! Show icon bookmark at line \e line. */
	void setBookmark( int line, bool enabled );

	/*! List of bookmark of the editor. */
	QList<int> listOfBookmark();

	/*! Set the list of error. */
	void setErrors( QList<int> errors );

	/*! Return true if the editor is read only */
	bool isReadOnly() const;
public slots:
	//! Change the highlighter to \e highlighter
	void setHighlighter( const QString & highlighter );

	//! Change the highlighter to \e highlighter but with color settings of \e config
	void setHighlighter( const QString & highlighter, XINXConfig * config );

	//! Change the read only property of the editor
	void setReadOnly( bool readonly );

	//! Change the document state (modified or not) of the document
	void setModified( bool modified );

	/*!
	 * Called when the configuration change and it's necessary to update the highlighter.
	 * If no highlighter is used, this function do nothing.
	 */
	void updateHighlighter();

	//! Update the font (when the configuration change)
	void updateFont();

	//! Set the text to \e text
	void setPlainText( const QString & text );

	//! Set the selection to \e text, or replace all the text if there is no selection.
	void setSelection( QString text );

	//! Insert the selection where the cursor is (and replace the selection if any). This method indent the text.
	void insertText( const QString & text );

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

	//! Set the highlight text with \e text. All text equals are highlighted.
	void setMatchingText( QString text );

	//! Refresh the text highlighter (in case the cursor position change)
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

	/*!
	 * Signal is emitted when a bookmark is toogled.
	 * \param line Line of the bookmark
	 * \param enabled If enabled, the bookmark is added.
	 */
	void bookmarkToggled( int line, bool enabled );

	//! The signal is emited when the state change. \e y is true if copy is available
	void copyAvailable(bool y);

	//! The signal is emited when the state change. \e y is true if undo is available
	void undoAvailable(bool y);
	//! The signal is emited when the state change. \e y is true if redo is available
	void redoAvailable(bool y);
protected slots:
	/*! Insert the completion based on the QCompleter */
	virtual void insertCompletion( const QModelIndex& index );
protected:
	virtual QString id() const;
	virtual QString name() const;

	virtual bool keyPressEvent( QKeyEvent * event, QEditor * editor );
	virtual void postKeyPressEvent( QKeyEvent * event, QEditor * editor );
	virtual void postMousePressEvent( QMouseEvent * event, QEditor * editor );

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
	virtual bool localKeyPressExecute( QKeyEvent * e );
private slots:
	void slotMarkChanged( QDocumentLineHandle* line, int type, bool enabled );
private:
	void init( bool action );
	void uploSelectedText( bool upper = true );
	void key_home( bool );

	QCodeEdit * m_editor;
	QDocumentSearch * m_matchingText;
	QCompleter * m_completer;
	QString m_matchingTextString;
};

/*!
 * XinxCodeEditAction is a wrapper of QCodeEdit editor. This editor active
 * action in QCodeEdit.
 */
class XinxCodeEditAction : public XinxCodeEdit {
	Q_OBJECT
public:
	/*!
	 * Create a XinxCodeEdit object.
	 *
	 * This create the QCodeEdit object with some default option.
	 */
	XinxCodeEditAction( QWidget * parent = 0 );
};

#endif /* _XINXCODEEDIT_H_ */
