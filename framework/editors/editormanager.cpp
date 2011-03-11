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

// Private header
#include "editormanager_p.h"
#include <editors/editorchoicedlg.h>
#include <session/sessionmanager.h>
#include <plugins/xinxpluginelement.h>
#include <core/uniqueapplication.h>
#include <application/mainformimpl.h>
#include "newfilewizardimpl.h"
#include <project/xinxprojectmanager.h>
#include <core/xinxcore.h>
#include <session/sessioneditor.h>

/* PrivateTabWidget */

PrivateTabWidget::PrivateTabWidget(EditorManager* parent) : _manager(parent)
{
	setAcceptDrops(true);
	setMovable(true);
	tabBar()->setAttribute(Qt::WA_Hover);
}

PrivateTabWidget::~PrivateTabWidget()
{
	tabBar()->removeEventFilter(_manager->d);
}

QTabBar * PrivateTabWidget::tabBar() const
{
	return QTabWidget::tabBar();
}

void PrivateTabWidget::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData *mimeData = event->mimeData();
	if (mimeData->hasUrls())
	{
		setBackgroundRole(QPalette::Highlight);
		event->acceptProposedAction();
	}
}

void PrivateTabWidget::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();
	const QList<QUrl> & urls = mimeData->urls();
	if (mimeData->hasUrls())
	{
		for (int i = 0; i < urls.size(); i++)
		{
			if ((!urls.at(i).toLocalFile().isEmpty()))
			{
				_manager->openFile(urls.at(i).toLocalFile());
			}
		}

		setBackgroundRole(QPalette::NoRole);
		event->acceptProposedAction();
	}
}

/* PrivateEditorManager */

PrivateEditorManager::PrivateEditorManager(EditorManager * manager) : _manager(manager), _clicked_item(-1)
{
	// Création de l'objet tabWidget
	tabWidget();

	createActions();
	createCloseButton();

	connect(_tab_widget, SIGNAL(currentChanged(int)), _manager, SIGNAL(currentChanged(int)));
	connect(_tab_widget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));

	connect(_manager, SIGNAL(fileOpened(QString)), this, SLOT(updateRecentFiles()));
	connect(_manager, SIGNAL(fileSaved(QString)), this, SLOT(updateRecentFiles()));

	connect(XINXConfig::self(), SIGNAL(changed()), this, SLOT(updateConfigElement()));

	updateRecentFiles();
	updateActions();
}

PrivateEditorManager::~PrivateEditorManager()
{
}

PrivateTabWidget * PrivateEditorManager::tabWidget()
{
	if (! _tab_widget)
	{
		_tab_widget = new PrivateTabWidget(_manager);
		_tab_widget->tabBar()->installEventFilter(this);
	}
	return _tab_widget;
}

void PrivateEditorManager::connectEditor(AbstractEditor * editor)
{
	connect(editor, SIGNAL(open(QString,int)),			_manager, SLOT(openFile(QString,int)));
	connect(editor, SIGNAL(contentChanged()),			_manager, SIGNAL(contentChanged()));
	connect(editor, SIGNAL(codecChanged()),				_manager, SIGNAL(codecChanged()));
	connect(editor, SIGNAL(modificationChanged(bool)),	this,     SLOT(slotModifiedChange()));
	connect(editor, SIGNAL(positionChanged(QString)), 	_manager, SIGNAL(positionChanged(QString)));

	/*
	 FIXME:
	if (editor->metaObject()->indexOfSignal("positionInEditorChanged(QModelIndex)") >= 0)
		connect(editor, SIGNAL(positionInEditorChanged(QModelIndex)), _manager, SIGNAL(positionChanged(QModelIndex)));
		*/
}

void PrivateEditorManager::disconnectEditor(AbstractEditor * editor)
{
	editor->disconnect(_manager);
	editor->disconnect(this);
}

void PrivateEditorManager::newFile()
{
	NewFileWizardImpl dlg(qApp->activeWindow());
	dlg.setPath(QString());
	if (dlg.exec() == QDialog::Accepted)
	{
		Q_ASSERT_X(dlg.selectedType(), "PrivateEditorManager::newFile", "No interface editor defined");
		Q_ASSERT_X(dlg.project(), "PrivateEditorManager::newFile", "No project defined");

		_manager->openFile(dlg.filename(), dlg.selectedType(), dlg.project());
	}
}

