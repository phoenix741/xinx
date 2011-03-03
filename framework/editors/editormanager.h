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

/*!
 * \file editormanager.h
 * \brief This class represent the object that manage the list of editors.
 */

#ifndef _EDITORMANAGER_H_
#define _EDITORMANAGER_H_
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/xinxsingleton.h>
#include <project/xinxprojectproject.h>

// Qt header
#include <QObject>
#include <QString>
#include <QAction>


class IFileTypePlugin;
class AbstractEditor;
class PrivateEditorManager;
class PrivateTabWidget;

namespace XinxSession
{
class SessionEditor;
}

/*!
 * \class EditorManager
 * \brief This class manage the list of editor. It can be used to get the tab widget.
 */
class LIBEXPORT EditorManager : public XinxLibSingleton<EditorManager>
{
	Q_OBJECT
public:
	~EditorManager();

	int currentIndex() const;
	AbstractEditor * currentEditor() const;
	AbstractEditor * editor(int index) const;
	AbstractEditor * editor(const QString & filename) const;
	QList<AbstractEditor*> editors() const;
	int editorsCount() const;

	QWidget * editorsWidget() const;

	/* File Action */
	QAction * newAction() const;
	QAction * openAction() const;
	QAction * refreshAction() const;
	QAction * saveAction() const;
	QAction * saveAsAction() const;
	QAction * saveAllAction() const;
	QAction * printAction() const;
	QAction * closeAction() const;
	QAction * closeAllAction() const;
	QAction * copyFileNameToClipboardAction() const;
	QAction * copyPathToClipboardAction() const;

	QAction * nextEditorAction() const;
	QAction * previousEditorAction() const;
public slots:
	void setCurrentEditor(int index);
	void setCurrentEditor(AbstractEditor * editor);

	void changeToNextEditor();
	void changeToPreviousEditor();

	void openFile();
	void openFile(const QString & filename, IFileTypePlugin * interface = 0, XinxProject::ProjectPtr project = XinxProject::ProjectPtr());
	void openFile(const QString & filename, int line, IFileTypePlugin * interface = 0, XinxProject::ProjectPtr project = XinxProject::ProjectPtr());
	void saveFile(bool saveAs = false);
	void saveFile(int index, bool saveAs = false);
	void saveFile(AbstractEditor * editor, bool saveAs = false);
	void saveAsFile();
	void saveAsFile(int index);
	void saveAsFile(AbstractEditor * editor);
	void saveAllFile();
	void refreshFile(int index);
	void refreshFile(AbstractEditor * editor);
	void refreshFile();
	void printFile(int index);
	void printFile(AbstractEditor * editor);
	void printFile();
	void closeFile(int index, bool confirm = true);
	void closeFile(AbstractEditor * editor, bool confirm = true);
	void closeFile(bool confirm = true);
	bool closeAllFile(XinxProject::ProjectPtr project = XinxProject::ProjectPtr());

	void copyFileNameToClipboard();
	void copyPathToClipboard();

	void serializeEditors(bool content = false);
	void deserializeEditors(QList<XinxSession::SessionEditor*> session_editors);
signals:
	void fileOpened(const QString & filename);
	void fileSaved(const QString & filename);
	void filePrinted(const QString & filename);
	void fileClosing(const QString & filename);
	void fileClosed(const QString & filename);

	void codecChanged();
	void contentChanged();
	void currentChanged(int index);
	// FIXME: The editor position must be change
	void positionChanged(const QString & text);
	void aboutToClose();
private:
	EditorManager();

	PrivateEditorManager * d;
	friend class PrivateEditorManager;
	friend class PrivateTabWidget;
	friend class XinxLibSingleton<EditorManager>;
};

#endif // _EDITORMANAGER_H_

