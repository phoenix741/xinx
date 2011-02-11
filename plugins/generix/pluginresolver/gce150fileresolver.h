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

#ifndef GCE150FILERESOLVER_H
#define GCE150FILERESOLVER_H
#pragma once

// Xinx header
#include <plugins/interfaces/resolver.h>

// Qt header
#include <QApplication>

/* Gce150FileResolver */

class Gce150FileResolver : public IFileResolverPlugin
{
	Q_DECLARE_TR_FUNCTIONS(Gce150FileResolver)
public:
	Gce150FileResolver();
	virtual ~Gce150FileResolver();

	static QString staticId();
	virtual QString id();
	virtual QString name();
	virtual bool resolveFileName (const QString& nameToResolve, QString& resolvedName, const QString& currentPath, XinxProject::ProjectPtr project);
};

#endif // GCE150FILERESOLVER_H
