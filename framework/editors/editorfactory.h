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

#ifndef EDITORFACTORY_H
#define EDITORFACTORY_H

// Xinx header
#include <core/lib-config.h>
#include <core/xinxsingleton.h>
#include <project/xinxprojectproject.h>

// Qt header
#include <QObject>
#include <QSharedPointer>

class AbstractEditor;
class IFileTypePlugin;

namespace XinxSession
{
	class SessionEditor;
}

class LIBEXPORT EditorFactory : public XinxLibSingleton<EditorFactory>
{
	Q_OBJECT
public:
	virtual ~EditorFactory();

	IFileTypePlugin * interfaceOfName(const QString & name);
	AbstractEditor * createEditor(const QString & filename, IFileTypePlugin * interface = 0, XinxProject::ProjectPtr project = XinxProject::ProjectPtr());
	AbstractEditor * createEditor(XinxSession::SessionEditor * session = 0);
private:
	EditorFactory();

	AbstractEditor * createEditor(IFileTypePlugin * interface);

	friend class XinxLibSingleton<EditorFactory>;
};

#endif // EDITORFACTORY_H