void PrivateEditorManager::openRecentFile()
{
	QAction * action = qobject_cast<QAction*>(sender());
	if (action)
	{
		_manager->openFile(action->data().toString());
	}
}

void PrivateEditorManager::closeAllExcpet()
{
	int item = getClickedTab();
	if (item == -1) item = tabWidget()->currentIndex();

	tabWidget()->setUpdatesEnabled(false);

	QList<AbstractEditor*> editors;
	for (int i = tabWidget()->count() - 1; i >= 0; i--)
	{
		if (i != item)
		{
			editors << _manager->editor(i);
		}
	}

	if (editorMayBeSave(editors))
	{
		foreach(AbstractEditor * e, editors)
		{
			_manager->closeFile(e, false); // Confirmation already made
		}
	}

	tabWidget()->setUpdatesEnabled(true);
}

void PrivateEditorManager::createOpenSubMenu()
{
	QAction * openAction = new QAction(QIcon(":/images/fileopen.png"), tr("&Open ..."), this);
	openAction->setShortcut(QKeySequence::Open);
	openAction->setToolTip(tr("Open a file on the disk"));
	openAction->setStatusTip(tr("Open an existing file on the disk"));
	connect(openAction, SIGNAL(triggered()), _manager, SLOT(openFile()));

	QMenu * recentFileMenu = new QMenu;
	recentFileMenu->addAction(openAction);

	_recent_action = new QAction(QIcon(":/images/fileopen.png"), tr("&Recent file"), this);
	_recent_action->setMenu(recentFileMenu);
	connect(_recent_action, SIGNAL(triggered()), _manager, SLOT(openFile()));

	_recent_separator = recentFileMenu->addSeparator();
	_recent_separator->setVisible(false);

	for (int i = 0; i < MAXRECENTFILES; i++)
	{
		_recent_actions[i] = new QAction(this);
		_recent_actions[i]->setVisible(false);
		recentFileMenu->addAction(_recent_actions[i]);
		connect(_recent_actions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}
}

void PrivateEditorManager::updateRecentFiles()
{
	int numRecentFiles = qMin(XinxSession::SessionManager::self()->currentSession()->lastOpenedFile().size(), MAXRECENTFILES);

	for (int i = 0; i < numRecentFiles; i++)
	{
		QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(XinxSession::SessionManager::self()->currentSession()->lastOpenedFile()[i]).fileName());
		if (XinxPluginsLoader::self()->matchedFileType(QFileInfo(text).fileName()).size())
			_recent_actions[i]->setIcon(QIcon(XinxPluginsLoader::self()->matchedFileType(QFileInfo(text).fileName()).at(0)->icon()));
		_recent_actions[i]->setText(text);
		_recent_actions[i]->setData(XinxSession::SessionManager::self()->currentSession()->lastOpenedFile()[i]);
		_recent_actions[i]->setVisible(true);
	}

	for (int j = numRecentFiles; j < MAXRECENTFILES; j++)
		_recent_actions[j]->setVisible(false);

	_recent_separator->setVisible(numRecentFiles > 0);
}

