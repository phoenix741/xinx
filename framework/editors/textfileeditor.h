/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#ifndef __FILEEDITOR_H__
#define __FILEEDITOR_H__

// Xinx headeur
#include <core/lib-config.h>
#include <editors/abstracteditor.h>
#include <plugins/interfaces/files.h>
#include <contentview3/file.h>
#include <codecompletion/context.h>

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
class BookmarkTextEditorInterface;
class QModelIndex;
class QBuffer;

namespace ContentView3
{
class Node;
class Parser;
class TreeModel;
}

namespace CodeCompletion
{
	class Completer;
	class Model;
}

class LIBEXPORT TextFileEditor : public AbstractEditor
{
	Q_OBJECT
public:
	enum EndOfLineType
	{
		WindowsEndOfLine,
		UnixEndOfLine,
		MacEndOfLine
	};

	explicit TextFileEditor(XinxCodeEdit * editor = 0, QWidget * parent = 0);
	virtual ~TextFileEditor();

	virtual XinxCodeEdit * textEdit() const;

	virtual void loadFromFile(const QString & fileName = QString());
	virtual void saveToFile(const QString & fileName = QString());
	virtual void loadFromDevice(QIODevice & d);
	virtual void saveToDevice(QIODevice & d);

	CodeCompletion::Context context();
	virtual QAbstractItemModel * model()  const;
	ContentView3::TreeModel * contentViewModel() const;

	void setCodecName(const QString & text);
	const QString & codecName() const;
	QTextCodec * codec() const;

	virtual EndOfLineType eol() const;

	virtual bool canCopy();
	virtual bool canPaste();
	virtual bool canUndo();
	virtual bool canRedo();

	virtual void serialize(XinxSession::SessionEditor * data, bool content);
	virtual void deserialize(XinxSession::SessionEditor * data);

	virtual BookmarkEditorInterface * bookmarkInterface();
	virtual BookmarkTextEditorInterface * bookmarkTextInterface();

	virtual void setProject(XinxProject::ProjectPtr project);
public slots :
	virtual void initSearch(SearchOptions & options);
	virtual bool find(const QString & text, SearchOptions options);
	virtual void replace(const QString & from, const QString & to, SearchOptions options);

	virtual void updateModel();

	virtual void undo();
	virtual void redo();

	virtual void cut();
	virtual void copy();
	virtual void paste();

	void selectAll();

	virtual bool autoIndent();
	virtual void complete();

	virtual void searchWord(const QString & word);

	virtual bool isSnipetMustBeShow() const;
signals:
	void selectionAvailable(bool yes);
	void codecChanged();

protected:
	ContentView3::NodePtr localNodeOfWord(const ContentView3::NodePtr & node, const QString & word);
	ContentView3::NodePtr globalNodeOfWord(const QString & word);

	virtual void detectEOL(QIODevice & d);
	virtual void detectCodec(QIODevice & d);
	virtual void setModified(bool isModified);
	virtual void contextMenuEvent(QContextMenuEvent * contextMenuEvent);

	virtual ContentView3::Parser * createParser();

	void setFile(ContentView3::FilePtr file);
	ContentView3::FilePtr file() const;

	virtual void initLayout();
	friend class EditorFactory;
protected slots:
	virtual void updateCodec();
	virtual void fileParsed();
	virtual void cursorPositionChanged();
private slots:
	void updateContext();
	void textChanged();
	void errorChanged();
private:
	void initObjects();

	QTimer * _modification_timer, * _move_timer;
	QString m_codec;

	XinxCodeEdit * m_view;
	EndOfLineType m_eol;
	QDocumentCursor m_cursorStart, m_cursorEnd;

	BookmarkTextEditorInterface * m_bookmarkInterface;

	CodeCompletion::Context _context;
	ContentView3::FilePtr _file;
	ContentView3::TreeModel * _model;
	CodeCompletion::Model * _completion_model;
	CodeCompletion::Completer * _completer;
};

#endif // __FILEEDITOR_H__
