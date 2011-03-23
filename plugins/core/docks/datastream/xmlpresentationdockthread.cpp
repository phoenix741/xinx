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
#include "xmlpresentationdockthread.h"
#include <project/xinxprojectmanager.h>
#include <config/selfwebpluginsettings.h>
#include "xquerydialogimpl.h"
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>

// Qt header
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>

/* XmlPresentationDockThread */

XmlPresentationDockThread::XmlPresentationDockThread(XmlPresentationDockWidget * parent) : m_model(0), m_sortFilterModel(0), m_watcher(0), m_parent(parent)
{
	qRegisterMetaType<QModelIndex>("QModelIndex");

	// The timer for written text, launch manually by keypress after one second.
	m_timerTextChanged.setSingleShot(true);
	m_timerTextChanged.setInterval(1000);

	// Init the widget
	m_xmlPresentationWidget = new Ui::XmlPresentationWidget();
	m_xmlPresentationWidget->setupUi(m_parent);
	m_xmlPresentationWidget->m_presentationProgressBar->hide();
	m_xmlPresentationWidget->m_filterComboBox->setCurrentIndex(SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree ? 0 : 1);
	connect(m_xmlPresentationWidget->m_filtreLineEdit, SIGNAL(textChanged(QString)), this, SLOT(filterTextChanged()));
	connect(m_xmlPresentationWidget->m_presentationTreeView, SIGNAL(expanded(QModelIndex)), this, SLOT(adaptColumns()));
	connect(m_xmlPresentationWidget->m_filterComboBox, SIGNAL(activated(int)), this, SLOT(updateXinxConf(int)));

	connect(&m_timerTextChanged, SIGNAL(timeout()), this, SLOT(filterTextChangedTimer()));
	connect(SelfWebPluginSettings::self(), SIGNAL(changed()), this, SLOT(filterTextChanged()));

	connect(this, SIGNAL(finished()), this, SLOT(threadTerminated()));
	connect(dynamic_cast<QObject*>(EditorManager::self()), SIGNAL(currentChanged(int)), this, SLOT(editorChanged()));
}


XmlPresentationDockThread::~XmlPresentationDockThread()
{
	terminate();
	wait();

	close();

	delete m_xmlPresentationWidget;
}

void XmlPresentationDockThread::evaluate()
{
	XQueryDialogImpl dlg(m_parent);
	dlg.setFileName(m_filename);

	QString m_currentXpath;
	if (m_sortFilterModel)
		m_currentXpath = m_sortFilterModel->data(m_xmlPresentationWidget->m_presentationTreeView->currentIndex(), Qt::UserRole).toString();
	dlg.setCurrentXPath(m_currentXpath);

	dlg.exec();
}

void XmlPresentationDockThread::adaptColumns()
{
	m_xmlPresentationWidget->m_presentationTreeView->resizeColumnToContents(0);
}


void XmlPresentationDockThread::close()
{
	Q_ASSERT(! isRunning());

	// Cleaning
	m_xmlPresentationWidget->m_presentationTreeView->setModel(NULL);
	delete m_watcher;
	delete m_sortFilterModel;
	delete m_model;

	// Clear filename
	m_filename = QString();
	emit m_parent->filenameChanged(QString());

	// Change the current property of the editor
	if (EditorManager::self() && EditorManager::self()->currentEditor())
		EditorManager::self()->currentEditor()->setProperty("XmlPresentationDockThread_filename", QString());
}

void XmlPresentationDockThread::open()
{
	// reopen current filename
	if (! m_filename.isEmpty())
	{
		open(m_filename);
	}
}

void XmlPresentationDockThread::disabledInterface(bool value)
{
	m_xmlPresentationWidget->m_presentationProgressBar->setVisible(value);
	m_xmlPresentationWidget->m_filtreLineEdit->setEnabled(!value);
	m_xmlPresentationWidget->m_filterComboBox->setEnabled(!value);
	if (!value)
	{
		m_xmlPresentationWidget->m_filtreLineEdit->setFocus();
	}
}

void XmlPresentationDockThread::open(const QString& filename)
{
	Q_ASSERT_X(! isRunning(), "XmlPresentationDockThread::open", "The thread mustn't be running.");

	if (m_watcher) m_watcher->desactivate();

	disabledInterface(true);

	if (m_sortFilterModel && m_xmlPresentationWidget)
		m_currentXpath = m_sortFilterModel->data(m_xmlPresentationWidget->m_presentationTreeView->currentIndex(), Qt::UserRole).toString();

	close();

	if (!QFileInfo(filename).exists())
	{
		disabledInterface(false);
		return;
	}

	m_filename = filename;
	m_filteredText = m_xmlPresentationWidget->m_filtreLineEdit->text();
	m_filteredElement = SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree;
	m_filterHidePath = SelfWebPluginSettings::self()->config().xmlPres.hidePath;

	m_threadAct = THREAD_OPENING;
	start(QThread::IdlePriority);
}

