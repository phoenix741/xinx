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

#ifndef __CVSTHREAD_H__
#define __CVSTHREAD_H__

// Xinx header
#include "rcs.h"

// Qt header
#include <QProcess>
#include <QThread>
#include <QStringList>

/* CVSThread */

/*!
 * Represent the base class used to create thread for CVS usage.
 * The class split the list of path from the constructor and pass to
 * \e callCVS only files from the same directory. So, callCVS can be calle
 * more one times.
 * 
 * To implement a call of CVS, you must redefine callCVS and transform the list
 * of parameters in a list of options, and the path of the executable. 
 */ 
class CVSThread : public QThread {
	Q_OBJECT
public:
	/*!
	 * Create a thread class. \e paths define the list of paths to process. The list 
	 * will be sorted and spitted to before call CVS.
	 * 
	 * \param paths List of path to process.
	 * \param terminate If true, the thread must call \e operationTerminated() when the 
	 * process is terminated.
	 */
	CVSThread( QStringList paths, bool terminate = true );
	/*!
	 * Destroy the thread.
	 */
	virtual ~CVSThread();
public slots:
	/*!
	 * Abort the thread. If called, the signal is sending to CVS application.
	 * On Windows, the signal can't be send. The process is killed ....
	 */
	void abort();
signals:
	/*!
	 * The signal is emited when the CVS application send information.
	 * \param niveau The niveau of thread.
	 * \param info Infos of the thread (i.e. The name of the file ...).
	 */
	void log( RCS::rcsLog niveau, const QString & info );
	/*!
	 * The signal is emited when the operation is terminated.
	 * If the terminate, at creation, is false, this signal is never emited.
	 */
	void operationTerminated();
protected:
	/*! 
	 * Called by the thread when there is data to read.
	 */ 
	virtual void processReadOutput();
	/*!
	 * Process the thread.
	 * In this base class, the process is to split the thread.
	 */
	virtual void run();
	/*!
	 * Call the CVS process.
	 * The process is executed and the thread wait output from CVS.
	 */
	virtual void callCVS( const QString & path, const QStringList & options );
	/*!
	 * Used to process one Line sending by CVS.
	 * This, determine the niveau of the log and send the signal \e log.
	 */
	void processLine( const QString & line );

	QProcess * m_process; ///< The CVS process executed.
	QStringList m_paths; ///< List of path to pass in parameters.
	bool m_terminate; ///< Determine if we want called \e operationTerminated().
};

/* CVSUpdateThread */

/*!
 * Thread used for the update of the working copy of a repository.
 */
class CVSUpdateThread : public CVSThread {
	Q_OBJECT
public:
	/*! 
	 * Create a thread with update directive.
	 * \param paths List of file to update
	 * \param terminate If true call signal operationTerminated()
	 */
	CVSUpdateThread( QStringList paths, bool terminate = true );
protected:
	virtual void callCVS( const QString & path, const QStringList & files );	
	virtual void run();
};

/* CVSUpdateRevisionThread */

/*!
 * Thread used to update one file to a specified revision.
 * If defined, the content can be stored in a string.
 */ 
class CVSUpdateRevisionThread : public CVSThread {
	Q_OBJECT
public:
	/*!
	 * Create an update revision thread.
	 * \param path The file to update in a specifique revision.
	 * \param revision The revision in wich the file must be udpated.
	 * \param content The content where the file must be stored. If not defined the old
	 * file is overwritten.
	 */
	CVSUpdateRevisionThread( const QString & path, const QString & revision, QString * content );
public slots:
	virtual void processReadOutput();
protected:
	virtual void callCVS( const QString & path, const QStringList & files );	
	virtual void run();
private:
	QString * m_content;
	QString m_revision;
};

/* CVSAddThread */

/*! 
 * Thread used to add files. 
 */
class CVSAddThread : public CVSThread {
	Q_OBJECT
public:
	/*! 
	 * Create a thread with the add directive.
	 * \param paths List of file to add
	 * \param terminate If true call signal operationTerminated()
	 */
	CVSAddThread( QStringList paths, bool terminate = true );
protected:
	virtual void callCVS( const QString & path, const QStringList & files );	
	virtual void run();
};

/* CVSRemoveThread */

/*!
 * Thread used to remove file.
 */
class CVSRemoveThread : public CVSThread {
	Q_OBJECT
public:
	/*! 
	 * Create a thread with remove directive.
	 * \param paths List of file to update
	 * \param terminate If true call signal operationTerminated()
	 */
	CVSRemoveThread( QStringList paths, bool terminate = true );
protected:
	virtual void callCVS( const QString & path, const QStringList & files );	
	virtual void run();
};

/* CVSCommitThread */

/*!
 * Thread to commit file to CVS. 
 * At the creation, a list of files is defined. The list contains the files 
 * to add and commit, to remove and commit, and to commit only.
 * This thread call CVSAddThread, and CVSRemoveThread and wait the end of each thread
 * to commit all.
 */
class CVSCommitThread : public CVSThread {
	Q_OBJECT
public:
	/*! 
	 * Create a thread with commit directive.
	 * \param paths List of file to commit, add and remove.
	 * \param message Message to use when commit.
	 */
	CVSCommitThread( RCS::FilesOperation paths, QString message );
protected:
	virtual void callCVS( const QString & path, const QStringList & files );	
	virtual void run();
private:
	QString m_message;
	QStringList m_addList;
	QStringList m_removeList;
};


#endif /* __CVSTHREAD_H__ */