void PrivateEditorManager::createActions()
{
	_new_action = new QAction(QIcon(":/images/filenew.png"), tr("&New"), this);
	_new_action->setToolTip(tr("New document"));
	_new_action->setStatusTip(tr("Create a new document"));
	_new_action->setShortcut(QKeySequence::New);
	connect(_new_action, SIGNAL(triggered()), this, SLOT(newFile()));

	createOpenSubMenu();

	_refresh_action = new QAction(QIcon(":/images/reload.png"), tr("&Refresh"), this);
	connect(_refresh_action, SIGNAL(triggered()), _manager, SLOT(refreshFile()));

	_save_action = new QAction(QIcon(":/images/filesave.png"), tr("&Save"), this);
	_save_action->setToolTip(tr("Save the document"));
	_save_action->setStatusTip(tr("Save the document to disk"));
	_save_action->setShortcut(QKeySequence::Save);
	connect(_save_action, SIGNAL(triggered()), _manager, SLOT(saveFile()));

	_save_as_action = new QAction(QIcon(":/images/filesaveas.png"), tr("Save &As..."), this);
	_save_as_action->setToolTip(tr("Save the document As"));
	_save_as_action->setStatusTip(tr("Save the document with a new name to the disk"));
	connect(_save_as_action, SIGNAL(triggered()), _manager, SLOT(saveAsFile()));

	_save_all_action = new QAction(QIcon(":/images/filesaveall.png"), tr("Save A&ll..."), this);
	_save_all_action->setToolTip(tr("Save all documents"));
	_save_all_action->setStatusTip(tr("Save all documents to disk"));
	connect(_save_all_action, SIGNAL(triggered()), _manager, SLOT(saveAllFile()));

	_print_action =  new QAction(QIcon(":/images/fileprint.png"), tr("&Print ..."), this);
	_print_action->setToolTip(tr("Print the document"));
	_print_action->setStatusTip(tr("Print the current document"));
	_print_action->setShortcut(QKeySequence::Print);
	connect(_print_action, SIGNAL(triggered()), _manager, SLOT(printFile()));

	_close_action = new QAction(QIcon(":/images/fileclose.png"), tr("&Close"), this);
	_close_action->setToolTip(tr("Close document"));
	_close_action->setStatusTip(tr("Close the current document"));
	_close_action->setShortcut(QKeySequence::Close);
	connect(_close_action, SIGNAL(triggered()), _manager, SLOT(closeFile()));

	_close_all_action = new QAction(tr("&Close All"), this);
	_close_all_action->setToolTip(tr("Close All document"));
	_close_all_action->setStatusTip(tr("Close All document"));
	connect(_close_all_action, SIGNAL(triggered()), _manager, SLOT(closeAllFile()));

	_close_all_except_action = new QAction(tr("C&lose All except current"), this);
	_close_all_except_action->setToolTip(tr("Close all document except the current"));
	_close_all_except_action->setStatusTip(tr("Close all document except the current"));
	connect(_close_all_except_action, SIGNAL(triggered()), this, SLOT(closeAllExcpet()));

	_copy_filename_action = new QAction(tr("&Copy filename to Clipboard"), this);
	connect(_copy_filename_action, SIGNAL(triggered()), _manager, SLOT(copyFileNameToClipboard()));

	_copy_path_action = new QAction(tr("C&opy path to clipboard"), this);
	connect(_copy_path_action, SIGNAL(triggered()), _manager, SLOT(copyPathToClipboard()));

	_next_tab_action = new QAction(tr("Next tab"), this);
	_next_tab_action->setToolTip(tr("Next tab"));
	_next_tab_action->setShortcut(QKeySequence::NextChild);
	connect(_next_tab_action, SIGNAL(triggered()), _manager, SLOT(changeToNextEditor()));

	_previous_tab_action = new QAction(tr("Previous Tab"), this);
	_previous_tab_action->setShortcut(QKeySequence::PreviousChild);
	connect(_previous_tab_action, SIGNAL(triggered()), _manager, SLOT(changeToPreviousEditor()));
}

void PrivateEditorManager::updateActions()
{
	_save_action->setEnabled(tabWidget()->count());
	_save_as_action->setEnabled(tabWidget()->count());
	_save_all_action->setEnabled(tabWidget()->count());
	_close_action->setEnabled(tabWidget()->count());
	_close_all_action->setEnabled(tabWidget()->count());
	_print_action->setEnabled(tabWidget()->count());
	_previous_tab_action->setEnabled(tabWidget()->count());
	_next_tab_action->setEnabled(tabWidget()->count());
}


void PrivateEditorManager::createCloseButton()
{
	_close_tab_button = new QToolButton(tabWidget());
	_close_tab_button->setIcon(QIcon(":/images/tabclose.png"));
	connect(_close_tab_button, SIGNAL(clicked()), _manager, SLOT(closeFile()));

	tabWidget()->setCornerWidget(_close_tab_button);

	_close_tab_button->setVisible(! XINXConfig::self()->config().editor.hideCloseTab);
	tabWidget()->setTabsClosable(XINXConfig::self()->config().editor.closeButtonOnEachTab);
}

