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
class XINX_EXPORT RCS : public QObject {
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
	
	typedef QPair<QString,rcsOperation> FileOperation;
	typedef QList<FileOperation> FilesOperation;
	
	virtual struct_rcs_infos infos( const QString & path ) = 0; 
	virtual FilesOperation operations( const QStringList & path ) = 0;
	virtual void update( const QStringList & path ) = 0;
	virtual void commit( const FilesOperation & path, const QString & message ) = 0;
	virtual void add( const QStringList & path ) = 0;
	virtual void remove( const QStringList & path ) = 0;
	virtual void updateToRevision( const QString & path, const QString & revision, QString * content = 0 ) = 0;
	
	const QString & getBasePath() const;
public slots:
	virtual void abort() = 0;
signals: 
	void stateChanged( const QString & fileName );
	void log( RCS::rcsLog niveau, const QString & info );
	void operationTerminated();
private:
	QString m_base;
};

Q_DECLARE_METATYPE( RCS::rcsLog )

#endif // __RCS_H__
