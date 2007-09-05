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

#ifndef __CVSFILES_H__
#define __CVSFILES_H__

// Xinx header
#include "rcs.h"

// Qt header
#include <QString>
#include <QDateTime>
#include <QFileInfo>
#include <QHash>

class QFileSystemWatcher;

/* CVSFileEntry */

/*!
 * Represent an cvs entry in file Entries.
 */
class CVSFileEntry : public QObject {
	Q_OBJECT
public:
	/*!
	 * Create a CVS File Entry
	 */
	CVSFileEntry();
	/*!
	 * Create a CVS File Entry. 
	 * \param filename The file name to used in the entry file.
	 * \param version The version of the entry in the file.
	 */
	CVSFileEntry( const QString & filename, const QString & version = QString() );
	/*!
	 * Create a CVS File Entry
	 * \param path The path where the file is.
	 * \param filename The file name in the entry.
	 * \param version The version number of the entry.
	 */
	CVSFileEntry( const QString & path, const QString & filename, const QString & version = QString() );
	/*!
	 * Delete the entry file.
	 */
	virtual ~CVSFileEntry();
	
	/*!
	 * Set the file name of the object. This file is watched for change.
	 * \param filename The file name to use with the object. 
	 */
	void setFileName( const QString & filename );
	/*!
	 * Set the file name and the path of the object. The file is watched for change.
	 * \param path The path where the file is.
	 * \param filename The filename to watch, in this path.
	 */
	void setFileName( const QString & path, const QString & filename );
	/*!
	 * Set the version to change.
	 * \param version The version to store in the object.
	 */
	void setVersion( const QString & version );
	
	/*!
	 * Return the status of the file.
	 * \return the status of the file.
	 */
	RCS::rcsState status();
public slots:
	/*!
	 * Set the date of the CVS file 
	 * \param date The date of the file.
	 */
	void setCVSFileDate( QString date );
	/*!
	 * Read the file date from entry.
	 */ 
	void getFileDate();
signals:
	/*!
	 * Signal emited when the file is changed.
	 */
	void fileChanged();
private:
	void init();
	
	QString m_fileName;

	QString m_cvsVersion;
	QDateTime m_cvsDate;
	bool m_hasConflict;
	
	QFileInfo m_fileInfo;
	QDateTime m_fileDate;
	
	QFileSystemWatcher * m_watcher;
};

/* CVSFileEntryList */

class CVSFileEntryList : public QHash<QString,CVSFileEntry> {
	
};

#endif // __CVSFILES_H__
