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

#include "newfilewizardimpl.h"
#include <project/xinxprojectmanager.h>
#include <project/xinxprojectproject.h>

NewFileWizardImpl::NewFileWizardImpl(QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags), _project(0), _interface(0)
{
	setupUi(this);

	_new_file_name->lineEdit()->setFileMustExist (false);
	_new_file_name->setDirectory (false);
	_directory_base->setVisible (false);
	_directory_base_label->setVisible (false);
	_file_type_selector->setVisible (false);
	_file_type_selector->setShowFileName (false);

	connect(_new_file_name->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));

	setPath(QString());
	setFileName (QString());
}

NewFileWizardImpl::~NewFileWizardImpl()
{

}

void NewFileWizardImpl::slotTextChanged(const QString & text)
{
	_file_type_selector->setFileName (text);
	_file_type_selector->setVisible ((!_interface) && (_file_type_selector->count () > 1));
}

void NewFileWizardImpl::setFileName(const QString& value)
{
	QString filename;
	if (!value.isEmpty ())
	{
		filename = value;
	}
	else
	{
		if (path().isEmpty())
		{
			XinxProject::ProjectPtr selectedProject = XinxProject::Manager::self()->selectedProject().toStrongRef();
			if (selectedProject)
			{
				filename = QDir(selectedProject->projectPath()).absoluteFilePath(tr("noname.txt"));
			}
			else
			{
				filename = QDir(qApp->applicationDirPath()).absoluteFilePath(tr("noname.txt"));
			}
		}
		else
		{
			filename = QDir(path()).absoluteFilePath(tr("noname.txt"));
		}
	}

	_new_file_name->lineEdit()->setText(QDir::toNativeSeparators (filename));
	int position = filename.lastIndexOf ("/");
	int length   = filename.size () - position;
	if (position != -1)
	{
		_new_file_name->lineEdit()->setSelection(position + 1, length - 1);
	}
	else
	{
		_new_file_name->lineEdit()->end(false);
	}
}

QString NewFileWizardImpl::filename() const
{
	return QDir::fromNativeSeparators (_new_file_name->lineEdit ()->text ());
}

void NewFileWizardImpl::setPath(const QString& value)
{
	const QString path = value.isEmpty() ? QString() : QDir::toNativeSeparators(value + (value.endsWith("/") ? "" : "/"));
	_directory_base->setText(path);
	_directory_base->setVisible(! path.isEmpty());
	_directory_base_label->setVisible(! path.isEmpty());
	_new_file_name->lineEdit()->setPrefix(path);
}

QString NewFileWizardImpl::path() const
{
	return QDir::fromNativeSeparators (_directory_base->text ());
}

void NewFileWizardImpl::setSelectedType(IFileTypePlugin * plugin)
{
	_interface = plugin;
}

IFileTypePlugin * NewFileWizardImpl::selectedType() const
{
	return _interface ? _interface : _file_type_selector->selectedType();
}

void NewFileWizardImpl::setProject(XinxProject::ProjectPtr project)
{
	_project = project;
}

XinxProject::ProjectPtr NewFileWizardImpl::project() const
{
	return _project ? _project : XinxProject::Manager::self()->projectOfFile(filename());
}

void NewFileWizardImpl::done ( int result )
{
	if (result == QDialog::Accepted)
	{
		QFile file(filename());
		if (file.exists())
		{
			file.resize(0);
		}
		else
		{
			file.open(QIODevice::WriteOnly);
			// TODO: Write the content of the file (Licence + Template)
		}
	}
	QWizard::done(result);
}

void NewFileWizardImpl::initializePage(int id)
{
	QWizard::initializePage(id);
}
