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

#ifndef EDITOR_H
#define EDITOR_H

// Qt header
#include <QFrame>
#include <QDomElement>
#include <QDataStream>
#include <QIcon>

class QAbstractItemModel;
class QAction;
class PrivateEditor;

/*!
 * Base class for construct editor for XINX. This base class contains minimum method
 * necessary to construct an editor.
 * An editor has a name, a title, and method related with clipboard. The editor has 
 * a model for show content of it.
 * An editor can be serialized and deserialized in a XML Document.
 */
class Editor : public QFrame {
	Q_OBJECT
public:
	/*!
	 * Create an editor and define default style for the Frame.
	 * \param parent Parent and containers of the editor.
	 */
	Editor( QWidget * parent = 0 );
	/*!
	 * The copy constructor used in case of serialization and deserialization.
	 * \param the editor to copy.
	 */
	Editor( const Editor & editor );
	/*!
	 * Destroy the created frame editor.
	 */
	virtual ~Editor();

	/*!
	 * Get the title of the editor. It can be use on the TabWidget to inform user. If editor has no 
	 * name, this can be equals to "noname". 
	 * \return The title of frame.
	 * \sa hasName()
	 */
	virtual QString getTitle() const = 0;
	/*!
	 * Check if the frame has a name. It's use to determine, if editor must use Save or Save As
	 * on editor of type FileEditor.
	 * \return true if the editor has a name else return false.
	 * \sa getTitle()
	 */
	virtual bool hasName() const = 0;
	
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
	 * Check if the document is modified. Read the state of the editor. Application propose
	 * to save on quit only if the editor need to be save (so the editor is modified).
	 * A signal is emitted if this state change.
	 * \return true if the editor is need to be saved, else return false.
	 * \sa setModified(), modificationChanged()
	 */
	virtual bool isModified() = 0;
	
	/*!
	 * Model that represents the content of the editor. This content is show in a QTreeView
	 * in XINX application.
	 * If model neet to be deleted or recreated, signals can be emitted.
	 * \return The model who contains the content of editor.
	 * \sa deleteModel(), createModel(), updateModel()
	 */
	virtual QAbstractItemModel * model() = 0;
	
	/*!
	 * Return an icon that represent the editor.
	 * \return an icon for the editor.
	 */
	virtual QIcon icon();
	
	/*!
	 * Get a serialized data of type QVariant. Do no process the type.
	 * Store in \e type the type of the data, and in \e data the content, of the 
	 * data. If an old version can't read datas, ingored.
	 */
	virtual void getSerializedData( QDataStream & stream, int & type, QVariant & data );
	
	/*!
	 * Serialize data \e data with a type \e type.
	 */  
	virtual void setSerializedData( QDataStream & stream, int type, QVariant data );
	
	/*!
	 * Serialize the editor and return the value in a byte array. The serialization save internal data of
	 * the editor (modified, content, position of cursor, ...).
	 * \param stream where datas must be stored.
	 * \param content If true, the editor save the modifed content, else the editor must save only
	 * the state.
	 * \sa deserialze(), deserialzeEditor()
	 */
	virtual void serialize( QDataStream & stream, bool content );
	
	/*!
	 * Restore the editor with the content of the XML document. The deserialization restore the
	 * maximum of information of the document.
	 * \param stream from what the data must be read
	 * \sa serialize(), deserializeEditor()
	 */
	virtual void deserialize( QDataStream & stream );
	/*!
	 * Create the right editor and deserualize it.
	 * \param stream from what the data must be read
	 * \return An editor
	 * \sa serialize(), deserialize()
	 */
	static Editor * deserializeEditor( QDataStream & stream );	
public slots : 
	/*!
	 * Update the model.
	 * \sa deleteModel(), createModel(), model()
	 */
	virtual void updateModel() = 0;
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
	 * Change the information about the state of the editor (modified/unmodified)
	 * \param modified true to change the state to modified, else the value must be false
	 * \sa isModified(), modificationChanged()
	 */
	virtual void setModified( bool modified ) = 0;
	
	/*!
	 * Go to the first Bookmark of the editor, if the editor have one bookmark. 
	 */
	virtual void firstBookmark();
	/*!
	 * Go to the previous bookmark or return false if there is no more bookmark.
	 */
	virtual bool previousBookmark();
	/*!
	 * Go to the next bookmark or return false if ther is no more bookmark.
	 */
	virtual bool nextBookmark();
	/*!
	 * Go to the last bookmark of the editor, if possible.
	 */
	virtual void lastBookmark();
	/*!
	 * Go to the \em i Bookmark of the editor.
	 */
	virtual void gotoBookmarkAt( int i );
	/*!
	 * Return a string reprensents the content of the \em i bookmark.
	 */
	virtual QString bookmarkAt( int i ) = 0;
	/*!
	 * Return the number of bookmark.
	 */
	virtual int bookmarkCount();
signals:
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
	 * Signal emitted when the document modified state changed (docment is modified, or saved).
	 * \param changed true if the document is modified, or false, if the document is intact.
	 * \sa setModified(), isModified()
	 */
	void modificationChanged( bool changed );

	/*!
	 * Signal emited when the model is changed. The model is NULL when he is deleted.
	 * \param model The model that changed
	 * \sa model()
	 */
	void modelUpdated( QAbstractItemModel * model );
private:
	PrivateEditor * d;
	friend class PrivateEditor;
};

#endif
