/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#include <rcs/rcs.h>
#include <core/exceptions.h>

/* RCS */

/*!
 * \class RCS
 * \brief  Base class to create "revision control system" plugin.
 *
 * This plugin will create a derivated object when the project
 * will be open, and destroy this object when the project is closed.
 *
 * This object will be used along the project mode, to show to the user the \e status of a file, to \e update from repository,
 * to \e commit to the repository, to \e add a file to the repository, or to \e remove a file from the repository.
 */

/*!
 * \enum RCS::rcsLog
 * \brief Information that can be returned in the log
 */

/*!
 * \var RCS::rcsLog RCS::LogError
 * \brief The message is an error
 */
/*!
 * \var RCS::rcsLog RCS::LogNormal
 * \brief The message has no attribute
 */
/*!
 * \var RCS::rcsLog RCS::LogConflict
 * \brief The message is about conflicted files.
 */
/*!
 * \var RCS::rcsLog RCS::LogLocallyModified
 * \brief The message is about modified files on the disk.
 */
/*!
 * \var RCS::rcsLog RCS::LogRemotlyModified
 * \brief The message is about modified files on the repository.
 */
/*!
 * \var RCS::rcsLog RCS::LogNotManaged
 * \brief The message has an unknown type.
 */
/*!
 * \var RCS::rcsLog RCS::LogApplication
 * \brief This is the application (XINX) who log the message.
 */


/*!
 * \enum RCS::rcsState
 * \brief Information that can be returned in the log
 */

/*!
 * \var RCS::rcsState RCS::Updated
 * \brief The file is up to date, no operation to do.
 */
/*!
 * \var RCS::rcsState RCS::LocallyModified
 * \brief The file is modified on the disk but not in the repository.
 */
/*!
 * \var RCS::rcsState RCS::LocallyAdded
 * \brief The file is added to the repository
 */
/*!
 * \var RCS::rcsState RCS::LocallyRemoved
 * \brief The file is removed from the repository
 */
/*!
 * \var RCS::rcsState RCS::NeedsCheckout
 * \brief The file exists in the repository, but not in the disk
 */
/*!
 * \var RCS::rcsState RCS::NeedPatch
 * \brief Not used
 */
/*!
 * \var RCS::rcsState RCS::UnresolvedConflict
 * \brief The file has a conflict but not resolved
 */
/*!
 * \var RCS::rcsState RCS::FileHadConflictsOnMerge
 * \brief The file has a resolved conflict.
 */
/*!
 * \var RCS::rcsState RCS::Unknown
 * \brief The state of the file is unknown, the file is not is the repository
 */

/*!
 * \struct RCS::struct_rcs_infos
 * \brief Information that can be returned in the log
 */

/*!
 * \var RCS::struct_rcs_infos::filename
 * \brief The informations' filename
 */
/*!
 * \var RCS::struct_rcs_infos::state
 * \brief State of the file (see the enum \e rcsState)
 */
/*!
 * \var RCS::struct_rcs_infos::version
 * \brief Version of the file on the disk
 */
/*!
 * \var RCS::struct_rcs_infos::rcsDate
 * \brief Date of the last modified time in the RCS
 */

/*!
 * \enum RCS::rcsOperation
 * \brief Operation that can be made on a file when commit is asked
 */

/*!
 * \var RCS::rcsOperation RCS::Updated
 * \brief Remove the file and commit this change
 */
/*!
 * \var RCS::rcsOperation RCS::LocallyModified
 * \brief Add the file and commit it.
 */
/*!
 * \var RCS::rcsOperation RCS::LocallyAdded
 * \brief Commit the file
 */
/*!
 * \var RCS::rcsOperation RCS::LocallyRemoved
 * \brief Do nothing
 */

/*!
 * \struct RCS::FileOperation
 * \brief Pair of file name and operation to do.
 */

/*!
 * \var RCS::FileOperation::filename
 * \brief File name impacted by the operation
 */
/*!
 * \var RCS::FileOperation::operation
 * \brief Operation made on the filename
 */
/*!
 * \brief Create an operation
 */
RCS::FileOperation::FileOperation(const QString & f, rcsOperation o) : filename(f), operation(o)
{
}

/*!
 * \brief Create an empty operation
 */
RCS::FileOperation::FileOperation()
{
}

