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
class QTimer;
class CVSFileEntryList;

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
	
	/*!
	 * Read the file date from entry.
	 */ 
	const QDateTime & getFileDate();
	
	/*!
	 * Read the CVS version stored in the object.
	 */
	const QString & getVersion() const;
	
	/*!
	 * Read the CVS Date stored in the object.
	 */
	const QDateTime & getCVSDate() const;
	
	/*!
	 * Read the file name from the object. 
	 */
	const QString & getFileName() const;

public slots:
	/*!
	 * Set the date of the CVS file 
	 * \param date The date of the file.
	 */
	void setCVSFileDate( QString date );
signals:
	/*!
	 * Signal emited when the file is changed.
	 */
	void fileChanged( const QString & filename );
private slots:
	void slotFileChanged( const QString & file );
private:
	static QFileSystemWatcher * watcherInstance();
	static void deleteInstance();
	
	void init();
	void refreshStatus();
	
	QString m_fileName;

	QString m_cvsVersion;
	QDateTime m_cvsDate;
	bool m_hasConflict;
	RCS::rcsState m_status; 
	
	QFileInfo m_fileInfo;
	QDateTime m_fileDate;
	
	friend class CVSFileEntryList;
};

/* CVSFileEntryList */

/*!
 * Watch the CVS Entries file and the directory contains files. If a file is added or removed
 * a signal can be emited to refresh the project repository. If a file is modified, a signal is 
 * propaged from CVSFileEntry to CVSFileEntryList.
 * 
 * This object is a Hashtable of path. This object propose method to search a path and check it's 
 * status.
 * 
 * This class contains also a list of directory. This directory are also a CVSFileEntryList. At a request
 * of a sub-directory, the object refere to another object. If the object isn't created, he create it.
 */
class CVSFileEntryList : public QObject, public QHash<QString,CVSFileEntry*> {
	Q_OBJECT
public:
	/*!
	 * Create a cvs file entry list contains all the entries from path \e path.
	 */
	CVSFileEntryList( const QString & path );
	/*!
	 * Destroy a CVS.
	 */
	virtual ~CVSFileEntryList();

	/*!
	 * Return the status of a file \e filename. If CVSFileEntryList don't find the file in himself
	 * he return Unknown.
	 * 
	 * If \e filename is just a filename, he search in himself the file, else, he search in a child directory
	 * object the status of the file.
	 * 
	 * \return the status of the file.
	 */
	CVSFileEntry * object( const QString & filename );
	
	/*! 
	 * Return CVS informations of the file \e filename.
	 * 
	 * \return CVSFileEntry for the file \e filename
	 */
	RCS::rcsState status( const QString & filename );
	
	/*!
	 * Return the entriesList for the path \e path.
	 * 
	 * \return list of entries file
	 */
	CVSFileEntryList * path( const QString & filename );
signals:
	/*!
	 * Signal is emitted when a file or a directory \e filename is modified.
	 */
	void fileChanged( const QString & filename );
private slots:
	void entriesChanged( const QString & filename );
	void directoryChanged( const QString & filename );
	void loadEntriesFile();
private:
	QString m_path, m_entries;
	QHash<QString,CVSFileEntryList*> m_directoryList;
	QTimer * m_refreshTimer;
};

#endif // __CVSFILES_H__
