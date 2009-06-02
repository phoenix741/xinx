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

#ifndef EDITOR_H
#define EDITOR_H
#pragma once

// Xinx header
#include "core/filewatcher.h"

// Qt header
#include <QFrame>
#include <QDataStream>
#include <QIcon>
#include <QPointer>

class QAction;
class QLabel;
class XinxProjectSessionEditor;
class BorderLayout;
class QAbstractItemModel;
class QSplitter;

/*!
 * Base class for construct editor for XINX. This base class contains minimum method
 * necessary to construct an editor.
 * An editor has a name, a title, and method related with clipboard. The editor has
 * a model for show content of it.
 * An editor can be serialized and deserialized in a XML Document.
 *
 * This class is also an editor that can read and write from and to a file. This class
 * open the file (with the correct option) and call loadFromDevice and saveTofDevice.
 */
class AbstractEditor : public QFrame {
	Q_OBJECT
	Q_PROPERTY( QString title READ getTitle STORED false )
	Q_PROPERTY( bool isModified READ isModified WRITE setModified )
	Q_PROPERTY( QString filename READ lastFileName WRITE setWatcher )
public:

	/*! Options used for search text in the editor. */
	enum SearchOption {
		ONLY_SELECTION = 0x01,      //!< Search only on selected element (text, widget, ...)
		BACKWARD = 0x02,            //!< Reverse search
		WHOLE_WORDS = 0x04,         //!< Search a word and not a piece of word
		REGULAR_EXPRESSION = 0x08,  //!< The search string is a regular expression
		SEARCH_FROM_START = 0x10,   //!< Search from the start of the document (backward and selection must be ignored)
		MATCH_CASE = 0x20           //!< The search is case sensitive
	};
	Q_DECLARE_FLAGS( SearchOptions, SearchOption )

	/*!
	 * Create an editor and define default style for the Frame.
	 * \param parent Parent and containers of the editor.
	 */
	AbstractEditor( QWidget * parent = 0 );

	/*! Destroy the created frame editor. */
	virtual ~AbstractEditor();

	/*!
	 * Get the title of the editor. Return the title of the FileEditor. The title is \e noname if \e getFileName() is Empty
	 * else return the File name (without the path).
	 * \return The title to use with Tab name
	 * \sa getFileName(), hasName()

	 */
	virtual QString getTitle() const;

	/*!
	 * Get the long title of the editor. It can be use on the TabWidget to inform user in a tool type. If editor has no
	 * name, this can be equals to "noname".
	 * \return The title of frame.
	 * \sa hasName()
	 */
	virtual QString getLongTitle() const;

	/*!
	 * Check if the editor has the capacity to copy or cut data to the clipboead.
	 * A signal is emmited when this value change.
	 * \return true if the editor can copy or cut data to clipboard else return false.
	 * \sa copy(), cut(), copyAvailable()
	 */
	virtual bool canCopy() = 0;
	/*!
	 * Return the action used to cut a selected text in the editor. This action call the \e cut().
	 * \return Return the action to cut.
	 * \sa cut(), copyAvailable(), canCopy()
	 */
	QAction * cutAction();
	/*!
	 * Return the action used to copy a selected text in the editor. This action call the \e copy().
	 * \return Return the action to copy.
	 * \sa copy(), copyAvailable(), canCopy()
	 */
	QAction * copyAction();

	/*!
	 * Check if the editor can accept paste data from clipboard.
	 * A signal is emmited when this value change.
	 * \return true if the editor can past data from clipboard.
	 * \sa paste(), pasteAvailable()
	 */
	virtual bool canPaste() = 0;

	/*!
	 * Return the action used to past a text in the editor. This action call the \e paste().
	 * \return Return the action to paste.
	 * \sa copy(), pasteAvailable(), canPaste()
	 */
	QAction * pasteAction();

	/*!
	 * Check if the editor can undo last command.
	 * A signal is emmited when this value change.
	 * \return true if the editor can undo last change, else return false.
	 * \sa undo(), undoAvailable()
	 */
	virtual bool canUndo() = 0;
	/*!
	 * Return the action used to undo operations in the editor. This action call the \e undo().
	 * \return Return the action to undo.
	 * \sa undo(), undoAvailable()
	 */
	QAction * undoAction();