void PrivateEditorManager::updateConfigElement()
{
	_close_tab_button->setVisible(! XINXConfig::self()->config().editor.hideCloseTab);
	tabWidget()->setTabsClosable(XINXConfig::self()->config().editor.closeButtonOnEachTab);
}

void PrivateEditorManager::slotModifiedChange()
{
	AbstractEditor * editor = qobject_cast<AbstractEditor*>(sender());
	Q_ASSERT_X(editor, "PrivateEditorManager::slotModifiedChange", "The slot must be called from an AbstractEditor");

	updateTabWidget(editor);
}

void PrivateEditorManager::updateTabWidget(AbstractEditor * editor)
{
	Q_ASSERT(tabWidget()->indexOf(editor) != -1);

	const QFontMetrics & metrics = tabWidget()->fontMetrics();

	int index = tabWidget()->indexOf(editor);
	int width = qApp->desktop()->screenGeometry().width();

	QString title = editor->getTitle();
	QString longTitle = metrics.elidedText(editor->getLongTitle(), Qt::ElideLeft, width);
	QString projectLine;

	if (!editor->project())
	{
		projectLine = tr("The project isn't correctly defined. It's a possible bug");
	}
	else if (editor->project() == XinxProject::Manager::self()->defaultProject())
	{
		projectLine = tr("The file isn't a part of a project");
	}
	else
	{
		projectLine = tr("The project name is \"%1\"").arg(editor->project()->projectName());
	}

	if (editor->isModified()) title += "*";

	tabWidget()->setTabText(index, title);
	tabWidget()->setTabToolTip(index, longTitle + "\n" + projectLine);
	tabWidget()->setTabIcon(index, editor->icon());
}

void PrivateEditorManager::addTab(AbstractEditor * editor)
{
	Q_ASSERT_X(editor, "PrivateEditorManager::addTab", "Editor can't be null");

	connectEditor(editor);

	int index = tabWidget()->addTab(editor, QString());
	updateTabWidget(editor);

	_manager->setCurrentEditor(index);
}

void PrivateEditorManager::tabCloseRequested(int index)
{
	_manager->closeFile(index);
}

int PrivateEditorManager::getClickedTab()
{
	int item = _clicked_item;
	_clicked_item = -1;
	return item;
}

bool PrivateEditorManager::editorMayBeSave(QList<AbstractEditor*> editors)
{
	SavingDialog savingDlg(qApp->activeWindow());

	foreach(AbstractEditor * editor, editors)
	{
		if (editor && (editor->isModified() || (qobject_cast<TextFileEditor*>(editor) && (!editor->hasNeverBeenModified()) && XINXConfig::self()->config().editor.autoindentOnSaving == "closing")))
		{
			savingDlg.addEditor(editor);
		}
	}

	if (savingDlg.countEditor())
	{
		if (savingDlg.exec() == QDialog::Rejected)
			return false;

		foreach(AbstractEditor * editor, savingDlg.selectedEditor())
		{
			if (XINXConfig::self()->config().editor.autoindentOnSaving == "closing")
			{
				TextFileEditor * textFileEditor = qobject_cast<TextFileEditor*>(editor);
				if (textFileEditor)
				{
					textFileEditor->autoIndent();
				}
			}
			_manager->saveFile(editor);
		}
	}

	return true;
}

int PrivateEditorManager::tabPosition(QPoint point)
{
	int clickedItem = -1;

	for (int i = 0; i < tabWidget()->tabBar()->count(); i++)
	{
		if (tabWidget()->tabBar()->tabRect(i).contains(point))
		{
			clickedItem = i;
			break;
		}
	}

	return clickedItem;
}

