/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

#ifndef MAINFORMIMPL_H
#define MAINFORMIMPL_H

// Xinx header
#include "ui_mainform.h"

// Qt header
#include <QMainWindow>
#include <QString>
#include <QStringList>

class PrivateMainformImpl;

/*!
 * This is the implementation of the MAIN FORM of XINX. This Windows allow the user to edit file,
 * create new project, open it. It show also dock for file content, and project content. It is
 * the main part of the project to interact with the user.
 *
 * This form contains also some function that can be call with D-BUS. This functionnality is defined
 * slots.
 */
class MainformImpl : public QMainWindow, public Ui::MainForm {
	Q_OBJECT
public:
	/*!
	 * Create the main form.
	 * Theres is normally no parent widget, and no specifique flags for the user interface.
	 */
	MainformImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	/*!
	 * Destroy the main form, and consequently quit the application.
	 */
	virtual ~MainformImpl();
public slots:
	/*!
	 * Open a dialog to create a new template (snipet).
	 * To create a template, a text must be selected.
	 * The dialog interact with the user to choose the information.
	 * \todo Propose some options, to create the a snipet without user interaction.
	 */
	void newTemplate();
	/*!
	 * Open the file \e filename in the tab editor
	 */
	void openFile( const QString & filename );
	/*!
	 * Save the current selected editor, in the file defined by \e filename. If
	 * no filename is specified, the current name is used.
	 */
	void saveFileAs( const QString & filename = QString() );
	/*!
	 * Save all opened files.
	 */
	void saveAllFile();
	/*!
	 * Close the current file. Ask to user to save it, if the file is modified.
	 */
	void closeFile();
	/*!
	 * Close all files of the editor. If some file are modified, XINX ask the user
	 * to save it.
	 */
	bool closeAllFile();

	/*!
	 * Create a new project. A dialog is opened and ask some question to the user.
	 * \todo Add some options to create a project without user interaciton.
	 */
	void newProject();
	/*!
	 * Open a project defined by the param \e filename. If project is already open,
	 * the project is closed.
	 */
	void openProject( const QString & filename );
	/*!
	 * Save(flush) the project.
	 * \param withSessionData If true, the project is saved with session data. It's used
	 * in case XINX stop with error.
	 */
	void saveProject( bool withSessionData );
	/*!
	 * Close the project with no session data.
	 */
	void closeProjectNoSessionData();
	/*!
	 * Close the project with session data. The session data saved is content of non-saved
	 * files.
	 */
	void closeProjectWithSessionData();

	/*!
	 * Update the project from Version management. If more one file is specified
	 * this method update only choose file.
	 */
	void updateFromVersionManager( const QStringList & list = QStringList() );
	/*!
	 * Commit the project from Version managerment. If more one file is specified
	 * this method update only choose file.
	 */
	void commitToVersionManager( const QStringList & list = QStringList() );
	/*!
	 * Add some files to version management.
	 */
	void addFilesToVersionManager( const QStringList & list );
	/*!
	 * Remove some file from version management.
	 * \warning Files are also delete from disk.
	 */
	void removeFilesFromVersionManager( const QStringList & list );
protected:
	void closeEvent( QCloseEvent *event );
private:
	PrivateMainformImpl * d;
	friend class PrivateMainformImpl;
};

#endif





