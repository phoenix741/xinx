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

	m_xmlPresentationWidget = new Ui::XmlPresentationWidget();
	m_xmlPresentationWidget->setupUi(m_parent);

	m_xmlPresentationWidget->m_presentationProgressBar->hide();

	m_timerTextChanged.setSingleShot(true);
	m_timerTextChanged.setInterval(1000);

	m_xmlPresentationWidget->m_filterComboBox->setCurrentIndex(SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree ? 0 : 1);

	initXmlPresentationCombo();

	connect(XinxProject::Manager::self(), SIGNAL(changed()), this, SLOT(projectChanged()));
	connect(m_xmlPresentationWidget->m_refreshToolButton, SIGNAL(clicked()), this, SLOT(initXmlPresentationCombo()));
	connect(m_xmlPresentationWidget->m_evaluateToolButton, SIGNAL(clicked()), this, SLOT(evaluate()));

	connect(m_xmlPresentationWidget->m_presentationComboBox, SIGNAL(activated(int)), this, SLOT(presentationActivated(int)));
	connect(m_xmlPresentationWidget->m_filtreLineEdit, SIGNAL(textChanged(QString)), this, SLOT(filterTextChanged(QString)));
	connect(this, SIGNAL(finished()), this, SLOT(threadTerminated()));

	connect(m_xmlPresentationWidget->m_presentationTreeView, SIGNAL(expanded(QModelIndex)), this, SLOT(adaptColumns()));

	connect(m_xmlPresentationWidget->m_filterComboBox, SIGNAL(activated(int)), this, SLOT(updateXinxConf(int)));
	connect(&m_timerTextChanged, SIGNAL(timeout()), this, SLOT(filterTextChangedTimer()));
	connect(SelfWebPluginSettings::self(), SIGNAL(changed()), this, SLOT(filterTextChangedTimer()));

	connect(dynamic_cast<QObject*>(EditorManager::self()), SIGNAL(currentChanged(int)), this, SLOT(editorChanged()));
}


XmlPresentationDockThread::~XmlPresentationDockThread()
{
	presentationActivated(0);
	delete m_xmlPresentationWidget;
}

