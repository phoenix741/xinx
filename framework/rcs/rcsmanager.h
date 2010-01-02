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

#ifndef RCSMANAGER_H
#define RCSMANAGER_H

// Xinx header
#include <rcs/rcs.h>

// Qt header
#include <QObject>
#include <QMap>
#include <QQueue>
#include <QPair>

/*!
 * The RCS Manager is the new interface to use to interact with RCS plugin.
 */
class RCSManager : public QObject {
	Q_OBJECT
public:
	//! Operation that can be added to the RCS Manager
	enum rcsManagerOperation {
		RCS_ADD,    //!< Add a file to RCS
		RCS_REMOVE, //!< Remove a file to the RCS
		RCS_STATUS,
		RCS_UPDATE
	};

	//! Destroy the RCS Manager
	~RCSManager();

	/*!
	 * List all the revision control that can be used. The Result is a list of
	 * QPair. The first element is the key and the second element is the description.
	 */
	QMap<QString,QString> revisionControlIds() const;

	/*!
	 * Method called by XINX to set a Revision control system.
	 * \param revision The system to use.
	 * \param basePath The path used in the constructor.
	 */
	bool setCurrentRCS( const QString & rcs, const QString & rootPath );
	//! Return the name of the current RCS
	QString currentRCS() const;
	//! Return the description of the current RCS
	QString description() const;

	//! Add an operation to the the RCS
	void addOperation( rcsManagerOperation op, const QStringList & filename );
	//! Validate all operation to the RCS (made in a separate thread).
	void validOperations();
	//! Rollback all operation to the RCS (return imediatly).
	void rollbackOperations();

	//! Return the single instance of the RCSManager
	static RCSManager * self();

	void abort();
signals:
	void statusChange( const QString & filename, RCS::struct_rcs_infos informations );
	void log( RCS::rcsLog niveau, const QString & info );
	void operationTerminated();
private:
	RCSManager();

	RCS * createRevisionControl( QString revision, QString basePath ) const;

	QString m_rcsName;
	RCS * m_rcs;
	QQueue< QPair<rcsManagerOperation,QStringList> > m_operations;
	static RCSManager * s_self;
};

#endif // RCSMANAGER_H
