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

#include <QObject>
#include <QPair>
#include <QList>

class RCS : public QObject {
	Q_OBJECT
public:
	enum rcsState {
		Updated, LocallyModified, LocallyAdded, LocallyRemoved, NeedsCheckout, NeedPatch, UnresolvedConflict, FileHadConflictsOnMerge, Unknown
	};
	enum rcsLog {
		Error, Warning, Information, Debug
	};
	enum rcsOperation {
		RemoveAndCommit, AddAndCommit, Commit, Nothing
	};
	
	typedef QPair<QString,rcsOperation> FileOperation;
	typedef QList<FileOperation> FilesOperation;
	
	virtual rcsState status( const QString & path ) = 0;
	virtual FilesOperation operations( const QString & path ) = 0;
	virtual void update( const QString & path ) = 0;
	virtual void commit( const QString & path, const QString & message ) = 0;
	virtual void add( const QString & path ) = 0;
	virtual void remove( const QString & path ) = 0;
public slots:
	virtual void abort() = 0;
signals: 
	void stateChanged( const QString & fileName );
	void log( RCS::rcsLog niveau, const QString & info );
	void updateTerminated();
	void commitTerminated();
	void addTerminated();
	void removeTerminated();
};

#endif // __RCS_H__