void XmlPresentationDockThread::evaluate()
{
	XQueryDialogImpl dlg(m_parent);
	dlg.setFileName(m_openingFile);

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

void XmlPresentationDockThread::projectChanged()
{
	m_dataStreamLocation.clear();
	foreach (XinxProject::Project * project, XinxProject::Manager::self()->projects())
	{
		const QString rel  = project->readProperty("dataStreamLocation").toString();
		m_dataStreamLocation << QDir(project->projectPath()).absoluteFilePath(rel);
	}
	m_dataStreamLocation.sort();
	m_dataStreamLocation.removeDuplicates();
	
	initXmlPresentationCombo();
}

void XmlPresentationDockThread::initXmlPresentationCombo()
{
	int index;
	QString filename = m_openingFile;

	m_xmlPresentationWidget->m_presentationComboBox->blockSignals(true);
	m_xmlPresentationWidget->m_presentationComboBox->clear();

	m_xmlPresentationWidget->m_presentationComboBox->addItem(tr("<No presentation file>"));
	m_xmlPresentationWidget->m_presentationComboBox->addItem(tr("<Choose an XML file ...>"));

	QAbstractItemModel * model = m_xmlPresentationWidget->m_presentationComboBox->model();
	foreach (const QString & dataStreamLocation, m_dataStreamLocation)
	{
		m_xmlPresentationWidget->m_presentationComboBox->addItem(dataStreamLocation);
		QModelIndex index = model->index(m_xmlPresentationWidget->m_presentationComboBox->count() - 1, 0);
		model->setData(index, QVariant(Qt::AlignCenter), Qt::TextAlignmentRole);
		
		QDir dataStreamDir(dataStreamLocation);
		if (dataStreamDir.exists())
		{
			QStringList files = dataStreamDir.entryList(QStringList() << "*.xml", QDir::Files | QDir::Readable);
			foreach(const QString & file, files)
			{
				m_xmlPresentationWidget->m_presentationComboBox->addItem(file, dataStreamLocation);
			}
		}
	}

	if (filename.isEmpty())
	{
		m_xmlPresentationWidget->m_presentationComboBox->blockSignals(false);
		presentationActivated(0);
		return;
	}

	if ((index = m_xmlPresentationWidget->m_presentationComboBox->findText(QFileInfo(filename).fileName())) >= 0)
	{
		m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex(index);
	}
	else if (QFileInfo(filename).exists())
	{
		m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex(1);
	}
	else
		presentationActivated(0);
	
	m_xmlPresentationWidget->m_presentationComboBox->blockSignals(false);
}

void XmlPresentationDockThread::presentationActivated(int index)
{
	if (index == 0)
	{
		m_xmlPresentationWidget->m_presentationTreeView->setModel(NULL);
		delete m_watcher;
		delete m_sortFilterModel;
		delete m_model;
		m_openingFile = QString();
		m_xmlPresentationWidget->m_evaluateToolButton->setEnabled(false);
		emit m_parent->filenameChanged(QString());
		if (EditorManager::self() && EditorManager::self()->currentEditor())
			EditorManager::self()->currentEditor()->setProperty("XmlPresentationDockThread_filename", QString());
	}
	else if (index == 1)
	{
		QString location;
		if (m_dataStreamLocation.size())
		{
			location = m_dataStreamLocation.at(0);
		}
		
		// Open a file
		QString name = QFileDialog::getOpenFileName(m_parent, tr("Open a presentation file"), location, tr("Presentation XML File (*.xml)"));
		if (name.isEmpty())
		{
			m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex(0);
		}
		else
		{
			setComboToolTip(name);
			open(name);
		}
	}
	else
	{
		// Open the file
		const QString location = m_xmlPresentationWidget->m_presentationComboBox->itemData(index).toString();
		const QString file     = m_xmlPresentationWidget->m_presentationComboBox->itemText(index);
		const QString name = QDir(location).absoluteFilePath(file);
		setComboToolTip(name);
		open(name);
	}
}

void XmlPresentationDockThread::setComboToolTip(const QString & filename)
{
	QFileInfo file(filename);
	char unite = ' ';
	double filesize = file.size();
	while (filesize > 1024)
	{
		filesize /= 1024;
		switch (unite)
		{
		case ' ' :
			unite = 'k';
			break;
		case 'k' :
			unite = 'm';
			break;
		case 'm' :
			unite = 'g';
			break;
		case 'g' :
			unite = 't';
			break;
		default:
			break;
		}
	}

	m_xmlPresentationWidget->m_presentationComboBox->setToolTip(
	    tr("File name : %1\n"
	       "Size : %2 %3b\n"
	       "File date : %4")    .arg(file.canonicalFilePath())
	    .arg(filesize, 0, 'f', 2).arg(unite)
	    .arg(file.lastModified().toString())
	);
}

void XmlPresentationDockThread::open(const QString& filename)
{
	Q_ASSERT(! isRunning());

	if (m_watcher) m_watcher->desactivate();

	m_xmlPresentationWidget->m_presentationProgressBar->show();
	m_xmlPresentationWidget->m_presentationComboBox->setEnabled(false);
	m_xmlPresentationWidget->m_filtreLineEdit->setEnabled(false);
	m_xmlPresentationWidget->m_filterComboBox->setEnabled(false);
	m_xmlPresentationWidget->m_refreshToolButton->setEnabled(false);

	if (m_sortFilterModel && m_xmlPresentationWidget)
		m_currentXpath = m_sortFilterModel->data(m_xmlPresentationWidget->m_presentationTreeView->currentIndex(), Qt::UserRole).toString();

	m_xmlPresentationWidget->m_presentationTreeView->setModel(NULL);

	m_openingFile = filename;

	m_filteredText = m_xmlPresentationWidget->m_filtreLineEdit->text();
	m_filteredElement = SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree;
	m_filterHidePath = SelfWebPluginSettings::self()->config().xmlPres.hidePath;

	delete m_sortFilterModel;
	delete m_model;
	delete m_watcher;

	if (!QFileInfo(filename).exists())
	{
		m_openingFile = QString();
		m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex(0);
		m_xmlPresentationWidget->m_presentationProgressBar->hide();
		m_xmlPresentationWidget->m_presentationComboBox->setEnabled(true);
		m_xmlPresentationWidget->m_filterComboBox->setEnabled(true);
		m_xmlPresentationWidget->m_refreshToolButton->setEnabled(true);
		m_xmlPresentationWidget->m_filtreLineEdit->setEnabled(true);
		m_xmlPresentationWidget->m_filtreLineEdit->setFocus();
		return;
	}

	m_threadAct = THREAD_OPENING;
	start(QThread::IdlePriority);
}

void XmlPresentationDockThread::open()
{
	if (isRunning()) return;
	open(m_openingFile);
}

void XmlPresentationDockThread::threadrun()
{
	delete m_sortFilterModel;
	if (m_threadAct == THREAD_OPENING)
	{
		QFile presentation(m_openingFile);
		QDomDocument document;
		if (presentation.open(QIODevice::ReadOnly | QIODevice::Text) && document.setContent(&presentation, false))
		{
			m_model = new XmlPresentationModel(document);
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
	if (m_threadAct == THREAD_OPENING)
	{
		m_xmlPresentationWidget->m_evaluateToolButton->setEnabled(true);
		m_watcher = new FileWatcher(m_openingFile);
		connect(m_watcher, SIGNAL(fileChanged()), this, SLOT(open()));

		if (EditorManager::self() && EditorManager::self()->currentEditor())
			EditorManager::self()->currentEditor()->setProperty("XmlPresentationDockThread_filename", m_openingFile);
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

	m_xmlPresentationWidget->m_presentationProgressBar->hide();
	m_xmlPresentationWidget->m_presentationComboBox->setEnabled(true);
	m_xmlPresentationWidget->m_filterComboBox->setEnabled(true);
	m_xmlPresentationWidget->m_refreshToolButton->setEnabled(true);
	m_xmlPresentationWidget->m_filtreLineEdit->setText(m_filteredText);   // Au cas o� des caract�res n'ont pas �t� pris en compte
	m_xmlPresentationWidget->m_filtreLineEdit->setEnabled(true);
	m_xmlPresentationWidget->m_filtreLineEdit->setFocus();

	emit m_parent->filenameChanged(m_openingFile);
}

void XmlPresentationDockThread::filterTextChanged(const QString & text)
{
	Q_UNUSED(text);
	if (m_sortFilterModel)
		m_timerTextChanged.start();
}

void XmlPresentationDockThread::filterTextChangedTimer()
{
	Q_ASSERT(! isRunning());
	m_timerTextChanged.stop();

	if (m_sortFilterModel)
	{
		// TODO: Delete this line in 4.4
		m_xmlPresentationWidget->m_filtreLineEdit->clearFocus();
		m_xmlPresentationWidget->m_filtreLineEdit->setEnabled(false);
		m_xmlPresentationWidget->m_presentationComboBox->setEnabled(false);
		m_xmlPresentationWidget->m_filterComboBox->setEnabled(false);
		m_xmlPresentationWidget->m_refreshToolButton->setEnabled(false);

		if (SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree != (m_xmlPresentationWidget->m_filterComboBox->currentIndex() == 0))
		{
			m_xmlPresentationWidget->m_filterComboBox->setCurrentIndex(SelfWebPluginSettings::self()->config().xmlPres.showFilteredSubTree ? 0 : 1);
		}

		m_currentXpath = m_sortFilterModel->data(m_xmlPresentationWidget->m_presentationTreeView->currentIndex(), Qt::UserRole).toString();

		m_xmlPresentationWidget->m_presentationTreeView->setModel(NULL);
		m_xmlPresentationWidget->m_presentationProgressBar->show();

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
		int index;
		if ((index = m_xmlPresentationWidget->m_presentationComboBox->findText(QFileInfo(filename).fileName())) >= 0)
		{
			m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex(index);
			presentationActivated(index);
		}
		else if (QFileInfo(filename).exists())
		{
			m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex(1);
			setComboToolTip(filename);
			open(filename);
		}
		else
		{
			m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex(0);
			presentationActivated(0);
		}
	}
	else
	{
		m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex(0);
		presentationActivated(0);
	}
}
