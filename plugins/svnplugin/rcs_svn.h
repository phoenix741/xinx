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

#ifndef RCS_SVN_H_
#define RCS_SVN_H_

// Xinx header
#include <rcs/rcs.h>

// Qt header
#include <QProcess>
#include <QPointer>
#include <QCache>

class RCS_SVN : public RCS {
	Q_OBJECT
public:
	RCS_SVN( const QString & basePath );
	virtual ~RCS_SVN();

	virtual struct_rcs_infos infos( const QString & path );
	virtual FilesOperation operations( const QStringList & paths );
	virtual void update( const QStringList & path );
	virtual void commit( const FilesOperation & path, const QString & message );
	virtual void add( const QStringList & path );
	virtual void remove( const QStringList & path );
	virtual void updateToRevision( const QString & path, const QString & revision, QString * content = 0 );

public slots:
	virtual void abort();
private slots:
	void logMessages();
	void finished( int exitCode, QProcess::ExitStatus exitStatus );
	void updateToRevisionFinished( int exitCode, QProcess::ExitStatus exitStatus );
private:
	virtual FilesOperation operations( const QString & path );

	QPointer<QProcess> m_process;
	QString * m_content, m_tmpfilename;
	QStringList m_fileChanged;
	QHash<QString,RCS::struct_rcs_infos> m_infos;
	bool m_isCommit;
};

#endif /*RCS_SVN_H_*/
