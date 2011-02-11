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

// Xinx header
#include "commitmessagedialogimpl.h"
#include <project/xinxprojectproject.h>
#include <core/exceptions.h>

// Qt header
#include <QMessageBox>
#include <QDir>

/* PrivateCommitMessageDialogImpl */

class PrivateCommitMessageDialogImpl
{
public:
	PrivateCommitMessageDialogImpl(CommitMessageDialogImpl * parent);
	~PrivateCommitMessageDialogImpl();

	XinxProject::ProjectPtr m_project;
	RCS::FilesOperation m_files;
private:
	CommitMessageDialogImpl * m_parent;
};

PrivateCommitMessageDialogImpl::PrivateCommitMessageDialogImpl(CommitMessageDialogImpl * parent)
{
	m_parent = parent;
}

PrivateCommitMessageDialogImpl::~PrivateCommitMessageDialogImpl()
{

}

/* CommitMessageDialogImpl */

/*!
 * \ingroup RCS
 * \class CommitMessageDialogImpl
 * \since 0.9.0.0
 *
 * \brief Show file to commit and let the user edit the message
 *
 * This This dialog is used to choose file to be commited and message to be assigned
 * at the operations. The dialog propose to the user the ability to add or remove some
 * files.
 *
 * \image html commitmessagedialogimpl1.png
 */

/*!
 * \brief Constructor of the commit dialog.
 *
 * The dialog as a windows flag who said the flag is not resizable.
 * \param parent Parent of the dialog
 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
 */
CommitMessageDialogImpl::CommitMessageDialogImpl(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);
	d = new PrivateCommitMessageDialogImpl(this);
}

/*!
 * \brief Destructor of commit dialog.
 */
CommitMessageDialogImpl::~CommitMessageDialogImpl()
{
	delete d;
}

/*!
 * \brief Set the project to \e project
 */
void CommitMessageDialogImpl::setXinxProject(XinxProject::ProjectPtr project)
{
	d->m_project = project;
}

/*!
 * \brief Get the project.
 * \return the current project
 */
XinxProject::ProjectPtr CommitMessageDialogImpl::project() const
{
	return d->m_project;
}

/*!
 * \brief Defines the default message to be commited.
 *
 * The message is show in the dialog. The user can change the message.
 * \param message Message to be stocked when commit is called.
 */
void CommitMessageDialogImpl::setMessages(const QString & message)
{
	m_textEditMessages->setText(message);
}

/*!
 * \brief Return the message define by the user.
 * \return The message to be sent to CVS.
 */
QString CommitMessageDialogImpl::messages()
{
	return m_textEditMessages->toPlainText();
}

/*!
 * \brief Set the list of file to be commited.
 *
 * If the file as mode RCS::RemoveAndCommit or AddAndCommit the file is not
 * checked by default in the list. If the mode is RCS::Commit the file is checked.
 * \param files List of files who ca be add/remove and commited
 */
void CommitMessageDialogImpl::setFilesOperation(RCS::FilesOperation files)
{
	Q_ASSERT(d->m_project != NULL);

	m_fileListWidget->clear();
	d->m_files = files;
	foreach(const RCS::FileOperation & file, d->m_files)
	{
		QString fileName = QDir(d->m_project->projectPath()).relativeFilePath(file.filename);
		switch (file.operation)
		{
		case RCS::Commit:
			fileName += tr(" ( Commit )");
			break;
		case RCS::AddAndCommit:
			fileName += tr(" ( Add and Commit )");
			break;
		case RCS::RemoveAndCommit:
			fileName += tr(" ( Remove and Commit )");
			break;
		default:
			;
		}
		QListWidgetItem * item = new QListWidgetItem(fileName, m_fileListWidget);
		item->setCheckState(Qt::Unchecked);
		if (file.operation == RCS::Commit)
		{
			item->setCheckState(Qt::Checked);
		}
	}
}

/*!
 * \brief Return the file list.
 *
 * If file is unchecked then the mode RCS::Nothing is set.
 * \return Return the list of files the user whant to commit.
 */
RCS::FilesOperation CommitMessageDialogImpl::filesOperation()
{
	for (int i = 0 ; i < m_fileListWidget->count(); i++)
	{
		RCS::FileOperation op(d->m_files.at(i));
		if (m_fileListWidget->item(i)->checkState() == Qt::Unchecked)
		{
			op.operation = RCS::Nothing;
			d->m_files.replace(i , op);
		}
	}
	return d->m_files;
}

/*!
 * \brief Show the dialog and wait its validation or cancellation.
 *
 * Redefine the exec function. When this function is called if the function
 * setFilesOperation aren't called (or if there is no file to commit)
 * then a message is popup (There is nothing to do).
 */
int CommitMessageDialogImpl::exec()
{
	if (d->m_files.size() == 0)
	{
		QMessageBox::information(this, tr("Commit"), tr("Nothing to do."));
		return QDialog::Rejected;
	}
	return QDialog::exec();
}
