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

/*!
 * \defgroup XinxEditors Editors for XINX
 */

/*!
 * \ingroup XinxEditors
 * \class AbstractEditor
 * \since 0.7.2.0

 * \brief Base class for construct editor for XINX.
 *
 * This base class contains minimum method necessary to construct an editor.
 * An editor has a name, a title, and method related with clipboard.
 *
 * \section ContentView Editor Content View
 *
 * The editor has a model for show content of it. The best way is to use
 * the ContentView2 name space and ContentView2::TreeModel to show the content
 * of the file, and for completion in the editor.
 *
 * This model will be show in the <em>Content View Dock</em> of XINX
 *
 * \image html abstracteditor1.png
 * \image latex abstracteditor1.png
 *
 * \section Serialization Editor Serialization
 *
 * An editor can be serialized and deserialized in a XinxProjectSessionEditor
 * (this object save datas in XML Document). For this child editor must registered
 * by using :
 *
 * \code
 *   qRegisterMetaType<MyEditor>("MyEditor");
 * \endcode
 *
 * The editor must use the Q_OBJECT macro too. The system use QObject::metaObject() classes
 * to get some editor information. You must save and restore your member in the methode serialize()
 * and deserialize().
 *
 * \section FileManagement Editor's files
 *
 * This class is also an editor that can read and write from and to a file. This class
 * open the file (with the correct option) and call loadFromDevice() and saveToDevice().
 * This method must be redifined to really read or save the file.
 *
 * If your goal is to make a text editor, you should use the object TextFileEditor instead of AbstractEditor.
 *
 * If a file is modified with an external application and in condition of XINXConfig::self()->config().editor.popupWhenFileModified
 * to true, a popup dialog ask to refresh the file in the editor. If the file wasn't modify
 * in the editor, the file is reloaded without prompt.
 */

/*!
 * \enum AbstractEditor::SearchOption
 * \brief Options used for search text in the editor.
 */

/*!
 * \var AbstractEditor::SearchOption AbstractEditor::ONLY_SELECTION
 * \brief Search only the selected element (text, widget, ...)
 */

/*!
 * \var AbstractEditor::SearchOption AbstractEditor::BACKWARD
 * \brief Inverse the searching mode and search before the cursor.
 */

/*!
 * \var AbstractEditor::SearchOption AbstractEditor::WHOLE_WORDS
 * \brief Search a whole word and not a piece of word
 */

/*!
 * \var AbstractEditor::SearchOption AbstractEditor::REGULAR_EXPRESSION
 * \brief The search string is a regular expression.
 */

/*!
 * \var AbstractEditor::SearchOption AbstractEditor::SEARCH_FROM_START
 * \brief Search from the start of the document (backward and selection must be ignored)
 */

/*!
 * \var AbstractEditor::SearchOption AbstractEditor::MATCH_CASE
 * \brief The search is case sensitive
 */


/*!
 * \brief Create an objects of type AbstractEditor.
 *
 * An editor is based on the QFrame object. Your widgets must be added in the QFrame object,
 * with help of a QLayout. You can use BorderLayout of the components library or Qt layout.
 *
 * The layout of the frame will be created later by EditorFactory. For this you must re-implemente initLayout()
 *
 * \param parent Parent and containers of the editor.
 */
AbstractEditor::AbstractEditor(QWidget * parent)  : QFrame(parent), m_isSaving(false), m_modified(false)
{
	initObjects();
}

/*!
 * \brief Constructor used to copy the editor content.
 *
 * This constructor must only exist for serialization works. You don't have to create your own copy constructor.
 * The editor use the same parent of \e editor but don't copy members of AbstractEditor. It's used to create a new
 * AbstractEditor.
 *
 * \param editor The original editor used for copy.
 */
AbstractEditor::AbstractEditor(const AbstractEditor & editor) : QFrame(qobject_cast<QWidget*>(editor.parent())), m_isSaving(false), m_modified(false)
{
	initObjects();
}

/*!
 * \internal
 */
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

/*!
 * \brief This method is used to initialize the layout.
 *
 * It can be used to add an header, a footer or to create an editor with a grid layout.
 * All your imagination can be here.
 * The TextFileEditor will have it's own layout, who can be replaced later by a child class.
 *
 * By default this method do nothing (no layout created).
 */
