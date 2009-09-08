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

#ifndef __FILEEDITOR_H__
#define __FILEEDITOR_H__
#pragma once

// Xinx headeur
#include <editors/abstracteditor.h>
#include <plugins/plugininterfaces.h>

// Qt header
#include <QTimer>

// QCodeEdit header
#include <qdocumentcursor.h>

class XinxCodeEdit;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class QAction;
class FileWatcher;
class SyntaxHighlighter;


/*!
 * Class used to represent a file editor. A file editor have a XinxCodeEditor class, who show the line in the document,
 * folding, ....
 * The File Editor is a subclass of Editor and implements all its pure virtual method.
 *
 * The File Editor propose a method to read the FileName : getFileName(), and the default suffix (for save as) to use (getSuffix).
 * If a file is open, getTitle() return the file name otherwise it return noname.
 *
 * FileEditor has also two methode for load and save file.
 */
class TextFileEditor : public AbstractEditor {
	Q_OBJECT
public:
	/*! Type of End Of Line that the editor can understand. */
	enum EndOfLineType {
		WindowsEndOfLine, //!< The end of line is terminated by \\r\\n
		UnixEndOfLine,    //!< The end of line is terminated by \\n
		MacEndOfLine      //!< The end of line is terminated by \\r\\n
	};
	/*!
	 * Construct a FileEditor with the help of a TextEditor and a parent.
	 * \param editor TextEditor to use to print file to screen (center widget)
	 * \param parent Parent of the editor.
	 */
	TextFileEditor( XinxCodeEdit * editor = 0, QWidget *parent = 0 );
	/*! Destructor of the FileEditor.*/
	virtual ~TextFileEditor();

 	/*!
 	 * Return the text editor corresponding on the file editor. This editor can't be null.
 	 * \return The text editor widget used by the file editor.
 	 */
	virtual XinxCodeEdit * textEdit() const;
	virtual QString defaultFileName() const;

	/*!
	 * Return the action used to comment text in the editor. This action call the \e commentSelectedText() with
	 * no parameters.
	 * \return Return the action to comment.
	 * \sa commentSelectedText()
	 */
	QAction * commentAction();
	/*!
	 * Return the action used to uncomment text in the editor. This action call the \e commentSelectedText().with
	 * true in the parameters
	 * \return Return the action to uncomment.
	 * \sa uncommentSelectedText()
	 */
	QAction * uncommentAction();

	/*!
	 * List of bookmarks of the editor
	 */
	virtual QList<int> bookmarks() const;

	virtual void loadFromDevice( QIODevice & d );
	virtual void saveToDevice( QIODevice & d );

	//! The default implementation return null.
	virtual QAbstractItemModel * model()  const;

	//! The codec used to read and write the file. By Default, the codec is defined in options.
	virtual QTextCodec * codec() const;
	/*!
	 * Return the EndOfLine of the document. This can't be modified.
	 * A newly created editor is in platform end of line type. Saving a file converte the
	 * EndOfLine to the platform behavour.
	 */
	virtual EndOfLineType eol() const;

	virtual bool canCopy();
	virtual bool canPaste();
	virtual bool canUndo();
	virtual bool canRedo();

	virtual void serialize( XinxProjectSessionEditor * data, bool content );
	virtual void deserialize( XinxProjectSessionEditor * data );

	void setBookmark( int line, bool enabled );
	virtual void toogledBookmark();
	virtual void gotoBookmarkAt( int i );
	virtual QString bookmarkAt( int i );
	virtual int bookmarkCount();
	virtual bool previousBookmark();
	virtual bool nextBookmark();
	virtual void clearAllBookmark();
public slots :
	virtual void initSearch( SearchOptions & options );
	virtual bool find( const QString & text, SearchOptions options );
	virtual void replace( const QString & from, const QString & to, SearchOptions options );

	//! The default implementation do nothing
	virtual void updateModel();

	virtual void undo();
	virtual void redo();

	virtual void cut();
	virtual void copy();
	virtual void paste();

	/*! Method used to select all the text in the editor. The call is sent to the TextEditor. */
	void selectAll();
	/*!
	 * Comment or Uncomment the selected text depending on the parrameter.
	 * If a part of a text is already (un)commented, the balise is moved to comment all the text.
	 * Warning: If you comment code with comment, the comment can be merged with code.
	 * \param uncomment If false (by default) the text is commented, else the text is uncommented
	 */
	void commentSelectedText( bool uncomment = false );
	/*! Auto indent all the document (named Pretty Print). */
	virtual bool autoIndent();
	/*! Call the completer of the text on the current position of the cursor, if possible. */
	virtual void complete();

	virtual void searchWord( const QString & word );
signals:
	/*!
	 * Signal emitted when a text is selected or deselected.
	 * \param yes The value is true if a part of the text is selected, else the value is false.
	 */
	void selectionAvailable ( bool yes );

protected slots:
	/*! Slot called when the bookmark is toogled on a line. This slot change the project settings. */
	void slotBookmarkToggled( int line, bool enabled );

protected:
	virtual void setModified( bool isModified );
	virtual bool eventFilter( QObject *obj, QEvent *event );
private slots:
	void comment();
	void uncomment();
private:
	QAction * m_commentAction, * m_uncommentAction;

	XinxCodeEdit * m_view;
	EndOfLineType m_eol;
	QDocumentCursor m_cursorStart, m_cursorEnd;
};

#endif // __FILEEDITOR_H__
