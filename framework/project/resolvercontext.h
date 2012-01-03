/*
 XINX
 Copyright (C) 2007-2012 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RESOLVERCONTEXT_H
#define RESOLVERCONTEXT_H
#pragma once

#include <core/lib-config.h>
#include <QSharedDataPointer>
#include <QSharedPointer>

class ResolverContextInformationData;
namespace XinxProject
{
	class Project;
	typedef QSharedPointer<Project> ProjectPtr;
	typedef QWeakPointer<Project> ProjectPtrWeak;
}

class LIBEXPORT ResolverContextInformation
{
public:
	ResolverContextInformation();
	ResolverContextInformation(const ResolverContextInformation& other);
	~ResolverContextInformation();

	bool isEmpty() const;

	XinxProject::ProjectPtr project() const;
	void setProject(XinxProject::ProjectPtr project);

	const QString & originalFileName() const;
	void setOriginalFileName(const QString & filename);

	const QString & currentPath() const;
	void setCurrentPath(const QString & path);

	void clearProperties();
	void setProperty(const QString & name, const QVariant & value);
	QVariant getProperty(const QString & name) const;

	bool operator==(const ResolverContextInformation & other) const;
	ResolverContextInformation & operator=(const ResolverContextInformation & other);
private:
	QSharedDataPointer<ResolverContextInformationData> d;
	friend uint qHash(const ResolverContextInformation &);
};

uint qHash(const ResolverContextInformation &);

#endif // RESOLVERCONTEXT_H