	/*!
	 * Return the action used to redo operations in the editor. This action call the \e redo().
	 * \return Return the action to redo.
	 * \sa redo(), redoAvailable()
	 */
	QAction * redoAction();

	/*!
	 * Check if the editor can redo change made by undo.
	 * A signal is emmited when this value change.
	 * \return true if the editor can redo last change, else return false.
	 * \sa redo(), redoAvailable()
	 */
	virtual bool canRedo() = 0;

	/*!
	 * Return an icon that represent the editor.
	 * \return an icon for the editor.
	 */
	virtual QIcon icon() const;

	/*!
	 * Load the editor from a device (from a file, the memory or any medium).
	 * \param d The device where XINX must load.
	 */
	virtual void loadFromDevice( QIODevice & d ) = 0;
	/*!
	 * Save the editor in a device(to a file, in the memory, or any medium).
	 * \param d The device where XINX must write.
	 */
	virtual void saveToDevice( QIODevice & d ) = 0;

	/*! Open and load from the file \e fileName */
	virtual void loadFromFile( const QString & fileName = QString() );
	/*! Open and save to file \e fileName */
	virtual void saveToFile( const QString & fileName = QString() );

	/*! Return the model that represent the content of the editor. */
	virtual QAbstractItemModel * model() const = 0;
	/*! Update the content of the model from the content of the editor */
	virtual void updateModel() = 0;

	/*! Return the last name used with \e loadFromFile() or \e saveToFile() */
	const QString & lastFileName() const;
	/*! Return the name used if no name is defined (ie. noname.txt) */
	virtual QString defaultFileName() const = 0;

	/*! Return true if the editor is modified by the user, else return false */
	virtual bool isModified();

	/*!
	 * Serialize the editor and return the value in a byte array. The serialization save internal data of
	 * the editor (modified, content, position of cursor, ...).
	 * \param data where datas must be stored.
	 * \param content If true, the editor save the modifed content, else the editor must save only
	 * the state.
	 * \sa deserialze(), deserialzeEditor()
	 */
	virtual void serialize( XinxProjectSessionEditor * data, bool content );

	/*!
	 * Restore the editor with the content of the XML document. The deserialization restore the
	 * maximum of information of the document.
	 * \param data from what the data must be read
	 * \sa serialize(), deserializeEditor()
	 */
	virtual void deserialize( XinxProjectSessionEditor * data );
	/*!
	 * Create the right editor and deserualize it.
	 * \param data from what the data must be read
	 * \return An editor
	 * \sa serialize(), deserialize()
	 */
	static AbstractEditor * deserializeEditor( XinxProjectSessionEditor * data );

	/*! Return a string reprensents the content of the \em i -ième bookmark. */
	virtual QString bookmarkAt( int i ) = 0;
	/*! Return the number of bookmark. */
	virtual int bookmarkCount() = 0;
public slots :
	/*!
	 * Call-it before the search/replace begins to define the option to use
	 * when search and initialize some variables.
	 * @param options Options used to search text.
	 */
	virtual void initSearch( SearchOptions & options ) = 0;

	/*!
	 * Search the \e text in the document, and select it.
	 * @param text The text to search in the document
	 * @param options User options used to find the text
	 */
	virtual bool find( const QString & text, SearchOptions options ) = 0;

	/*!
	 * Replace the current selection by the user text.
	 * @param from The text to replace (as asked by user so be aware of regexp)
	 * @param to The text the user want to put. (if regexp \\1, \\2 is catched text)
	 * @param options User options used to replace the text
	 */
	virtual void replace( const QString & from, const QString & to, SearchOptions options ) = 0;

	/*!
	 * Call undo operation on the editor, if available. This operation rollback the last modification
	 * made on the editor.
	 * \sa canUndo(), undoAvailable()
	 */
	virtual void undo() = 0;
	/*!
	 * Call redo operation on the editor, if available. This operation rollback the last undo command
	 * and redo the last operaton
	 * \sa canRedo(), redoAvailable()
	 */
	virtual void redo() = 0;

	/*!
	 * Cut selected data on the editor to the clipboard.
	 * \sa canCopy(), copy(), copyAvailable()
	 */
	virtual void cut() = 0;
	/*!
	 * Copy selected data on the editor to the clipboard.
	 * \sa canCopy(), cut(), copyAvailable()
	 */
	virtual void copy() = 0;
	/*!
	 * Paste data from the clipboard on the editor.
	 * \sa canPaste(), pasteAvailable()
	 */
	virtual void paste() = 0;

