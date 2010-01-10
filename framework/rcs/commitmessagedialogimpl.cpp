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
#include "commitmessagedialogimpl.h"
#include <project/xinxproject.h>
#include <core/exceptions.h>

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

CommitMessageDialogImpl::CommitMessageDialogImpl( QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {
	setupUi(this);
	d = new PrivateCommitMessageDialogImpl( this );
}

CommitMessageDialogImpl::~CommitMessageDialogImpl() {
	delete d;
}

void CommitMessageDialogImpl::setMessages( const QString & message ) {
	m_textEditMessages->setText( message );
}

QString CommitMessageDialogImpl::messages() {
	return m_textEditMessages->toPlainText();
}

void CommitMessageDialogImpl::setFilesOperation( RCS::FilesOperation files ) {
	Q_ASSERT( XINXProjectManager::self()->project() != NULL );

	m_fileListWidget->clear();
	d->m_files = files;
	foreach( const RCS::FileOperation & file, d->m_files ) {
		QString fileName = QDir( XINXProjectManager::self()->project()->projectPath() ).relativeFilePath( file.filename );
		switch( file.operation ) {
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
		if( file.operation == RCS::Commit ) {
			item->setCheckState( Qt::Checked );
		}
	}
}

RCS::FilesOperation CommitMessageDialogImpl::filesOperation() {
	for( int i = 0 ; i < m_fileListWidget->count(); i++ ) {
		RCS::FileOperation op( d->m_files.at( i ) );
		if( m_fileListWidget->item( i )->checkState() == Qt::Unchecked ) {
			op.operation = RCS::Nothing;
			d->m_files.replace( i , op );
		}
	}
	return d->m_files;
}

int CommitMessageDialogImpl::exec() {
	if( d->m_files.size() == 0 ) {
		QMessageBox::information( this, tr("Commit"), tr("Nothing to do.") );
		return QDialog::Rejected;
	}
	return QDialog::exec();
}
