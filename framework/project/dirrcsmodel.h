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

#ifndef __DIRRCSMODEL_H__
#define __DIRRCSMODEL_H__
#pragma once

// Xinx header
#include <rcs/rcs.h>

// Qt header
#include <QDirModel>
#include <QStringList>
#include <QCache>

class RCSCachedElement;

/*!
 * The goal of this class is to store the Version Manager Information \e RCS and show
 * to user information of this Version Manager Information.
 *
 * At construction of the object, the object create a \e RCS object if a project exists
 * and manage the RCS.
 * The currently version manager supported is CVS.
 *
 * The item model show in a different color files modified, added, removed, or not in the
 * repository.
 * A tool tip show some information that the information manager supporte.
 */
class DirRCSModel : public QDirModel {
	Q_OBJECT
public:
	DirRCSModel( const QStringList & nameFilters, QDir::Filters filters, QDir::SortFlags sort, QObject * parent = 0 );
	DirRCSModel( QObject *parent = 0 );
	virtual ~DirRCSModel();
	QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

	/*!
	 * Return the rcs associate with the model. If RCS is not managed, this
	 * function return NULL
	 * \return The rcs object.
	 */
	RCS * rcs();

public slots:
	void refresh( const QString & path );
private:
	RCSCachedElement cachedValue( const QString & key ) const;

	RCS * m_rcs;
	mutable QCache<QString,RCSCachedElement> m_cache;
};

#endif // __DIRRCSMODEL_H__