	/*!
	 * Toogled a bookmark on the selected element.
	 */
	virtual void toogledBookmark() = 0;
	/*!
	 * Go to the first Bookmark of the editor, if the editor have one bookmark.
	 */
	virtual void firstBookmark();
	/*!
	 * Go to the previous bookmark or return false if there is no more bookmark.
	 */
	virtual bool previousBookmark() = 0;
	/*!
	 * Go to the next bookmark or return false if ther is no more bookmark.
	 */
	virtual bool nextBookmark() = 0;
	/*!
	 * Go to the last bookmark of the editor, if possible.
	 */
	virtual void lastBookmark();
	/*!
	 * Go to the \em i Bookmark of the editor.
	 */
	virtual void gotoBookmarkAt( int i ) = 0;
	/*!
	 * Delete all existing bookmark
	 */
	virtual void clearAllBookmark() = 0;
signals:
	//! Signal emited when the content changed to update view synchronised on this container.
	void modificationChanged( bool isModified );

	/*!
	 * Signal emitted when the undo state change.
	 * \param available  true if undo is Available, else false
	 * \sa canUndo(), undoAvailable()
	 */
	void undoAvailable( bool available );
	/*!
	 * Signal emitted when the redo state change.
	 * \param available  true if redo available, else false.
	 * \sa canRedo(), redoAvailable()
	 */
	void redoAvailable( bool available );
	/*!
	 * Signal emitted when the cut/copy state change.
	 * \param available true if copy or cut is available (e.g. a text is selected), else false
	 * \sa copy(), cut(), canCopy()
	 */
	void copyAvailable( bool available );
	/*!
	 * Signal emitted when the paste state change.
	 * \param available  true if paste is available, else false.
	 * \sa canPaste(), paste()
	 */
	void pasteAvailable( bool available );

	/*!
	 * Signal emited when the list of bookmark is updated. When the signal is emited
	 * a range is gived to tell which row is modified.
	 * \param minValue The first bound
	 * \param maxValue The second bound
	 */
	void bookmarkModified( int minValue, int maxValue );

	/*!
	 * Signal emited when a request to open a file is made.
	 * \param filename file name of the file to open
	 * \param line line of the file to open
	 */
	void open( const QString & filename, int line );

	/*!
	 * Signal emited when the content of the editor change.
	 */
	void contentChanged();
protected:
	/*! Constructor used to copy the editor content. This constructor must exist for serialization works. */
	AbstractEditor( const AbstractEditor & editor );

	/*!
	 * The layout used for the editor. This layout is decomposed with a center widget and
	 * four border.
	 *
	 *    +---------------------------+
	 *    |            TOP            |
	 *    +-------+------------+------+
	 *    |       |            |      |
	 *    |       |            |      |
	 *    | RIGHT |   CENTER   | LEFT |
	 *    |       |            |      |
	 *    |       |            |      |
	 *    +-------+------------+------+
	 *    |           BOTTOM          |
	 *    +---------------------------+
	 */
	BorderLayout * borderLayout();

	/*!
	 * Used for the center widget to have a horizontal splitter
	 */
	QSplitter * splitter() const;
protected slots:
	/*!
	 * Set the modified attribute in local.
	 * \param isModified The new value for the modified attribute.
	 */
	virtual void setModified( bool isModified );

	/*!
	 * Set a message and show it on the screen in a no blocking popup.
	 * \param message When the message is set, the message is show on the screen. If the message is blank, the popup is hidden.
	 */
	void setMessage( QString message );
private slots:
	void fileChanged();
private:
	void init();
	void desactivateWatcher();
	void activateWatcher();
	void setWatcher( const QString & path );

	bool m_isSaving, m_modified;
	QPointer<FileWatcher> m_watcher;
	QString m_lastFileName;
	QAction * m_undoAction, * m_redoAction, * m_cutAction, * m_copyAction, * m_pasteAction;
	QWidget * m_messageWidget;
	QLabel * m_messageLabel;
	QSplitter * m_splitter;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( AbstractEditor::SearchOptions );

#endif
