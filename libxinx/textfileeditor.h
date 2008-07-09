/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

// Xinx headeur
#include "abstractfileeditor.h"
#include "filecontentitemmodel.h"
#include "plugininterfaces.h"

// Qt header
#include <QTimer>
#include <QTextEdit>

class NumberBar;
class TextEditor;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class QAction;
class FileWatcher;
class SyntaxHighlighter;


/*!
 * Class used to represent a file editor. A file editor have a TextEditor class (subclassing of QTextEdit) and a NumberBar,
 * who show the line in the document.
 * The File Editor is a subclass of Editor and implements all its pure virtual method.
 *
 * The File Editor propose a method to read the FileName : getFileName(), and the default suffix (for save as) to use (getSuffix).
 * If a file is open, getTitle() return the file name otherwise it return noname.
 *
 * FileEditor has also two methode for load and save file.
 */
class TextFileEditor : public AbstractFileEditor {
	Q_OBJECT
public:
	enum EndOfLineType { WindowsEndOfLine, UnixEndOfLine, MacEndOfLine };
	/*!
	 * Construct a FileEditor with the help of a TextEditor and a parent.
	 * \param textEditor TextEditor to use to print file to screen
	 * \param parent Parent of the editor.
	 */
	TextFileEditor( TextEditor * editor = 0, QWidget *parent = 0 );
	/*!
	 * Destructor of the FileEditor.
	 */
	virtual ~TextFileEditor();

	/*!
	 * Return the title of the FileEditor. The title is \e noname if \e getFileName() is Empty
	 * else return the File name (without the path).
	 * \return The title to use with Tab name
	 * \sa getFileName(), hasName()
	 */
	virtual QString getTitle() const;

 	/*!
 	 * Return the text editor corresponding on the file editor. This editor can't be null.
 	 * \return The text editor widget used by the file editor.
 	 */
	virtual TextEditor * textEdit() const;
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
	virtual QList<int> & bookmarks() const;

	virtual void loadFromDevice( QIODevice & d );
	virtual void saveToDevice( QIODevice & d );

	//! The default implementation return null.
	virtual QAbstractItemModel * model()  const;

	virtual QTextCodec * codec() const;
	virtual EndOfLineType eol() const;

	virtual bool canCopy();
	virtual bool canPaste();
	virtual bool canUndo();
	virtual bool canRedo();

	virtual void serialize( XSLProjectSessionEditor * data, bool content );
	virtual void deserialize( XSLProjectSessionEditor * data );

	void setBookmark( int line, bool enabled );
	virtual void toogledBookmark();
	virtual void gotoBookmarkAt( int i );
	virtual QString bookmarkAt( int i );
	virtual int bookmarkCount();
	virtual bool previousBookmark();
	virtual bool nextBookmark();
	virtual void clearAllBookmark();
public slots :
	//! The default implementation do nothing
	virtual void updateModel();

	virtual void undo();
	virtual void redo();

	virtual void cut();
	virtual void copy();
	virtual void paste();

	/*!
	 * Method used to select all the text in the editor. The call is sent to the TextEditor.
	 */
	virtual void selectAll();
	/*!
	 * Comment or Uncomment the selected text depending on the parrameter.
	 * If a part of a text is already (un)commented, the balise is moved to comment all the text.
	 * Warning: If you comment code with comment, the comment can be merged with code.
	 * \param uncomment If false (by default) the text is commented, else the text is uncommented
	 */
	virtual void commentSelectedText( bool uncomment = false );
	/*!
	 * Indent or unindent the selected text depending on the parameter.
	 * \param unindent If false (by default) the text is indented. (the character \\t is added), else the text is unindented.
	 */
	virtual void indent( bool unindent = false );
	/*!
	 * Auto indent all the document (named Pretty Print).
	 */
	virtual bool autoIndent();
	/*!
	 * Call the completer of the text on the current position of the cursor, if possible.
	 */
	virtual void complete();

	virtual void searchWord( const QString & word );
signals:
	/*!
	 * Signal emitted when a text is selected or deselected.
	 * \param yes The value is true if a part of the text is selected, else the value is false.
	 */
	void selectionAvailable ( bool yes );

protected slots:
	/*!
	 * Slot called when the bookmark is toogled on a line. This slot change the project settings.
	 */
	void slotBookmarkToggled( int line, bool enabled );

protected:
	virtual void setModified( bool isModified );
	virtual bool eventFilter( QObject *obj, QEvent *event );
	NumberBar * numbersBar() const;

private slots:
	void comment();
	void uncomment();
private:
	QAction * m_commentAction, * m_uncommentAction;

	NumberBar * m_numbers;
	TextEditor * m_view;
	EndOfLineType m_eol;
};

#endif // __FILEEDITOR_H__
