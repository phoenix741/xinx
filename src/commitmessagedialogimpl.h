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

#ifndef COMMITMESSAGEDIALOGIMPL_H
#define COMMITMESSAGEDIALOGIMPL_H
//
#include "ui_commitmessages.h"
//
#include "rcs.h"

class PrivateCommitMessageDialogImpl;

/*!
 * Implementation of a commit dialog. This dialog is used to choose file to be commited
 * and message to be assigned at the operations.
 */
class CommitMessageDialogImpl : public QDialog, public Ui::CommitMessageDialog {
	Q_OBJECT
public:
	/*!
	 * Constructor of the commit dialog. The dialog as a windows flag who said the flag is not
	 * resizable.
	 */
	CommitMessageDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	/*!
	 * Destructor of commit dialog.
	 */
	~CommitMessageDialogImpl();
	
	/*!
	 * Defines the default message to be commited. The message is show in the dialog.
	 * The user can change the message.
	 * \param message Message to be stocked when commit is called.
	 */
	void setMessages( const QString & message );
	/*!
	 * Return the message define by the user.
	 * \return The message to be sent to CVS.
	 */
	QString messages();
	
	/*!
	 * Set the list of file to be commited. If the file as mode RCS::RemoveAndCommit 
	 * or AddAndCommit the file is not checked in the list. If the mode is RCS::Commit
	 * the file is checked.
	 * \param files List of files who ca be add/remove and commited
	 */
	void setFilesOperation( RCS::FilesOperation files );
	/*!
	 * Return the file list.
	 * If file is unchecked then the mode RCS::Nothing is set.
	 * \return Return the list of files the user whant to commit.
	 */
	RCS::FilesOperation filesOperation(); 
	
public slots:

	/*!
	 * Redefine the exec function. When this function is called if the function 
	 * setFilesOperation aren't called then a message is popup (There is nothing to do).
	 */
	int exec();
private slots:
private:
	PrivateCommitMessageDialogImpl * d;
	friend class PrivateCommitMessageDialogImpl;
};
#endif





