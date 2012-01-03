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

#include "resolvercontext.h"
#include <QHash>
#include <QVariant>
#include <project/xinxprojectproject.h>

/* ResolverContextInformationData */

class ResolverContextInformationData : public QSharedData
{
public:
	ResolverContextInformationData();
	ResolverContextInformationData(const ResolverContextInformationData & other);

	QHash<QString,QVariant> _properties;
	QString _original_file_name;
	QString _current_path;
	XinxProject::ProjectPtrWeak _project;
};

ResolverContextInformationData::ResolverContextInformationData()
{

}

ResolverContextInformationData::ResolverContextInformationData(const ResolverContextInformationData & other) : QSharedData(other), _properties(other._properties), _original_file_name(other._original_file_name), _current_path(other._current_path), _project(other._project)
{

}

/* ResolverContextInformation */

ResolverContextInformation::ResolverContextInformation() : d(new ResolverContextInformationData)
{

}

ResolverContextInformation::ResolverContextInformation(const ResolverContextInformation& other) : d(other.d)
{

}

ResolverContextInformation::~ResolverContextInformation()
{

}

bool ResolverContextInformation::isEmpty() const
{
	return d->_project.isNull() && d->_original_file_name.isNull() && d->_current_path.isNull() && d->_properties.isEmpty();
}

XinxProject::ProjectPtr ResolverContextInformation::project() const
{
	return d->_project.toStrongRef();
}

void ResolverContextInformation::setProject(XinxProject::ProjectPtr project)
{
	d->_project = project.toWeakRef();
}

const QString & ResolverContextInformation::originalFileName() const
{
	return d->_original_file_name;
}

void ResolverContextInformation::setOriginalFileName(const QString & filename)
{
	d->_original_file_name = filename;
}

const QString & ResolverContextInformation::currentPath() const
{
	return d->_current_path;
}

void ResolverContextInformation::setCurrentPath(const QString & path)
{
	d->_current_path = path;
}

void ResolverContextInformation::clearProperties()
{
	d->_properties.clear();
}

void ResolverContextInformation::setProperty(const QString & name, const QVariant & value)
{
	d->_properties.insert(name, value);
}

QVariant ResolverContextInformation::getProperty(const QString & name) const
{
	return d->_properties.value(name);
}

bool ResolverContextInformation::operator==(const ResolverContextInformation & other) const
{
	return (d->_current_path == other.d->_current_path)
		&& (d->_original_file_name == other.d->_original_file_name)
		&& (d->_project == other.d->_project)
		&& (d->_properties == other.d->_properties);
}

ResolverContextInformation & ResolverContextInformation::operator=(const ResolverContextInformation & other)
{
	d = other.d;
	return *this;
}

/**
 * The hash is calculate on 3 member only. This is not a problem, because the == operator will be used to made a
 * differentiation.
 */
uint qHash(const ResolverContextInformation & o)
{
	uint result = 0;

	result += qHash(o.currentPath()) * 2;
	result += qHash(o.originalFileName()) * 5;

	XinxProject::ProjectPtr project = o.project();
	if (project)
	{
		result += qHash(project->projectPath()) * 7;
	}

	return result;
}