bool PrivateEditorManager::eventFilter(QObject* obj, QEvent* event)
{
	if (obj==tabWidget()->tabBar())
	{
		if (((event->type() == QEvent::MouseButtonPress) && (static_cast<QMouseEvent *>(event)->button() == Qt::RightButton)) || (event->type() == QEvent::MouseButtonDblClick))
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			_clicked_item = tabPosition(mouseEvent->pos());
			if (_clicked_item == -1) return QObject::eventFilter(obj, event);

			if ((event->type() == QEvent::MouseButtonPress) && (mouseEvent->button() == Qt::RightButton))
			{
				QMenu *menu = new QMenu;

				menu->addAction(_refresh_action);
				menu->addSeparator();
				menu->addAction(_save_action);
				menu->addAction(_save_as_action);
				menu->addSeparator();
				menu->addAction(_close_action);
				menu->addAction(_close_all_action);
				menu->addAction(_close_all_except_action);
				menu->addSeparator();
				menu->addAction(_copy_filename_action);
				menu->addAction(_copy_path_action);

				menu->exec(mouseEvent->globalPos());
				delete menu;
			}
			else if ((event->type() == QEvent::MouseButtonDblClick) && (mouseEvent->button() == Qt::LeftButton))
			{
				_close_action->trigger();
			}
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}

/* EditorManager */

EditorManager::EditorManager()
{
	d = new PrivateEditorManager(this);
}

//! Destroy the class
EditorManager::~EditorManager()
{
	delete d;
}

void EditorManager::setCurrentEditor(AbstractEditor * editor)
{
	if (editor != d->tabWidget()->currentWidget())
	{
		d->tabWidget()->setCurrentWidget(editor);
		UniqueApplication::mainWindow()->activateWindow();
		editor->setFocus();
		emit currentChanged(d->tabWidget()->currentIndex());
	}
}

//! Show to the editor \e index
void EditorManager::setCurrentEditor(int index)
{
	if (index != d->tabWidget()->currentIndex())
	{
		d->tabWidget()->setCurrentIndex(index);
		UniqueApplication::mainWindow()->activateWindow();
		emit currentChanged(currentIndex());
	}
}

int EditorManager::currentIndex() const
{
	return d->tabWidget()->currentIndex();
}

//! Return the current editor
AbstractEditor * EditorManager::currentEditor() const
{
	return dynamic_cast<AbstractEditor*>(d->tabWidget()->currentWidget());
}

//! Return the editor at index \e index
AbstractEditor * EditorManager::editor(int index) const
{
	return dynamic_cast<AbstractEditor*>(d->tabWidget()->widget(index));
}

/*!
 * \brief Return the editor of name \e filename.
 *
 * The \p filename must not be in the native separator and must be absolute.
 */
AbstractEditor * EditorManager::editor(const QString & filename) const
{
	for (int i = 0; i < d->tabWidget()->count(); i++)
	{
		AbstractEditor * ed = dynamic_cast<AbstractEditor*>(editor(i));
		if (ed && (ed->lastFileName() == filename))
			return editor(i);
	}
	return NULL;
}

//! Return the list of editors
QList<AbstractEditor*> EditorManager::editors() const
{
	QList<AbstractEditor*> list;
	for (int i = 0 ; i < d->tabWidget()->count() ; i++)
	{
		list << editor(i);
	}
	return list;
}

//! Return the number of editor
int EditorManager::editorsCount() const
{
	return d->tabWidget()->count();
}

//! Show the next editor
void EditorManager::changeToNextEditor()
{
	setCurrentEditor((d->tabWidget()->currentIndex() + 1) % d->tabWidget()->count());
}

//! Show the previous editor
void EditorManager::changeToPreviousEditor()
{
	setCurrentEditor((d->tabWidget()->currentIndex() - 1 + d->tabWidget()->count()) % d->tabWidget()->count());
}

void EditorManager::openFile()
{
	QStringList selectedFiles = QFileDialog::getOpenFileNames(qApp->activeWindow(), tr("Open text file"), d->m_lastOpenedFileName, XinxPluginsLoader::self()->openDialogBoxFilters().join(";;"));

	d->tabWidget()->setUpdatesEnabled(false);
	foreach(const QString & filename, selectedFiles)
	{
		d->m_lastOpenedFileName = QFileInfo(filename).absolutePath();
		openFile(filename);
	}
	d->tabWidget()->setUpdatesEnabled(true);
}

void EditorManager::openFile(const QString& filename, IFileTypePlugin* interface, XinxProject::ProjectPtr project)
{
	Q_ASSERT(! filename.isEmpty());

	AbstractEditor * openingEditor = editor(filename);
	if (! openingEditor)
	{
		ScriptManager::self()->callScriptsBeforeLoad(currentEditor());

		openingEditor = EditorFactory::self()->createEditor(filename, interface, project);
		d->addTab(openingEditor);
		d->updateActions();
		emit codecChanged();

		ScriptManager::self()->callScriptsAfterLoad(openingEditor);

		// Add recent action
		XinxSession::SessionManager::self()->currentSession()->addOpenedFile(filename);

		serializeEditors();

		emit fileOpened(filename);
	}

	setCurrentEditor(openingEditor);
}

void EditorManager::openFile(const QString& filename, int line, IFileTypePlugin* interface, XinxProject::ProjectPtr project)
{
	if (! filename.isEmpty())
	{
		openFile(filename, interface, project);
	}
	else
	{
		// In this case we works on the current editor
	}

	// Deplace to rigth line.
	TextFileEditor * textFileEditor = qobject_cast<TextFileEditor*>(currentEditor());
	if (textFileEditor)
	{
		textFileEditor->textEdit()->gotoLine(line);
		textFileEditor->textEdit()->setFocus(Qt::OtherFocusReason);
	}
	else
	{
		qWarning("EditorManager::openFile(QString, int) : try to move to a line in a graphical editor");
	}
}

void EditorManager::saveAllFile()
{
	for (int i = 0; i < d->tabWidget()->count(); i++)
	{
		saveFile(i);
	}
}

void EditorManager::saveFile(bool saveAs)
{
	int item = d->getClickedTab();
	if (item == -1) item = d->tabWidget()->currentIndex();
	saveFile(item, saveAs);
}

void EditorManager::saveFile(int index, bool saveAs)
{
	Q_ASSERT_X(index >= 0, "EditorManager::saveFile", "The index must be positive");
	Q_ASSERT_X(index < d->tabWidget()->count(), "EditorManager::saveFile", "The index must be lesser than the number of editor");
	Q_ASSERT_X(editor(index), "EditorManager::saveFile", "The editor must be defined");

	saveFile(editor(index), saveAs);
}

void EditorManager::saveFile(AbstractEditor* editor, bool saveAs)
{
	Q_ASSERT_X(editor, "EditorManager::saveFile", "Editor must be initialized");

	const QString filename     = editor->lastFileName();
	const QString deffilename  = editor->defaultFileName();
	const QString filter       = XinxPluginsLoader::self()->matchedFileType(filename.isEmpty() ? deffilename : filename).size() ? XinxPluginsLoader::self()->fileTypeFilter(XinxPluginsLoader::self()->matchedFileType(filename.isEmpty() ? deffilename : filename).at(0)) : "";
	const bool    emptyname    = filename.isEmpty();

	bool    accept      = false;
	QString newFilename;

	foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
	{
		IXinxInputOutputPlugin * plugin = qobject_cast<IXinxInputOutputPlugin*>(e->plugin());

		if (e->isActivated() && plugin)
		{
			newFilename = plugin->getFilename(editor, filename, deffilename, filter, saveAs, accept, qApp->activeWindow());
			if (accept) break;
		}
	}
	if (! accept)
	{
		newFilename = filename;
		if (saveAs || emptyname)
		{
			if (newFilename.isEmpty() && editor->project())
			{
				newFilename = editor->project()->projectPath();
			}
			newFilename = QFileDialog::getSaveFileName(qApp->activeWindow(), tr("Save text file"), newFilename, filter, const_cast<QString*>(&filter));
		}
	}
	if (newFilename.isEmpty())
		return;

	if ((! QFile(newFilename).exists()) && editor->project() && editor->project()->rcsProxy())
	{
		editor->project()->rcsProxy()->addFileOperation(VersionControl::RCSProxy::RCS_ADD, QStringList() << newFilename, qApp->activeWindow());
	}

	ScriptManager::self()->callScriptsBeforeSave(editor);
	editor->saveToFile(newFilename);
	ScriptManager::self()->callScriptsAfterSave(editor);

	if (editor->project() && editor->project()->rcsProxy())
	{
		editor->project()->rcsProxy()->validFileOperations();
	}

	serializeEditors();

	emit fileSaved(QFileInfo(newFilename).absoluteFilePath());
}

void EditorManager::saveAsFile()
{
	saveFile(true);
}

void EditorManager::saveAsFile(int index)
{
	saveFile(index, true);
}

void EditorManager::saveAsFile(AbstractEditor* editor)
{
	saveFile(editor, true);
}

void EditorManager::refreshFile()
{
	int item = d->getClickedTab();
	if (item == -1) item = d->tabWidget()->currentIndex();
	refreshFile(item);
}

void EditorManager::refreshFile(AbstractEditor* editor)
{
	Q_ASSERT_X(editor, "EditorManager::refreshFile", "Editor must be initialized");

	bool act = ! editor->isModified();

	if (! act)
	{
		QMessageBox::StandardButton ret;
		ret = QMessageBox::question(qApp->activeWindow(), tr("Application"), tr("The document %1 has been modified.\nDo you really want refresh this?").arg(editor->getTitle()), QMessageBox::Yes | QMessageBox::No);
		act = (ret == QMessageBox::Yes);
	}

	if (act)
	{
		ScriptManager::self()->callScriptsBeforeLoad(editor);
		editor->loadFromFile();
		ScriptManager::self()->callScriptsAfterLoad(editor);
	}
}

void EditorManager::refreshFile(int index)
{
	Q_ASSERT_X(index >= 0, "EditorManager::refreshFile", "The index must be positive");
	Q_ASSERT_X(index < d->tabWidget()->count(), "EditorManager::refreshFile", "The index must be lesser than the number of editor");
	Q_ASSERT_X(editor(index), "EditorManager::refreshFile", "The editor must be defined");

	AbstractEditor * ed = qobject_cast<AbstractEditor*>(editor(index));
	refreshFile(ed);
}

void EditorManager::printFile(int index)
{
	Q_ASSERT_X(index >= 0, "EditorManager::printFile", "The index must be positive");
	Q_ASSERT_X(index < d->tabWidget()->count(), "EditorManager::printFile", "The index must be lesser than the number of editor");
	Q_ASSERT_X(editor(index), "EditorManager::printFile", "The editor must be defined");

	AbstractEditor * ed = qobject_cast<AbstractEditor*>(editor(index));
	printFile(ed);
}

void EditorManager::printFile(AbstractEditor* editor)
{
	TextFileEditor * textFileEditor = qobject_cast<TextFileEditor*>(editor);
	if (textFileEditor)
	{
		QPrinter printer;

		QScopedPointer<QPrintDialog> dlg(new QPrintDialog(&printer, qApp->activeWindow()));
		if (dlg->exec() != QDialog::Accepted) return;

		textFileEditor->textEdit()->print(&printer);

		emit filePrinted(textFileEditor->lastFileName());
	}
}

void EditorManager::printFile()
{
	int item = d->getClickedTab();
	if (item == -1) item = d->tabWidget()->currentIndex();
	printFile(item);
}

void EditorManager::closeFile(int index, bool confirm)
{
	Q_ASSERT_X(index >= 0, "EditorManager::closeFile", "The index must be positive");
	Q_ASSERT_X(index < d->tabWidget()->count(), "EditorManager::closeFile", "The index must be lesser than the number of editor");
	Q_ASSERT_X(editor(index), "EditorManager::closeFile", "The editor must be defined");

	AbstractEditor * ed = qobject_cast<AbstractEditor*>(editor(index));
	closeFile(ed, confirm);
}

void EditorManager::closeFile(AbstractEditor* editor, bool confirm)
{
	Q_ASSERT_X(editor, "EditorManager::refreshFile", "Editor must be initialized");

	if ((!confirm) || d->editorMayBeSave(QList<AbstractEditor*>() << editor))
	{
		emit fileClosing(editor->lastFileName());

		const int index = d->tabWidget()->indexOf(editor);
		Q_ASSERT_X(index >= 0, "EditorManager::closeFile", "Editor must be in EditorManager");

		d->tabWidget()->removeTab(index);
		d->updateActions();
		delete editor;

		serializeEditors();

		emit fileClosed(editor->lastFileName());
	}
}

void EditorManager::closeFile(bool confirm)
{
	int item = d->getClickedTab();
	if (item == -1) item = d->tabWidget()->currentIndex();
	closeFile(item, confirm);
}

bool EditorManager::closeAllFile(XinxProject::ProjectPtr project)
{
	d->tabWidget()->setUpdatesEnabled(false);

	QList<AbstractEditor*> editors;
	for (int i = d->tabWidget()->count() - 1; i >= 0; i--)
	{
		if ((!project) || (editor(i)->project().data() == project.data()))
		{
			editors << editor(i);
		}
	}

	if (d->editorMayBeSave(editors))
	{
		foreach(AbstractEditor * e, editors)
		{
			closeFile(e, false); // Confirmation already made
		}

		d->tabWidget()->setUpdatesEnabled(true);

		return true;
	}
	else
	{
		d->tabWidget()->setUpdatesEnabled(true);
		return false;
	}
}

void EditorManager::copyFileNameToClipboard()
{
	int item = d->getClickedTab();
	if (item == -1) item = d->tabWidget()->currentIndex();
	QString name = editor(item)->getTitle();
	qApp->clipboard()->setText(name);
}

void EditorManager::copyPathToClipboard()
{
	int item = d->getClickedTab();
	if (item == -1) item = d->tabWidget()->currentIndex();
	const AbstractEditor * ed = editor(item);
	if (ed)
	{
		const QString name = QDir::toNativeSeparators(ed->lastFileName());
		qApp->clipboard()->setText(name);
	}
}

void EditorManager::serializeEditors(bool content)
{
	if (XinxSession::SessionManager::self()->isApplicationStopping()) return;

	XinxSession::SessionManager::self()->currentSession()->clearSerializedEditors();
	for (int i = 0; i < editorsCount(); i++)
	{
		XinxSession::SessionEditor * session_editor = new XinxSession::SessionEditor(XinxSession::SessionManager::self()->currentSession());
		editor(i)->serialize(session_editor, content);
		XinxSession::SessionManager::self()->currentSession()->addSerializedEditor(session_editor);
	}
}

void EditorManager::deserializeEditors(QList<XinxSession::SessionEditor*> session_editors)
{
	d->_tab_widget->setUpdatesEnabled(false);

	foreach(XinxSession::SessionEditor* editor, session_editors)
	{
		d->addTab(EditorFactory::self()->createEditor(editor));
	}

	if (editorsCount() > 0) setCurrentEditor(0);
	d->_tab_widget->setUpdatesEnabled(true);

	d->updateRecentFiles();
	d->updateActions();
}

QWidget * EditorManager::editorsWidget() const
{
	return d->tabWidget();
}

QAction * EditorManager::newAction() const
{
	return d->_new_action;
}

QAction * EditorManager::openAction() const
{
	return d->_recent_action;
}

QAction* EditorManager::refreshAction() const
{
	return d->_refresh_action;
}

QAction* EditorManager::saveAction() const
{
	return d->_save_action;
}

QAction* EditorManager::saveAsAction() const
{
	return d->_save_as_action;
}

QAction* EditorManager::saveAllAction() const
{
	return d->_save_all_action;
}

QAction* EditorManager::printAction() const
{
	return d->_print_action;
}

QAction* EditorManager::closeAction() const
{
	return d->_close_action;
}

QAction* EditorManager::closeAllAction() const
{
	return d->_close_all_action;
}

QAction* EditorManager::copyFileNameToClipboardAction() const
{
	return d->_copy_filename_action;
}

QAction* EditorManager::copyPathToClipboardAction() const
{
	return d->_copy_path_action;
}

QAction* EditorManager::nextEditorAction() const
{
	return d->_next_tab_action;
}

QAction* EditorManager::previousEditorAction() const
{
	return d->_previous_tab_action;
}