/*!
 * \typedef RCS::FilesOperation
 * \brief List of \e FileOperation
 */

/*!
 * Create a RCS base object.
 * \param workingDirectory This is the project path, used in the RCS as the base path of operations.
 */
RCS::RCS(const QString & workingDirectory) : m_workingDirectory(workingDirectory)
{
	qRegisterMetaType<RCS::rcsLog>("RCS::rcsLog");
}

/*! Destroy the object */
RCS::~RCS()
{

}

/*!
 * \fn virtual void RCS::update(const QStringList & paths) = 0
 * \brief Call the update command of the plugin and return immediatly.
 *
 * The update function must be launch in a thread or a process and sent signal for log (\e log) or at the end
 * of the thread (\e operationTerminated).
 *
 * \param paths List of path or file to be updated.
 * \sa abort(), add(), remove(), commit(), updateToRevision()
 */

/*!
 * \fn virtual void RCS::updateToRevision(const QString & path, const QString & revision, QByteArray * content = 0) = 0
 * \brief Call the update command of the plugin for a file at a given revision.
 *
 * The update function can send signal for log (\e log).
 *
 * \param path Path of the file to retrieve.
 * \param revision Destination revision of the file.
 * \param content Where the content of the revision must be stored. If null, the revision must override the original file.
 * \sa abort(), add(), remove(), commit(), update()
 */

/*!
 * \fn virtual void RCS::commit(const QStringList & paths, const QString & message) = 0
 * \brief Call the commit command of the plugin.
 *
 * The commit function can send signal for log (\e log).
 *
 * \param paths List of file to be commited. The file must be added or removed to be commited. This method only call the commit method of the rcs.
 * \param message Message used while commit.
 * \sa abort(), add(), remove(), updateToRevision(), update()
 */


/*!
 * \fn virtual void RCS::add(const QStringList & paths) = 0
 * \brief Call the add command of the plugin.
 *
 * The add function can send signal for log (\e log).
 * of the thread (\e operationTerminated).
 *
 * \param paths List of path or file to be added.
 * \sa abort(), remove(), commit(), updateToRevision(), update()
 */

/*!
 * \fn virtual void RCS::remove(const QStringList & paths) = 0
 * \brief Call the remove command of the plugin.
 *
 * The remove function can send signal for log (\e log).
 *
 * \param paths List of path or file to be removed.
 * \sa abort(), add(), commit(), updateToRevision(), update()
 */

/*!
 * \fn virtual RCS::FilesOperation RCS::operations(const QStringList & path) = 0
 * \brief Return a list of operation for a path or a list of path (in case of multiple selections.
 *
 * In function of plugin and state of file, the operation can be \e RCS::RemoveAndCommit, \e RCS::AddAndCommit,
 * and \e RCS::Commit. This list will be used in the commit dialog to show files to Commit.
 */

/*!
 * \fn virtual RCS::struct_rcs_infos RCS::info(const QString & path) = 0
 * \brief Take a file name in argument and return informations about the filename in a \e struct_rcs_infos structure.
 *
 * The file name must be in the base path
 * \sa getBasePath(), searchFileToAddOrRemove()
 */

/*!
 * \fn virtual void RCS::abort() = 0
 * \brief This function is called when an operation is in progress and the user want abort it.
 *
 * If the operation can't be aborted, the function do nothing.
 */

/*!
 * \fn void RCS::stateChanged(const QString & fileName, RCS::struct_rcs_infos informations)
 * \brief Signal emited when the state of a file is changed but the file don't change (because file is commited,
 * or for another reason).
 * \param fileName the name of the file who changed.
 * \param informations New status of the file
 */

/*!
 * \fn void RCS::log(RCS::rcsLog niveau, const QString & info)
 * \brief Signal emited when a new message must be added to the log. The \e niveau is used to highlight the
 * text in different way, depending on the gravity of the message.
 * \param niveau gravity of the message
 * \param info message to be added.
 * \sa add(), remove(), commit(), updateToRevision(), update()
 */

/// Set the base path of the revision control system class.
void RCS::setWorkingDirectory(const QString & value)
{
	m_workingDirectory = value;
}

/// Return the base path of the revision control system class.
const QString & RCS::getWorkingDirectory() const
{
	return m_workingDirectory;
}
