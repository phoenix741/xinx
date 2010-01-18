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

#ifndef EXTERNALFILERESOLVER_H
#define EXTERNALFILERESOLVER_H
#pragma once

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QObject>
#include <QHash>

class IFileResolverPlugin;

/* ExternalFileResolver */

class LIBEXPORT ExternalFileResolver : public QObject {
	Q_OBJECT
public:
	virtual ~ExternalFileResolver();

	QStringList externalFileResoverIds() const;
	QStringList externalFileResoverNames() const;
	IFileResolverPlugin * externalFileResover( const QString & id ) const;

	QString resolveFileName( const QString & nameToResolve, const QString & currentPath = QString() );

	static ExternalFileResolver * self();
public slots:
	void clearCache();
private:
    ExternalFileResolver();

	QHash< QPair<QString,QString> ,QString> m_externalFileResolverCache;

	static ExternalFileResolver * s_self;
};

#endif // EXTERNALFILERESOLVER_H
