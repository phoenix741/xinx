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

#ifndef __RCS_H__
#define __RCS_H__
#pragma once

// Xinx header
#include <core/xinxcore.h>

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
	 * \param workingDirectory This is the project path, used in the RCS as the base path of operations.
	 */
	RCS( const QString & workingDirectory );
	/*!
	 * Create a RCS base object.
	 */
	RCS();
	/*! Destroy the object */
	virtual ~RCS();

	/*! Information that can be returned in the log */
	enum rcsLog {
		LogError,           ///< The message is an error
		LogNormal,          ///< The message has no attribute
		LogConflict,        ///< The message is about conflicted files.
		LogLocallyModified, ///< The message is about modified files on the disk.
		LogRemotlyModified, ///< The message is about modified files on the repository.
		LogNotManaged,      ///< The message has an unknown type.
		LogApplication      ///< This is the application (XINX) who log the message.
	};
	/*! RCS state of a file on the disk */
	enum rcsState {
		Updated,                 ///< The file is up to date, no operation to do.
		LocallyModified,         ///< The file is modified on the disk but not in the repository.
		LocallyAdded,            ///< The file is added to the repository
		LocallyRemoved,          ///< The file is removed from the repository
		NeedsCheckout,           ///< The file exists in the repository, but not in the disk
		NeedPatch,               ///< Not used
		UnresolvedConflict,      ///< The file has a conflict but not resolved
		FileHadConflictsOnMerge, ///< The file has a resolved conflict.
		Unknown                  ///< The state of the file is unknown, the file is not is the repository
	};
	/*! Informations about a file  */
	struct struct_rcs_infos {
		rcsState state;    ///< State of the file (see the enum \e rcsState)
		QString version;   ///< Version of the file on the disk
		QDateTime rcsDate; ///< Date of the last modified time in the RCS
	};

	/* Method on file of the repository */

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
	 * Call the update command of the plugin for a file at a given revision.
	 *
	 * The update function can send signal for log (\e log).
	 *
	 * \param path Path of the file to retrieve.
	 * \param revision Destination revision of the file.
	 * \param content Where the content of the revision must be stored. If null, the revision must override the original file.
	 * \sa abort(), add(), remove(), commit(), update()
	 */
	virtual QByteArray updateToRevision( const QString & path, const QString & revision, QByteArray * content = 0 ) = 0;
	/*!
	 * Call the commit command of the plugin.
	 *
	 * The commit function can send signal for log (\e log).
	 *
	 * \param path List of file to be commited. The file must be added or removed to be commited. This method only call the commit method of the rcs.
	 * \param message Message used while commit.
	 * \sa abort(), add(), remove(), updateToRevision(), update()
	 */
	virtual void commit( const QStringList & path, const QString & message ) = 0;

	/* Operation on file of the working copy */

	/*!
	 * Call the add command of the plugin.
	 *
	 * The add function can send signal for log (\e log).
	 * of the thread (\e operationTerminated).
	 *
	 * \param path List of path or file to be added.
	 * \sa abort(), remove(), commit(), updateToRevision(), update()
	 */
	virtual void add( const QStringList & path ) = 0;
	/*!
	 * Call the remove command of the plugin.
	 *
	 * The remove function can send signal for log (\e log).
	 *
	 * \param path List of path or file to be removed.
	 * \sa abort(), add(), commit(), updateToRevision(), update()
	 */
	virtual void remove( const QStringList & path ) = 0;

	/*!
	 * Return the current list of file to add or remove in the list of path given.
	 *
	 * \param path List of path, or file to check
	 * \param toAdd List of file to add, returned by the method.
	 * \param toRemove List of file to remove, returned by the method.
	 * \sa infos()
	 */
	virtual void searchFileToAddOrRemove( const QStringList & path, QStringList & toAdd, QStringList & toRemove ) = 0;
	/*!
	 * Take a file name in argument and return informations about the filename in a \e struct_rcs_infos structure.
	 * The file name must be in the base path
	 * \sa getBasePath(), searchFileToAddOrRemove()
	 */
	virtual struct_rcs_infos infos( const QString & path ) = 0;

	/* Working Directory */

	/// Set the base path of the revision control system class.
	void setWorkingDirectory( const QString & value );

	/// Return the base path of the revision control system class.
	const QString & getWorkingDirectory() const;
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
	 * \param informations New status of the file
	 */
	void stateChanged( const QString & fileName, RCS::struct_rcs_infos informations );
	/*!
	 * Signal emited when a new message must be added to the log. The \e niveau is used to highlight the
	 * text in different way, depending on the gravity of the message.
	 * \param niveau gravity of the message
	 * \param info message to be added.
	 * \sa add(), remove(), commit(), updateToRevision(), update()
	 */
	void log( RCS::rcsLog niveau, const QString & info );
private:
	QString m_workingDirectory;
};

Q_DECLARE_METATYPE( RCS::rcsLog )

#endif // __RCS_H__