void AbstractEditor::initLayout()
{

}

/*!
 * \brief Destroy the created frame editor.
 */
AbstractEditor::~AbstractEditor()
{
	desactivateWatcher();
	delete m_watcher;

	// Actions deleted as children
}

/*!
 * \fn virtual bool AbstractEditor::canCopy() = 0
 * \brief Check if the editor has the capacity to copy or cut data to the clipboard.
 *
 * In practice, this method must return true if the user have selected text, element, ...
 * The signal copyAvailable() is emmited when this value change.
 * \return true if the editor can copy or cut data to clipboard else return false.
 * \sa copy(), cut(), copyAvailable()
 */

/*!
 * \fn virtual bool AbstractEditor::canPaste() = 0
 * \brief Check if the editor can accept paste data from clipboard.
 *
 * The signal pasteAvailable() is emmited when this value change.
 * \return true if the editor can past data from clipboard.
 * \sa paste(), pasteAvailable()
 */

/*!
 * \fn virtual bool AbstractEditor::canUndo() = 0
 * \brief Check if the editor can undo last command.
 *
 * The signal undoAvailable() is emmited when this value change.
 * \return true if the editor can undo last change, else return false.
 * \sa undo(), undoAvailable()
 */

/*!
 * \fn virtual bool AbstractEditor::canRedo() = 0
 * \brief Check if the editor can redo change made by undo.
 *
 * The signal redoAvailable() is emmited when this value change.
 * \return true if the editor can redo last change, else return false.
 * \sa redo(), redoAvailable()
 */

/*!
 * \fn virtual void AbstractEditor::loadFromDevice(QIODevice & d) = 0
 * \brief Load the editor from a device (from a file, the memory or any medium).
 *
 * The QIODevice must be open for reading and seek to the position 0.
 *
 * \note If you want use a Text editor, you should use TextFileEditor who already re-implement
 * this method.
 *
 * \param d The device where XINX must load.
 */

/*!
 * \fn virtual void AbstractEditor::saveToDevice(QIODevice & d) = 0
 * \brief Save the editor in a device(to a file, in the memory, or any medium).
 *
 * The QIODevice must be open for reading and seek to the position 0.
 *
 * \note If you want use a Text editor, you should use TextFileEditor who already re-implement
 * this method.
 *
 * \param d The device where XINX must write.
 */

/*!
 * \fn virtual QAbstractItemModel * AbstractEditor::model() const = 0
 * \brief Return the model that represent the content of the editor.
 *
 * This model will be show in the in the <em>Content View Dock</em> of XINX as
 * describe \ref ContentView upper.
 */

/*!
 * \fn virtual void AbstractEditor::updateModel() = 0
 * \brief Update the content of the model from the content of the editor
 */

/*!
 * \fn virtual QString AbstractEditor::defaultFileName() const = 0
 * \brief Return the file name name to use if no filename is defined (ie. noname.txt)
 *
 * This method is used to known the file extention, and the default file name to propose
 * to the user when saving.
 */

/*!
 * \fn virtual void AbstractEditor::initSearch(AbstractEditor::SearchOptions & options) = 0
 * \brief This method will be called before the search/replace begins to define the option to use
 * and initialize some variables.
 *
 * Options to use are defined in AbstractEditor::SearchOption. This method can prepare the search
 * by init the cursor, ...
 * \param options Options used to search text.
 */

/*!
 * \fn virtual bool AbstractEditor::find(const QString & text, AbstractEditor::SearchOptions options) = 0
 * \brief Search the \e text in the document, and select it.
 *
 * The parameter \p options is the same of the parameter throw via initSearch().
 * \param text The text to search in the document
 * \param options User options used to find the text
 */

/*!
 * \fn virtual void AbstractEditor::replace(const QString & from, const QString & to, AbstractEditor::SearchOptions options) = 0
 * \brief Replace the current selection by the user text.
 *
 * This method is called after the method find(), if the searching is in mode \e replace.
 * The parameter \p options is the same of the parameter throw via initSearch().
 * @param from The text to replace (as asked by user so be aware of regexp)
 * @param to The text the user want to put. (if regexp \\1, \\2 is catched text)
 * @param options User options used to replace the text
 */

