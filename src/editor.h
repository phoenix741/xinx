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
//
#include <QFrame>
#include <QDomElement>
//

class QAbstractItemModel;

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
	 */
	Editor( QWidget * parent = 0 );
	/*!
	 * Destroy the created frame editor.
	 */
	virtual ~Editor();

	/*!
	 * Get the title to show in the Tab
	 * \return The title of frame
	 */
	virtual QString getTitle() const = 0;
	/*!
	 * Check if the frame has a name to know if we must use Save or Save As ...
	 * \return true if the editor has a name else return false.
	 */
	virtual bool hasName() const = 0;
	
	/*!
	 * Check if the editor has the capacity to copy.
	 */
	virtual bool canCopy() = 0;
	/*!
	 * Check if the editor can accept paste
	 */
	virtual bool canPaste() = 0;
	/*!
	 * Check if the editor can undo last command.
	 */
	virtual bool canUndo() = 0;
	/*! 
	 * Check if the editor can redo change made by undo.
	 */
	virtual bool canRedo() = 0;
	/*!
	 * Check if the document is modified 
	 */
	virtual bool isModified() = 0;
	
	/*!
	 * Model that represents the content of the editor. This content is show in a QTreeView
	 * in XINX application.
	 */
	virtual QAbstractItemModel * model() = 0;
	
	/*!
	 * Serialize the editor to be backup to a XML file. The serialization backup the status of
	 * the editor (modified, content, position of cursor).
	 */
	virtual void serializeEditor( QDomElement & element, bool content ) = 0;
	/*!
	 * Restore the editor with the content of the XML document. The deserialization restore the
	 * maximum of information of the document.
	 */
	virtual void deserializeEditor( const QDomElement & element ) = 0;
	
public slots : 
	/*!
	 * Call undo on the editor.
	 */
	virtual void undo() = 0;
	/*!
	 * Call redo on the editor.
	 */
	virtual void redo() = 0;

	/*! 
	 * Call cut on the editor 
	 */
	virtual void cut() = 0;
	/*!
	 * Call copy on the editor.
	 */
	virtual void copy() = 0;
	/*!
	 * Call paste on the editor.
	 */
	virtual void paste() = 0;
	
	/*!
	 * Change the information about the state of the editor (modified/unmodified)
	 */
	virtual void setModified( bool ) = 0;
	
signals:
	/*!
	 * Signal emitted when the undo state change
	 */
	void undoAvailable( bool available );
	/*!
	 * Signal emitted when the redo state change
	 */
	void redoAvailable( bool available );
	/*!
	 * Signal emitted when the cut/copy state change
	 */
	void copyAvailable( bool available );
	/*!
	 * Signal emitted when the paste state change
	 */
	void pasteAvailable( bool available );
	
	/*!
	 * Signal emitted when the document is modified, or saved
	 */
	void modificationChanged( bool changed );

	/*!
	 * Signal emited when the model is deleted and becomes NULL
	 */
	void deleteModel();
	/*!
	 * Signal emitted when the model is created and can be used.
	 */
	void createModel();
};
#endif
