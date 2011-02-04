/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
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

#pragma once
#ifndef XINXSESSIONSESSIONEDITOR_H
#define XINXSESSIONSESSIONEDITOR_H

#include <core/lib-config.h>
#include <QString>
#include <QVariant>
#include <QScopedPointer>

namespace XinxProject
{
	class Project;
}

namespace XinxSession 
{

class Session;
class PrivateSessionEditor;

class LIBEXPORT SessionEditor
{
public:
	explicit SessionEditor(Session * parent = 0);
	~SessionEditor();

	void setProject(XinxProject::Project * project);
	XinxProject::Project * project() const;

	void writeProperty(const QString & key, QVariant value);
	QVariant readProperty(const QString & key);
	QStringList propertieKeys() const;
private:
	QScopedPointer<PrivateSessionEditor> d;
};


} // namespace XinxSession

#endif // XINXSESSIONSESSIONEDITOR_H