/*!
 * \fn virtual void AbstractEditor::undo() = 0
 * \brief Call undo operation on the editor, if available.
 *
 * This operation rollback the last modification made on the editor.
 * \sa canUndo(), undoAvailable()
 */

/*!
 * \fn virtual void AbstractEditor::redo() = 0
 * \brief Call redo operation on the editor, if available.
 *
 * This operation rollback the last undo command and redo the last operaton
 * \sa canRedo(), redoAvailable()
 */

/*!
 * \fn virtual void AbstractEditor::cut() = 0
 * \brief Cut selected data on the editor to the clipboard.
 * \sa canCopy(), copy(), copyAvailable()
 */

/*!
 * \fn virtual void AbstractEditor::copy() = 0
 * \brief Copy selected data on the editor to the clipboard.
 * \sa canCopy(), cut(), copyAvailable()
 */

/*!
 * \fn virtual void AbstractEditor::paste() = 0
 * \brief Paste data from the clipboard on the editor.
 * \sa canPaste(), pasteAvailable()
 */

/*!
 * \fn void AbstractEditor::modificationChanged(bool isModified)
 * \brief Signal emited when the \e modification state change.
 *
 * The modification state will be used by example to show the [*] after the filename.
 * \param isModified This value contains the \e modification state of the editor.
 */

/*!
 * \fn void AbstractEditor::undoAvailable(bool available)
 * \brief Signal emitted when the undo state change.
 * \param available  true if undo is Available, else false
 * \sa canUndo(), undoAvailable()
 */

/*!
 * \fn void AbstractEditor::redoAvailable(bool available)
 * \brief Signal emitted when the redo state change.
 * \param available  true if redo available, else false.
 * \sa canRedo(), redoAvailable()
 */

/*!
 * \fn void AbstractEditor::copyAvailable(bool available)
 * \brief Signal emitted when the cut/copy state change.
 * \param available true if copy or cut is available (e.g. a text is selected), else false
 * \sa copy(), cut(), canCopy()
 */

/*!
 * \fn void AbstractEditor::pasteAvailable(bool available)
 * \brief Signal emitted when the paste state change.
 * \param available  true if paste is available, else false.
 * \sa canPaste(), paste()
 */

/*!
 * \fn void AbstractEditor::open(const QString & filename, int line)
 * \brief Signal emited when the editor request to open a file is made.
 *
 * This signal can be emited when the user \c Ctrl+CLICK to ask to open the
 * file \p filename at the position \p line where you can find the definition
 * of the word choose by the user.
 *
 * \param filename file name of the file to open
 * \param line line of the file to open
 */

/*!
 * \fn void AbstractEditor::contentChanged()
 * \brief Signal emited when the content of the editor change.
 */

/*!
 * \fn virtual BookmarkEditorInterface * AbstractEditor::bookmarkInterface() = 0
 * \brief Return the interface used to navigate throw Bookmark.
 *
 * Bookmark are manager throw the BookmarkEditorInterface. This interface must be reimplemented
 * in the child class of AbstractEditor.
 */

/*!
 * \brief Get the title of the editor.
 *
 * Return the title of the FileEditor. The title return the value of defaultFileName(),
 * if the method getFileName() return an empty string else return the File name (without the path).
 * \return The title to use with Tab name
 * \sa getFileName(), hasName()
 */
QString AbstractEditor::getTitle() const
{
	if (! m_lastFileName.isEmpty())
		return QFileInfo(m_lastFileName).fileName();
	else
		return defaultFileName();
}

/*!
 * \brief Get the long title of the editor.
 *
 * It can be use on the TabWidget to inform user in a tool type. If the method getFileName()
 * return an empty string, the editor retorun the content of defaultFileName(), else the
 * filename with path is returned.
 * \return The title of frame.
 * \sa hasName()
 */
QString AbstractEditor::getLongTitle() const
{
	if (! m_lastFileName.isEmpty())
		return m_lastFileName;
	else
		return defaultFileName();
}

/*!
 * \brief Load the editor from the file \p fileName
 *
 * This method open the file \p fileName and pass an opened QFile to loadFromDevice().
 *
 * This method call method IXinxInputOutputPlugin::loadFile() of all plugin who implements this method.
 *
 * \param fileName The file name to load
 * \sa saveToFile()
 */
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

