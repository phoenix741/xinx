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

#include <QMetaType>
#include <QObject>
#include <QPair>
#include <QList>
#include <QDateTime>

class RCS : public QObject {
	Q_OBJECT
public:
	RCS( const QString & basePath );
	virtual ~RCS();

	enum rcsState {
		Updated, LocallyModified, LocallyAdded, LocallyRemoved, NeedsCheckout, NeedPatch, UnresolvedConflict, FileHadConflictsOnMerge, Unknown
	};
	enum rcsLog {
		LogError, LogNormal, LogConflict, LogLocallyModified, LogRemotlyModified, LogNotManaged, LogApplication
	};
	enum rcsOperation {
		RemoveAndCommit, AddAndCommit, Commit, Nothing
	};
	enum rcsInfos {
		rcsVersions, rcsDate, rcsFileDate
	};
	
	struct struct_rcs_infos {
		rcsState state;
		QString version;
		QDateTime rcsDate;
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