void XmlPresentationDockThread::threadrun()
{
	delete m_sortFilterModel;
	if (m_threadAct == THREAD_OPENING)
	{
		QString errorMsg;
		QFile presentation(m_filename);
		QDomDocument document;
		if (presentation.open(QIODevice::ReadOnly | QIODevice::Text) && document.setContent(&presentation, false, &errorMsg))
		{
			m_model = new XmlPresentationModel(document);
		}
		else
		{
			qCritical() << tr("Can't read file %1 because of error \"%2\".").arg(m_filename).arg(errorMsg);
		}
	}
	if (m_model)
	{
		m_sortFilterModel = new RecursiveSortFilterProxyModel(m_model);
		m_sortFilterModel->setSourceModel(m_model);
		m_sortFilterModel->setHidePath(m_filterHidePath);
		m_sortFilterModel->setShowAllChild(m_filteredElement);
		m_sortFilterModel->setFilterRegExp(m_filteredText);
		m_model->moveToThread(qApp->thread());
	}
}

void XmlPresentationDockThread::threadTerminated()
{
	if (! m_model) // An error occured
	{
		disabledInterface(false);
		return;
	}

	Q_ASSERT_X(m_sortFilterModel, "XmlPresentationDockThread::threadTerminated", "Sort filter model is empty. Is the objet destroyed ?");

	if (m_threadAct == THREAD_OPENING)
	{
		m_watcher = new FileWatcher(m_filename);
		connect(m_watcher, SIGNAL(fileChanged()), this, SLOT(open()));

		if (EditorManager::self() && EditorManager::self()->currentEditor())
			EditorManager::self()->currentEditor()->setProperty("XmlPresentationDockThread_filename", m_filename);
	}
	else if (m_threadAct == THREAD_FILTERED)
	{
	}

	m_xmlPresentationWidget->m_presentationTreeView->setModel(m_sortFilterModel);

	/* Filter part */
	QModelIndex expandIndex = QModelIndex();
	if (!m_filteredText.isEmpty())
	{
		while (m_sortFilterModel->rowCount(expandIndex) > 0)
		{
			m_xmlPresentationWidget->m_presentationTreeView->expand(expandIndex);
			expandIndex = m_sortFilterModel->index(0, 0, expandIndex);
		}
	}
	else
	{
		QStringList xpath = m_currentXpath.isEmpty() ? SelfWebPluginSettings::self()->config().xmlPres.autoExpandedPath.split('/') : m_currentXpath.split('/');
		foreach(const QString & path, xpath)
		{
			m_xmlPresentationWidget->m_presentationTreeView->expand(expandIndex);
			if (! path.isEmpty() && m_sortFilterModel->rowCount(expandIndex))
			{
				QModelIndexList matchedIndex = m_sortFilterModel->match(m_sortFilterModel->index(0, 0, expandIndex), XmlPresentationModel::NamedViewRole, path, 1, Qt::MatchExactly);
				if (matchedIndex.count() > 0)
					expandIndex = matchedIndex.at(0);
				else
				{
					matchedIndex = m_sortFilterModel->match(m_sortFilterModel->index(0, 0, expandIndex), Qt::DisplayRole, path, 1, Qt::MatchExactly);
					if (matchedIndex.count() > 0)
						expandIndex = matchedIndex.at(0);
				}
			}
		}
	}

	m_xmlPresentationWidget->m_presentationTreeView->expand(expandIndex);
	m_xmlPresentationWidget->m_presentationTreeView->setCurrentIndex(expandIndex);
	/* End of filter part */

	m_xmlPresentationWidget->m_filtreLineEdit->setText(m_filteredText);   // In case of some key press char have not been registered
	disabledInterface(false);

	emit m_parent->filenameChanged(m_filename);
}

void XmlPresentationDockThread::filterTextChanged()
{
	if (m_sortFilterModel && ! isRunning())
	{
		m_timerTextChanged.start();
	}
}

void XmlPresentationDockThread::filterTextChangedTimer()
{
	Q_ASSERT_X(! isRunning(), "XmlPresentationDockThread::filterTextChangedTimer", "Thread mustn't be running");

	m_timerTextChanged.stop();

	if (m_sortFilterModel)
	{
		// TODO: Delete this line in 4.4
		m_xmlPresentationWidget->m_filtreLineEdit->clearFocus();
		disabledInterface(true);

		if (SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree != (m_xmlPresentationWidget->m_filterComboBox->currentIndex() == 0))
		{
			m_xmlPresentationWidget->m_filterComboBox->setCurrentIndex(SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree ? 0 : 1);
		}

		m_currentXpath = m_sortFilterModel->data(m_xmlPresentationWidget->m_presentationTreeView->currentIndex(), Qt::UserRole).toString();

		m_xmlPresentationWidget->m_presentationTreeView->setModel(NULL);

		m_filteredText = m_xmlPresentationWidget->m_filtreLineEdit->text();
		m_filteredElement = SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree;
		m_filterHidePath = SelfWebPluginSettings::self()->config().xmlPres.hidePath;

		m_model->moveToThread(this);

		m_threadAct = THREAD_FILTERED;
		start(QThread::IdlePriority);
	}
}

void XmlPresentationDockThread::updateXinxConf(int value)
{
	bool filteredElement = value == 0;
	if (SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree != filteredElement)
	{
		SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree = filteredElement;
		SelfWebPluginSettings::self()->save();
	}
}

void XmlPresentationDockThread::editorChanged()
{
	QString filename;
	if (EditorManager::self() && EditorManager::self()->currentEditor() && !(filename = EditorManager::self()->currentEditor()->property("XmlPresentationDockThread_filename").toString()).isEmpty())
	{
		if (QFileInfo(filename).exists())
		{
			open(filename);
			return;
		}
	}
	close();
}
