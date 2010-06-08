/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#pragma once
#ifndef __FILEEDITOR_H__
#define __FILEEDITOR_H__

// Xinx headeur
#include <core/lib-config.h>
#include <editors/abstracteditor.h>
#include <plugins/plugininterfaces.h>
#include <contentview2/contentview2file.h>

// Qt header
#include <QTimer>
#include <QSqlDatabase>

// QCodeEdit header
#include <qdocumentcursor.h>

class XinxCodeEdit;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class QAction;
class FileWatcher;
class SyntaxHighlighter;
class BookmarkTextEditorInterface;
class QModelIndex;
class QBuffer;
namespace ContentView2
{
class CompletionModel;
class Node;
class Parser;
class TreeModel;
}


/*!
 * \class TextFileEditor
 * Class used to represent a file editor. A file editor have a XinxCodeEditor class, who show the line in the document,
 * folding, ....
 * The File Editor is a subclass of Editor and implements all its pure virtual method.
 *
 * The File Editor propose a method to read the FileName : getFileName(), and the default suffix (for save as) to use (getSuffix).
 * If a file is open, getTitle() return the file name otherwise it return noname.
 *
 * FileEditor has also two methode for load and save file.
 *
 * An \e TextFileEditor load and unload the content view model editor when
 * reading the device. The content model is based on a ContentViewParser and
 * some ContentViewNode.
 */
class LIBEXPORT TextFileEditor : public AbstractEditor
{
	Q_OBJECT
public:
	/*! Type of End Of Line that the editor can understand. */
	enum EndOfLineType
	{
		WindowsEndOfLine, //!< The end of line is terminated by \\r\\n
		UnixEndOfLine,    //!< The end of line is terminated by \\n
		MacEndOfLine      //!< The end of line is terminated by \\r\\n
	};
	/*!
	 * Construct a FileEditor with the help of a TextEditor and a parent.
	 * \param editor TextEditor to use to print file to screen (center widget)
	 * \param parent Parent of the editor.
	 */
	TextFileEditor(XinxCodeEdit * editor = 0, QWidget *parent = 0);
	/*! Destructor of the FileEditor.*/
	virtual ~TextFileEditor();

	/*!
	 * Return the text editor corresponding on the file editor. This editor can't be null.
	 * \return The text editor widget used by the file editor.
	 */
	virtual XinxCodeEdit * textEdit() const;
	virtual QString defaultFileName() const;

	virtual void loadFromFile(const QString & fileName = QString());
	virtual void saveToFile(const QString & fileName = QString());
	virtual void loadFromDevice(QIODevice & d);
	virtual void saveToDevice(QIODevice & d);

	/*!
	 * The default implementation return null
	 * \sa contentViewModel()
	 */
	virtual QAbstractItemModel * model()  const;
	/*!
	 * Return the model used in the edi	tor as model() but with the type ContentViewModel
	 * \sa model()
	 */
	ContentView2::TreeModel * contentViewModel() const;

	//! Set the codec to \e text
	void setCodecName(const QString & text);
	//! Get the codec name
	const QString & codecName() const;
	//! The codec used to read and write the file. By Default, the codec is defined in options.
	QTextCodec * codec() const;

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

	virtual void serialize(XinxProjectSessionEditor * data, bool content);
	virtual void deserialize(XinxProjectSessionEditor * data);

	virtual BookmarkEditorInterface * bookmarkInterface();
	virtual BookmarkTextEditorInterface * bookmarkTextInterface();
public slots :
	virtual void initSearch(SearchOptions & options);
	virtual bool find(const QString & text, SearchOptions options);
	virtual void replace(const QString & from, const QString & to, SearchOptions options);

	//! The default implementation do nothing
	virtual void updateModel();

	virtual void undo();
	virtual void redo();

	virtual void cut();
	virtual void copy();
	virtual void paste();

	/*! Method used to select all the text in the editor. The call is sent to the TextEditor. */
	void selectAll();

	/*! Auto indent all the document (named Pretty Print). */
	virtual bool autoIndent();
	/*! Call the completer of the text on the current position of the cursor, if possible. */
	virtual void complete();

	virtual void searchWord(const QString & word);
signals:
	/*!
	 * Signal emitted when a text is selected or deselected.
	 * \param yes The value is true if a part of the text is selected, else the value is false.
	 */
	void selectionAvailable(bool yes);
	/*!
	 * Signal emited when the position in the editor is changed. The \e index give
	 * the position of the cursor in the ContentViewModel.
	 */
	void positionInEditorChanged(const QModelIndex & index);

	/*!
	 * This signal is emited when the codec is modified.
	 */
	void codecChanged();

protected:
	virtual void detectEOL(QIODevice & d);
	virtual void detectCodec(QIODevice & d);
	virtual void setModified(bool isModified);
	virtual void contextMenuEvent(QContextMenuEvent * contextMenuEvent);

	virtual ContentView2::CompletionModel * createModel(QSqlDatabase db, QObject * parent = 0);

	virtual void initLayout();
	virtual void initCompleter();
	friend class EditorFactory;

	//! Return the current root node
	ContentView2::Node rootNode() const;
	//! Return the file container
	ContentView2::FileContainer fileContainer() const;
	//! Create a parser for the text editor
	virtual ContentView2::Parser * createParser();
protected slots:
	virtual void updateCodec();

private slots:
	void textChanged();
	void updateImports(const ContentView2::File & file);
	void errorChanged();
private:
	void initObjects();

	QTimer * m_keyTimer;
	QString m_codec;

	QBuffer * m_buffer;
	XinxCodeEdit * m_view;
	EndOfLineType m_eol;
	QDocumentCursor m_cursorStart, m_cursorEnd;

	BookmarkTextEditorInterface * m_bookmarkInterface;

	QString m_uuid;
	ContentView2::FileContainer m_file;
	ContentView2::CompletionModel * m_completionModel;
	ContentView2::TreeModel * m_model;
};

#endif // __FILEEDITOR_H__
