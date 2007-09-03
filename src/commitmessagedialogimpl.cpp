/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "commitmessagedialogimpl.h"
#include "globals.h"
#include "xslproject.h"

// Qt header
#include <QMessageBox>
#include <QDir>

/* PrivateCommitMessageDialogImpl */

class PrivateCommitMessageDialogImpl {
public:
	PrivateCommitMessageDialogImpl( CommitMessageDialogImpl * parent );
	~PrivateCommitMessageDialogImpl();
	
	RCS::FilesOperation m_files;
private:
	CommitMessageDialogImpl * m_parent;
};

PrivateCommitMessageDialogImpl::PrivateCommitMessageDialogImpl( CommitMessageDialogImpl * parent ) {
	m_parent = parent;
}

PrivateCommitMessageDialogImpl::~PrivateCommitMessageDialogImpl() {
	
}

/* CommitMessageDialogImpl */

/*!
 * \class CommitMessageDialogImpl
 * 
 * Implementation of the commit dialog. This dialog is used to choose file to be commited
 * and message to be assigned at the operations.
 */

/*!
 * Constructor of the commit dialog. The dialog as a windows flag who said the flag is not
 * resizable.
 * \param parent Parent of the dialog
 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
 */
CommitMessageDialogImpl::CommitMessageDialogImpl( QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {
	setupUi(this);
	d = new PrivateCommitMessageDialogImpl( this );
}

/*!
 * Destructor of commit dialog.
 */
CommitMessageDialogImpl::~CommitMessageDialogImpl() {
	delete d;
}

/*!
 * Defines the default message to be commited. The message is show in the dialog.
 * The user can change the message.
 * \param message Message to be stocked when commit is called.
 */
void CommitMessageDialogImpl::setMessages( const QString & message ) {
	m_textEditMessages->setText( message );
}

/*!
 * Return the message define by the user.
 * \return The message to be sent to CVS.
 */
QString CommitMessageDialogImpl::messages() {
	return m_textEditMessages->toPlainText();
}

/*!
 * Set the list of file to be commited. If the file as mode RCS::RemoveAndCommit 
 * or AddAndCommit the file is not checked in the list. If the mode is RCS::Commit
 * the file is checked.
 * \param files List of files who ca be add/remove and commited
 */
void CommitMessageDialogImpl::setFilesOperation( RCS::FilesOperation files ) {
	Q_ASSERT( global.m_project != NULL );
	
	m_fileListWidget->clear();
	d->m_files = files;
	foreach( RCS::FileOperation file, d->m_files ) {
		QString fileName = QDir( global.m_project->projectPath() ).relativeFilePath( file.first );
		switch( file.second ) {
		case RCS::Commit: 
			fileName += tr( " ( Commit )" );
			break;
		case RCS::AddAndCommit: 
			fileName += tr( " ( Add and Commit )" );
			break;
		case RCS::RemoveAndCommit: 
			fileName += tr( " ( Remove and Commit )" );
			break;
		default: ;
		}
		QListWidgetItem * item = new QListWidgetItem( fileName, m_fileListWidget );
		item->setCheckState( Qt::Unchecked );
		if( file.second == RCS::Commit ) {
			item->setCheckState( Qt::Checked );
		}
	}
}

/*!
 * Return the file list.
 * If file is unchecked then the mode RCS::Nothing is set.
 * \return Return the list of files the user whant to commit.
 */
RCS::FilesOperation CommitMessageDialogImpl::filesOperation() {
	for( int i = 0 ; i < m_fileListWidget->count(); i++ ) {
		RCS::FileOperation op;
		op = d->m_files.at( i );
		if( m_fileListWidget->item( i )->checkState() == Qt::Unchecked )
			d->m_files.replace( i , qMakePair( op.first, RCS::Nothing ) );
	}
	return d->m_files;
}

/*!
 * Redefine the exec function. When this function is called if the function 
 * setFilesOperation aren't called then a message is popup (There is nothing to do).
 */
int CommitMessageDialogImpl::exec() {
	if( d->m_files.size() == 0 ) {
		QMessageBox::information( this, tr("Commit"), tr("Nothing to do.") );
		return QDialog::Rejected;
	}
	return QDialog::exec();
}
