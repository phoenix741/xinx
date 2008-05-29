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

#ifndef __RCS_H__
#define __RCS_H__

// Xinx header
#include "xinxcore.h"

// Qt header
#include <QMetaType>
#include <QObject>
#include <QPair>
#include <QList>
#include <QDateTime>

/*!
 * Base class to create "revision control system" plugin. This plugin will create a derivated object when the project
 * will be open, and destroy this object when the project is closed.
 * 
 * This object will be used along the project mode, to show to the user the \e status of a file, to \e update from repository,
 * to \e commit to the repository, to \e add a file to the repository, or to \e remove a file from the repository.
 */
class  RCS : public QObject {
	Q_OBJECT
public:
	/*! 
	 * Create a RCS base object.
	 * \param basePath This is the project path, used in the RCS as the base path of operations.
	 */
	RCS( const QString & basePath );
	/*! Destroy the object */
	virtual ~RCS();

	/*! RCS state of a file on the disk */
	enum rcsState {
		Updated, ///< The file is up to date, no operation to do. 
		LocallyModified, ///< The file is modified on the disk but not in the repository. 
		LocallyAdded, ///< The file is added to the repository 
		LocallyRemoved, ///< The file is removed from the repository 
		NeedsCheckout, ///< The file exists in the repository, but not in the disk 
		NeedPatch, ///< Not used 
		UnresolvedConflict, ///< The file has a conflict but not resolved 
		FileHadConflictsOnMerge, ///< The file has a resolved conflict. 
		Unknown ///< The state of the file is unknown, the file is not is the repository
	};
	/*! Information that can be returned in the log */
	enum rcsLog {
		LogError, ///< The message is an error 
		LogNormal, ///< The message has no attribute 
		LogConflict, ///< The message is about conflicted files. 
		LogLocallyModified, ///< The message is about modified files on the disk. 
		LogRemotlyModified, ///< The message is about modified files on the repository.  
		LogNotManaged, ///< The message has an unknown type. 
		LogApplication ///< This is the application (XINX) who log the message.
	};
	/*! Operation that can be made on a file when commit is asked */
	enum rcsOperation {
		RemoveAndCommit, ///< Remove the file and commit this change
		AddAndCommit, ///< Add the file and commit it.
		Commit, ///< Commit the file
		Nothing ///< Do nothing
	};
	/*! Informations about a file  */
	struct struct_rcs_infos {
		rcsState state; ///< State of the file (see the enum \e rcsState)
		QString version; ///< Version of the file on the disk
		QDateTime rcsDate; ///< Date of the last modified time in the RCS
	};
	
	/// Pair of file name and operation to do. 
	typedef QPair<QString,rcsOperation> FileOperation;
	/// List of \e FileOperation 
	typedef QList<FileOperation> FilesOperation;
	
	/*!
	 * Take a file name in argument and return informations about the filename in a \e struct_rcs_infos structure.
	 * The file name must be in the base path 
	 * \sa getBasePath()
	 */
	virtual struct_rcs_infos infos( const QString & path ) = 0;
	/*!
	 * Return a list of operation for a path or a list of path (in case of multiple selections. In function of
	 * plugin and state of file, the operation can be \e RCS::RemoveAndCommit, \e RCS::AddAndCommit, and \e RCS::Commit.
	 * This list will be used in the commit dialog to show files to Commit.
	 */
	virtual FilesOperation operations( const QStringList & path ) = 0;
	/*!
	 * Call the update command of the plugin and return immediatly. 
	 * 
	 * The update function must be launch in a thread or a process and sent signal for log (\e log) or at the end
	 * of the thread (\e operationTerminated).
	 * 
	 * \param path List of path or file to be updated.
	 * \sa abort(), add(), remove(), commit(), updateToRevision()
	 */
	virtual void update( const QStringList & path ) = 0;
	/*!
	 * Call the update command of the plugin for a file at a given revision and return immediatly. 
	 * 
	 * The update function must be launch in a thread or a process and sent signal for log (\e log) or at the end
	 * of the thread (\e operationTerminated).
	 * 
	 * \param path Path of the file to retrieve.
	 * \param revision Destination revision of the file.
	 * \param content Where the content of the revision must be stored. If null, the revision must override the original file.
	 * \sa abort(), add(), remove(), commit(), update()
	 */
	virtual void updateToRevision( const QString & path, const QString & revision, QString * content = 0 ) = 0;
	/*!
	 * Call the commit command of the plugin and return immediatly. 
	 * 
	 * The commit function must be launch in a thread or a process and sent signal for log (\e log) or at the end
	 * of the thread (\e operationTerminated).
	 * 
	 * \param path List of file to be commited. Each file is associated with a operation : \e RCS::Commit, 
	 * \e RCS::AddAndCommit, and RCS::RemoveAndCommit.
	 * \param message Message used while commit. 
	 * \sa abort(), add(), remove(), updateToRevision(), update()
	 */
	virtual void commit( const FilesOperation & path, const QString & message ) = 0;
	/*!
	 * Call the add command of the plugin and return immediatly. 
	 * 
	 * The add function must be launch in a thread or a process and sent signal for log (\e log) or at the end
	 * of the thread (\e operationTerminated).
	 * 
	 * \param path List of path or file to be added.
	 * \sa abort(), remove(), commit(), updateToRevision(), update()
	 */
	virtual void add( const QStringList & path ) = 0;
	/*!
	 * Call the remove command of the plugin and return immediatly. 
	 * 
	 * The remove function must be launch in a thread or a process and sent signal for log (\e log) or at the end
	 * of the thread (\e operationTerminated).
	 * 
	 * \param path List of path or file to be removed.
	 * \sa abort(), add(), commit(), updateToRevision(), update()
	 */
	virtual void remove( const QStringList & path ) = 0;
	
	/// Return the base path of the revision control system class.
	const QString & getBasePath() const;
public slots:
	/*!
	 * This function is called when an operation is in progress and the user want abort it.
	 * If the operation can't be aborted, the function do nothing. 
	 */
	virtual void abort() = 0;
signals: 
	/*!
	 * Signal emited when the state of a file is changed but the file don't change (because file is commited, 
	 * or for another reason).
	 * \param fileName the name of the file who changed.
	 */
	void stateChanged( const QString & fileName );
	/*!
	 * Signal emited when a new message must be added to the log. The \e niveau is used to highlight the 
	 * text in different way, depending on the gravity of the message.
	 * \param niveau gravity of the message
	 * \param info message to be added.
	 * \sa add(), remove(), commit(), updateToRevision(), update()
	 */
	void log( RCS::rcsLog niveau, const QString & info );
	/*!
	 * Signal emited when an operation is terminated.
	 * \sa add(), remove(), commit(), updateToRevision(), update()
	 */
	void operationTerminated();
private:
	QString m_base;
};

Q_DECLARE_METATYPE( RCS::rcsLog )

#endif // __RCS_H__
