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
#include "derivationdialogimpl.h"
#include "projectproperty/generixproject.h"
#include <directoryedit.h>

// Qt header
#include <QDir>

/* DerivationDialogImpl */

DerivationDialogImpl::DerivationDialogImpl(XinxProject::ProjectPtr project, QWidget* parent, Qt::WFlags f) : QDialog(parent, f), _project(project)
{
	setupUi(this);

	m_directoryEdit->setDirectory(false);
	m_directoryEdit->lineEdit()->setFileMustExist(false);

	connect(m_derivationPathList, SIGNAL(currentTextChanged(QString)), this, SLOT(changePath()));
	connect(m_prefixList, SIGNAL(currentTextChanged(QString)), this, SLOT(changePath()));
	connect(m_derivationChk, SIGNAL(toggled(bool)), this, SLOT(changePath()));
}

DerivationDialogImpl::~DerivationDialogImpl()
{
}

void DerivationDialogImpl::load(const QString & filename, const QString & filter)
{
	QSharedPointer<GenerixProject> gnxProject = _project.staticCast<GenerixProject>();
	if (! gnxProject) return;

	m_directoryEdit->lineEdit()->setFilter(filter);

	if (QDir::isRelativePath(filename))
		m_filename = QDir::cleanPath(QDir(gnxProject->projectPath()).absoluteFilePath(filename));
	else
		m_filename = QDir::cleanPath(filename);

	QList<GenerixProject::DerivationPath> derivationPaths = gnxProject->derivationsPath();
	foreach(GenerixProject::DerivationPath dp, derivationPaths)
	{
		QString path = QDir::cleanPath(QDir(gnxProject->projectPath()).absoluteFilePath(dp.path));

		if (dp.visible)
		{
			QListWidgetItem * item = new QListWidgetItem(m_derivationPathList);
			item->setText(dp.name);
			item->setData(Qt::UserRole, dp.path);
		}

		if (m_filename.contains(path))
		{
			m_relativeFilename = QDir(path).relativeFilePath(QFileInfo(m_filename).absolutePath());
		}
	}
	m_derivationPathList->setCurrentRow(0);

	int prefixIndex = 0, index = 1;
	QListWidgetItem * noprefix = new QListWidgetItem(tr("<No prefix>"), m_prefixList);
	noprefix->setSelected(true);
	noprefix->setData(Qt::UserRole, true);

	QStringList prefixes = gnxProject->prefixes();
	foreach(const QString & prefix, prefixes)
	{
		QListWidgetItem * item = new QListWidgetItem(prefix, m_prefixList);
		if (! gnxProject->defaultPrefix().isEmpty() && gnxProject->defaultPrefix() == prefix)
		{
			item->setData(Qt::UserRole, false);
			prefixIndex = index;
		}
		index++;
	}
	m_prefixList->setCurrentRow(prefixIndex);
	m_prefixList->setVisible(!prefixes.isEmpty());
	_prefixLabel->setVisible(!prefixes.isEmpty());

	changePath();
}

QString DerivationDialogImpl::getNewPath() const
{
	return m_directoryEdit->lineEdit()->text();
}

bool DerivationDialogImpl::isDerivableFile(XinxProject::ProjectPtr p, const QString& filename)
{
	QSharedPointer<GenerixProject> project = p.staticCast<GenerixProject>();
	if (! project) return false;

	QString absoluteFilename;
	if (QDir::isRelativePath(filename))
		absoluteFilename = QDir::cleanPath(QDir(project->projectPath()).absoluteFilePath(filename));
	else
		absoluteFilename = QDir::cleanPath(filename);

	QList<GenerixProject::DerivationPath> derivationPaths = project->derivationsPath();
	foreach(GenerixProject::DerivationPath dp, derivationPaths)
	{
		QString path = QDir::cleanPath(QDir(project->projectPath()).absoluteFilePath(dp.path));
		if (absoluteFilename.contains(path))
		{
			return dp.derivation;
		}
	}
	return false;
}

void DerivationDialogImpl::changePath()
{
	QSharedPointer<GenerixProject> gnxProject = _project.staticCast<GenerixProject>();
	if (! gnxProject) return;

	if (m_derivationChk->isChecked() && m_derivationPathList->currentItem() && m_prefixList->currentItem())
	{
		QListWidgetItem * derivationPathItem = m_derivationPathList->currentItem();
		QListWidgetItem * prefixItem         = m_prefixList->currentItem();

		const QString projectPath    = gnxProject->projectPath();
		const QString derivationPath = derivationPathItem->data(Qt::UserRole).toString();
		const QString prefix         = prefixItem->text();

		QString pathname             = QDir(projectPath).absoluteFilePath(derivationPath) + (m_relativeFilename.isEmpty() ? "/" : ("/" + m_relativeFilename + "/"));
		QString filename             = prefix + "_" + QFileInfo(m_filename).fileName();
		if (prefixItem->data(Qt::UserRole).toBool())
			filename                 = QFileInfo(m_filename).fileName();

		if (gnxProject->createMissingDirectory())
			QDir::current().mkpath(pathname);
		m_directoryEdit->lineEdit()->setText(pathname + filename);
		m_directoryEdit->lineEdit()->setSelection(pathname.length(), QFileInfo(filename).baseName().length());
	}
	else
	{
		m_directoryEdit->lineEdit()->setText(m_filename);
	}
}

