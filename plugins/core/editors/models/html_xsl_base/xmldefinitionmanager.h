/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
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

#ifndef XMLDEFINITIONMANAGER_H
#define XMLDEFINITIONMANAGER_H

#include <QSharedPointer>
#include <core/xinxsingleton.h>
#include <contentview3/file.h>
#include "editors/models/html_xsl_base/definition_rootnode.h"

namespace Core {
namespace BaliseDefinition {

class XmlDefinitionManager : public XinxSingleton<XmlDefinitionManager>
{
	Q_OBJECT
public:
	ContentView3::FilePtr getHtmlFile() const;
	ContentView3::FilePtr getXslFile() const;

	QSharedPointer<Core::BaliseDefinition::RootNode> getHtmlRootNode() const;
	QSharedPointer<Core::BaliseDefinition::RootNode> getXslRootNode() const;

private:
	XmlDefinitionManager();
	friend class XinxSingleton<XmlDefinitionManager>;

	ContentView3::FilePtr _html_file;
	ContentView3::FilePtr _xsl_file;
};

} // namespace BaliseDefinition
} // namespace Core

#endif // XMLDEFINITIONMANAGER_H
