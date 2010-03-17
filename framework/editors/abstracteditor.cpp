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

// Xinx header
#include "editors/abstracteditor.h"
#include "project/xinxproject.h"
#include "borderlayout.h"
#include "core/xinxconfig.h"
#include "plugins/xinxpluginsloader.h"

// Qt header
#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include <QFile>
#include <QApplication>
#include <QMessageBox>
#include <QSplitter>

/* AbstractEditor */

AbstractEditor::AbstractEditor(QWidget * parent)  : QFrame(parent), m_isSaving(false), m_modified(false)
{
	initObjects();
}

AbstractEditor::AbstractEditor(const AbstractEditor & editor) : QFrame(qobject_cast<QWidget*>(editor.parent())), m_isSaving(false), m_modified(false)
{
	initObjects();
}

void AbstractEditor::initObjects()
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setLineWidth(2);

	m_undoAction = new QAction(QIcon(":/images/undo.png"), AbstractEditor::tr("&Undo"), this);
	m_undoAction->setEnabled(false);
	QObject::connect(m_undoAction, SIGNAL(triggered()), this, SLOT(undo()));
	QObject::connect(this, SIGNAL(undoAvailable(bool)), m_undoAction, SLOT(setEnabled(bool)));
	m_redoAction = new QAction(QIcon(":/images/redo.png"), AbstractEditor::tr("&Redo"), this);
	m_redoAction->setEnabled(false);
	QObject::connect(m_redoAction, SIGNAL(triggered()), this, SLOT(redo()));
	QObject::connect(this, SIGNAL(redoAvailable(bool)), m_redoAction, SLOT(setEnabled(bool)));

	m_cutAction = new QAction(QIcon(":/images/editcut.png"), AbstractEditor::tr("&Cut"), this);
	m_cutAction->setEnabled(false);
	QObject::connect(m_cutAction, SIGNAL(triggered()), this, SLOT(cut()));
	QObject::connect(this, SIGNAL(copyAvailable(bool)), m_cutAction, SLOT(setEnabled(bool)));
	m_copyAction = new QAction(QIcon(":/images/editcopy.png"), AbstractEditor::tr("C&opy"), this);
	m_copyAction->setEnabled(false);
	QObject::connect(m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
	QObject::connect(this, SIGNAL(copyAvailable(bool)), m_copyAction, SLOT(setEnabled(bool)));
	m_pasteAction = new QAction(QIcon(":/images/editpaste.png"), AbstractEditor::tr("&Paste"), this);
	m_pasteAction->setEnabled(true);
	QObject::connect(m_pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
	QObject::connect(this, SIGNAL(pasteAvailable(bool)), m_pasteAction, SLOT(setEnabled(bool)));
}

void AbstractEditor::initLayout()
{

}

AbstractEditor::~AbstractEditor()
{
	desactivateWatcher();
	delete m_watcher;

	// Actions deleted as children
}

QString AbstractEditor::getTitle() const
{
	if (! m_lastFileName.isEmpty())
		return QFileInfo(m_lastFileName).fileName();
	else
		return defaultFileName();
}

QString AbstractEditor::getLongTitle() const
{
	if (! m_lastFileName.isEmpty())
		return m_lastFileName;
	else
		return defaultFileName();
}

void AbstractEditor::loadFromFile(const QString & fileName)
{
	if (! fileName.isEmpty()) setWatcher(fileName);

	QIODevice * file = new QFile(m_lastFileName);
	foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
	{
		if (e->isActivated() && qobject_cast<IXinxInputOutputPlugin*>(e->plugin()))
		{
			file = qobject_cast<IXinxInputOutputPlugin*>(e->plugin())->loadFile(m_lastFileName);
			if (file) break;
		}
	}
	if (! file)
		file = new QFile(m_lastFileName);

	if (! file->open(QIODevice::ReadOnly))
	{
		qCritical(qPrintable(tr("Can't open file for reading %1 : %2").arg(m_lastFileName).arg(file->errorString())));
		return;
	}
	loadFromDevice(*file);

	delete file;

	setModified(false);
}

void AbstractEditor::saveToFile(const QString & fileName)
{
	/* Make a backup of the file */
	if ((fileName.isEmpty() || (fileName == m_lastFileName))
	        && XINXConfig::self()->config().editor.createBackupFile)
	{

		if (QFile::exists(fileName + ".bak"))
			QFile::remove(fileName + ".bak");
		QFile::copy(fileName, fileName + ".bak");
	}

	/* Create the device */
	QIODevice * file = 0;
	foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
	{
		if (e->isActivated() && qobject_cast<IXinxInputOutputPlugin*>(e->plugin()))
		{
			file = qobject_cast<IXinxInputOutputPlugin*>(e->plugin())->saveFile(fileName, m_lastFileName);
			if (file) break;
		}
	}
	if (! file)
		file = new QFile(fileName);

	/* Change the file name */
	if (! fileName.isEmpty()) setWatcher(fileName);

	m_isSaving = true;
	desactivateWatcher();
	qApp->processEvents();

	/* Open the file for writting an save */
	if (! file->open(QIODevice::WriteOnly))
	{
		qCritical(qPrintable(tr("Can't open file for writing %1 : %2").arg(m_lastFileName).arg(file->errorString())));
		m_isSaving = false;
		activateWatcher();
		delete file;
		return;
	}
	saveToDevice(*file);
	file->close();

	delete file;

	m_isSaving = false;
	activateWatcher();
	setModified(false);
}

QAction * AbstractEditor::undoAction()
{
	return m_undoAction;
}

QAction * AbstractEditor::redoAction()
{
	return m_redoAction;
}

QAction * AbstractEditor::cutAction()
{
	return m_cutAction;
}

QAction * AbstractEditor::copyAction()
{
	return m_copyAction;
}

QAction * AbstractEditor::pasteAction()
{
	return m_pasteAction;
}

QIcon AbstractEditor::icon() const
{
	return QIcon(":/images/typeunknown.png");
}

bool AbstractEditor::isModified()
{
	return m_modified;
}


void AbstractEditor::setModified(bool isModified)
{
	if (m_modified != isModified)
	{
		m_modified = isModified;
		emit modificationChanged(isModified);
	}
}

const QString & AbstractEditor::lastFileName() const
{
	return m_lastFileName;
}

void AbstractEditor::serialize(XinxProjectSessionEditor * data, bool content)
{
	Q_UNUSED(content);
	data->writeProperty("ClassName", metaObject()->className());   // Store the class name
	data->writeProperty("FileName", QVariant(QDir(data->projectPath()).relativeFilePath(m_lastFileName)));
	data->writeProperty("Modified", QVariant(m_modified));
}

void AbstractEditor::deserialize(XinxProjectSessionEditor * data)
{
	// Dont't read the class name, already read.
	const QString lastFileName = data->readProperty("FileName").toString();

	if (QFileInfo(lastFileName).isRelative())
		m_lastFileName = QDir(data->projectPath()).absoluteFilePath(lastFileName);
	else
		m_lastFileName = lastFileName;

	m_modified = data->readProperty("Modified").toBool();

	activateWatcher();

}

AbstractEditor * AbstractEditor::deserializeEditor(XinxProjectSessionEditor * data)
{
	QString name = data->readProperty("ClassName").toString();

	int id = QMetaType::type(qPrintable(name));
	if (id != -1)
	{
		void * editorPtr = QMetaType::construct(id);
		if (editorPtr)
		{
			AbstractEditor * editor = static_cast<AbstractEditor*>(editorPtr);
			editor->deserialize(data);
			return editor;
		}
	}
	return NULL;
}

void AbstractEditor::fileChanged()
{
	if (m_isSaving) return;
	if (! XINXConfig::self()->config().editor.popupWhenFileModified) return ;

	m_watcher->desactivate();
	if (QFile(m_lastFileName).exists() && QMessageBox::question(qApp->activeWindow(), tr("Reload page"), tr("The file %1 was modified. Reload the page ?").arg(QFileInfo(m_lastFileName).fileName()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		loadFromFile();
	else
		setModified(true);

	if (! QFile(m_lastFileName).exists())
	{
		QMessageBox::warning(qApp->activeWindow(), tr("Reload page"), tr("The file %1 was removed.").arg(QFileInfo(m_lastFileName).fileName()));
		setModified(true);
	}
	m_watcher->activate();
}

void AbstractEditor::desactivateWatcher()
{
	if (m_watcher)
		m_watcher->desactivate();
}

void AbstractEditor::activateWatcher()
{
	if (! m_watcher)
	{
		m_watcher = new FileWatcher(m_lastFileName);
		connect(m_watcher, SIGNAL(fileChanged()), this, SLOT(fileChanged()));
	}
	else
		m_watcher->activate();
}

void AbstractEditor::setWatcher(const QString & path)
{
	if (m_lastFileName != path)
	{
		delete m_watcher;
		m_lastFileName = path;
		activateWatcher();
	}
}