/*!
 * \brief Save the editor's content to file \e fileName
 *
 * This method open the file \p fileName and pass an opened QFile to saveToDevice().
 * Before saving, if XINXConfig::self()->config().editor.createBackupFile is true, a backup file
 * is created in appending ".bak" to the original filename.
 *
 * This method call method IXinxInputOutputPlugin::saveFile() of all plugin who implements this method.

 * \param fileName The file name to save.
 * \sa loadFromFile()
 */
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

/*!
 * \brief Return the action used to undo operations in the editor.
 *
 * This action call the \e undo().
 * \return Return the action to undo.
 * \sa undo(), undoAvailable()
 */
QAction * AbstractEditor::undoAction()
{
	return m_undoAction;
}

/*!
 * \brief Return the action used to redo operations in the editor.
 *
 * This action call the \e redo().
 * \return Return the action to redo.
 * \sa redo(), redoAvailable()
 */
QAction * AbstractEditor::redoAction()
{
	return m_redoAction;
}

/*!
 * \brief Return the action used to cut a selected text in the editor.
 *
 * This action call the \e cut().
 * \return Return the action to cut.
 * \sa cut(), copyAvailable(), canCopy()
 */
QAction * AbstractEditor::cutAction()
{
	return m_cutAction;
}

/*!
 * \brief Return the action used to copy a selected text in the editor.
 *
 * This action call the \e copy().
 * \return Return the action to copy.
 * \sa copy(), copyAvailable(), canCopy()
 */
QAction * AbstractEditor::copyAction()
{
	return m_copyAction;
}

/*!
 * \brief Return the action used to past a text in the editor.
 *
 * This action call the \e paste().
 * \return Return the action to paste.
 * \sa copy(), pasteAvailable(), canPaste()
 */
QAction * AbstractEditor::pasteAction()
{
	return m_pasteAction;
}

/*!
 * \brief Return an icon that represent the editor.
 * \return an icon for the editor.
 */
QIcon AbstractEditor::icon() const
{
	return QIcon(":/images/typeunknown.png");
}

/*!
 * \brief Return true if the editor is modified by the user, else return false
 */
bool AbstractEditor::isModified()
{
	return m_modified;
}


/*!
 * \brief Set the modified attribute in local.
 * \param isModified The new value for the modified attribute.
 */
void AbstractEditor::setModified(bool isModified)
{
	if (m_modified != isModified)
	{
		m_modified = isModified;
		emit modificationChanged(isModified);
	}
}

/*!
 * \brief Return the last name used with \e loadFromFile() or \e saveToFile()
 */
const QString & AbstractEditor::lastFileName() const
{
	return m_lastFileName;
}

/*!
 * \brief Serialize the editor and return the value in a XinxProjectSessionEditor.
 *
 * The serialization save internal data of the editor (modified, content,
 * position of cursor, file name, ...). The format of the data depend of XinxProjectSessionEditor.
 *
 * \param data where datas must be stored.
 * \param content If true, the editor save the modifed content, else the editor must save only
 * the state.
 * \sa deserialze(), deserialzeEditor()
 */
void AbstractEditor::serialize(XinxProjectSessionEditor * data, bool content)
{
	Q_UNUSED(content);
	data->writeProperty("ClassName", metaObject()->className());   // Store the class name
	data->writeProperty("FileName", QVariant(QDir(data->projectPath()).relativeFilePath(m_lastFileName)));
	data->writeProperty("Modified", QVariant(m_modified));
}

/*!
 * \brief Restore the editor with the content of the XML document.
 *
 * The deserialization restore the maximum of information of the document.
 *
 * \param data from what the data must be read
 * \sa serialize(), deserializeEditor()
 */
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

/*!
 * \brief Create the editor and deseroalize it.
 *
 * This \e static method is used by EditorFactory to restore the editor. This
 * method create the right class with the help of the \c ClassName property,
 * QMetaType::type(name) and QMetaType::construct(id).
 *
 * Next this method call the \e virtual method deserialize()
 *
 * \param data from what the data must be read
 * \return An editor
 * \sa serialize(), deserialize()
 */
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
