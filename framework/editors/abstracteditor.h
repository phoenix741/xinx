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
#ifndef EDITOR_H
#define EDITOR_H

// Xinx header
#include <core/lib-config.h>
#include <core/filewatcher.h>
#include <editors/bookmarkeditorinterface.h>
#include <project/xinxprojectproject.h>

// Qt header
#include <QFrame>
#include <QDataStream>
#include <QIcon>
#include <QPointer>

class QAction;
class QAbstractItemModel;
class IFileTypePlugin;

typedef QSharedPointer<IFileTypePlugin> IFileTypePluginPtr;

class LIBEXPORT AbstractEditor : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(QString title READ getTitle STORED false)
	Q_PROPERTY(bool isModified READ isModified WRITE setModified)
	Q_PROPERTY(QString filename READ lastFileName WRITE setWatcher)
public:
	enum SearchOption
	{
		ONLY_SELECTION = 0x01,
		BACKWARD = 0x02,
		WHOLE_WORDS = 0x04,
		REGULAR_EXPRESSION = 0x08,
		SEARCH_FROM_START = 0x10,
		MATCH_CASE = 0x20
	};
	Q_DECLARE_FLAGS(SearchOptions, SearchOption)

	explicit AbstractEditor(QWidget * parent = 0);
	virtual ~AbstractEditor();

	virtual QString getTitle() const;
	virtual QString getLongTitle() const;

	virtual bool canCopy() = 0;
	virtual bool canPaste() = 0;

	virtual bool canRedo() = 0;
	virtual bool canUndo() = 0;

	QIcon icon() const;

	virtual void loadFromDevice(QIODevice & d) = 0;
	virtual void saveToDevice(QIODevice & d) = 0;

	virtual void loadFromFile(const QString & fileName = QString());
	virtual void saveToFile(const QString & fileName = QString());

	virtual QAbstractItemModel * model() const = 0;
	virtual void updateModel() = 0;

	const QString & lastFileName() const;
	QString defaultFileName() const;

	virtual bool isModified();
	virtual bool hasNeverBeenModified();

	virtual void serialize(XinxSession::SessionEditor * data, bool content);
	virtual void deserialize(XinxSession::SessionEditor * data);
	static AbstractEditor * deserializeEditor(XinxSession::SessionEditor * data);

	virtual BookmarkEditorInterface * bookmarkInterface() = 0;

	virtual void setFileTypePluginInterface(IFileTypePluginPtr value);
	virtual IFileTypePluginPtr fileTypePluginInterface() const;

	virtual void setProject(XinxProject::ProjectPtr project);
	virtual XinxProject::ProjectPtr project() const;
public slots :
	virtual void initSearch(AbstractEditor::SearchOptions & options) = 0;
	virtual bool find(const QString & text, AbstractEditor::SearchOptions options) = 0;
	virtual void replace(const QString & from, const QString & to, AbstractEditor::SearchOptions options) = 0;

	virtual void undo() = 0;
	virtual void redo() = 0;
	virtual void cut() = 0;
	virtual void copy() = 0;
	virtual void paste() = 0;

signals:
	void modificationChanged(bool isModified);

	void undoAvailable(bool available);
	void redoAvailable(bool available);
	void copyAvailable(bool available);
	void pasteAvailable(bool available);

	void open(const QString & filename, int line);

	void contentChanged();
	void positionChanged(const QString & textPosition);
protected:
	AbstractEditor(const AbstractEditor & editor);

	virtual void initLayout();
	friend class EditorFactory;
protected slots:
	virtual void setModified(bool isModified);

private slots:
	void fileChanged();
private:
	void initObjects();
	void desactivateWatcher();
	void activateWatcher();
	void setWatcher(const QString & path);

	XinxProject::ProjectPtrWeak _project;
	IFileTypePluginPtr m_fileTypePlugin;
	bool m_isSaving, m_modified, m_neverModified;
	QPointer<FileWatcher> m_watcher;
	QString m_lastFileName;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractEditor::SearchOptions);

#endif
