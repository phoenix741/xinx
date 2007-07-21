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

#ifndef __FILEEDITOR_H__
#define __FILEEDITOR_H__

#include "editor.h"

class NumberBar;
class TextEditor;
class QTextEdit;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;

class PrivateFileEditor;

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
class FileEditor : public Editor {
	Q_OBJECT
public:
	/*!
	 * Construct a FileEditor with the help of a TextEditor and a parent.
	 * \param textEditor TextEditor to use to print file to screen
	 * \param parent Parent of the editor.
	 */
	FileEditor( TextEditor * textEditor, QWidget *parent = 0 );
	/*!
	 * Destructor of the FileEditor.
	 */
	virtual ~FileEditor();

	/*!
	 * Get the file name of the editor, if a file is loaded.
	 * \return File name loaded in the editor.
	 * \sa hasName(), getTitle()
	 */
	const QString & getFileName() const;
	/*!
	 * Return the default suffix to use with the editor. (e.g. : txt, xsl, ...)
	 * \return The default suffix to use
	 */
	virtual QString getSuffix() const;
	/*! 
	 * Return the title of the FileEditor. The title is \e noname if \e getFileName() is Empty
	 * else return the File name (without the path).
	 * \return The title to use with Tab name
	 * \sa getFileName(), hasName()
	 */
	virtual QString getTitle() const;
	virtual bool hasName() const;
	
	/*!
	 * Load the file with the name \e fileName in the editor.
	 * If the file can't be read a dialog is show and nothing is do.
	 * \param fileName Name of the file to load in the editor. If empty, reload the current file.
	 * \sa saveFile()
	 */
	virtual void loadFile( const QString &fileName = "" );
	/*!
	 * Save the file with the name \e fileName in the editor.
	 * If the name of the file is empty, the application save the file in the current file.
	 * \param fileName Name of the file where the content must be save.
	 * \return If the file can't be save, the function return false.
	 * \sa loadFile()
	 */
	virtual bool saveFile( const QString &fileName = "" );
 
 	/*!
 	 * Return the text editor corresponding on the file editor. This editor can't be null.
 	 * \return The text editor widget used by the file editor.
 	 */
	TextEditor * textEdit() const;
 
	virtual bool canCopy();
	virtual bool canPaste();
	virtual bool canUndo();
	virtual bool canRedo();
	virtual bool isModified();

	virtual QAbstractItemModel * model();
	virtual void updateModel();

	virtual void serializeEditor( QDomElement & element, bool content );
	virtual void deserializeEditor( const QDomElement & element );
public Q_SLOTS : 
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
	 * Duplicate the current line in the editor.
	 * \todo Duplicate more than one line. (duplicate the selected text).
	 */
	virtual void duplicateCurrentLine();
	/*!
	 * Move the current line up.
	 * Swap the current line with the line above.
	 * \sa moveLineDown()
	 */
	virtual void moveLineUp();
	/*!
	 * Move the current line down.
	 * Swap the current line with the line after.
	 * \sa moveLineUp()
	 */
	virtual void moveLineDown();
	/*!
	 * Replace the selected text by upper case character or lower case character depending on 
	 * the parameter.
	 * \param upper If true (by default), the selected text is replaced by upper characters, else it's replaced by lower case charcters.
	 */
	virtual void uploSelectedText( bool upper = true );
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
	 * Call the completer of the text on the current position of the cursor, if possible.
	 */
	virtual void complete();

	virtual void setModified( bool modified );
Q_SIGNALS:
	void mouseHover( const QString &word );
	void mouseHover( const QPoint &pos, const QString &word );
	
	/*!
	 * Signal emitted when a text is selected or deselected.
	 * \param yes The value is true if a part of the text is selected, else the value is false.
	 */
	void selectionAvailable ( bool yes );

protected slots:
	/*!
	 * Set a message and show it on the screen in a no blocking popup. 
	 * \param message When the message is set, the message is show on the screen. If the message is blank, the popup is hidden.
	 */
	void setMessage( QString message );

protected:
	virtual bool eventFilter( QObject *obj, QEvent *event );

	/*!
	 * Desactivate the watcher of the file. If the file is modified in an external editor, a popup is show
	 * asking if the user want to reload the file.
	 * \sa activateWatcher()
	 */
	void desactivateWatcher();
	/*!
	 * Activate the watcher of the file. If the file is modified in an external editor, a popup is show
	 * asking if the user want to reload the file.
	 * \sa activateWatcher()
	 */
	void activateWatcher();

	/*!
	 * Set the file name in the editor (in case of load or save). If the file name is modified and 
	 * the new name is changed from standard file, to specifique file, in project mode, a copy of a 
	 * standard file is made in the project directory.
	 * \param name Name of the new file.
	 */
	void setFileName( const QString & name );
	/*!
	 * Create a backup of the file with the name filename. (Made a copy of the file with an extention
	 * \e .bak
	 * \param filename Name of the file to copy
	 */
	static void createBackup( const QString & filename );
  
	QString m_fileName;

	QVBoxLayout * m_vbox;
	QHBoxLayout * m_hbox;
	NumberBar * m_numbers;
	TextEditor * m_view;
	QLabel * m_messageErreur;
	QWidget * m_messageBox;
private:
	PrivateFileEditor * d;
	friend class PrivateFileEditor;
};


#endif // __FILEEDITOR_H__
