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

#ifndef CORE_BALISEDEFINITION_ITEMMODELFACTORY_H
#define CORE_BALISEDEFINITION_ITEMMODELFACTORY_H

#include <codecompletion/itemmodelfactory.h>
#include <contentview3/file.h>
#include "editors/models/html_xsl_base/definition_rootnode.h"
#include <codecompletion/model.h>
#include <qdocumentcursor.h>

namespace Core
{
	namespace BaliseDefinition
	{
		class RootNode;
		class XmlContextType;
	}
};

namespace Core {

namespace BaliseDefinition {

class BaliseNodeItem : public CodeCompletion::ContentViewNodeItem
{
public:
	BaliseNodeItem(const ContentView3::NodePtr & node);
	virtual ~BaliseNodeItem();

	virtual void execute(CodeCompletion::Context context, TextFileEditor* editor);
};

class AttributeNodeItem : public CodeCompletion::ContentViewNodeItem
{
public:
	AttributeNodeItem(const ContentView3::NodePtr & node);
	virtual ~AttributeNodeItem();

	virtual void execute(CodeCompletion::Context context, TextFileEditor *editor);
};

class ItemModelFactory : public CodeCompletion::ItemModelFactory
{
	Q_DECLARE_TR_FUNCTIONS(ItemModelFactory)
public:
	ItemModelFactory();
	virtual ~ItemModelFactory();

	virtual void generate();
private:
	void addCompletion(XmlContextType * c, const QString & context, QSharedPointer<Core::BaliseDefinition::RootNode> root);
	void addBalises(const QString & context, QSharedPointer<Core::BaliseDefinition::RootNode> node);
	void addAttributes(XmlContextType * c, const QString & context, QSharedPointer<Core::BaliseDefinition::RootNode> root);
	void addValues(XmlContextType * c, const QString & context, QSharedPointer<Core::BaliseDefinition::RootNode> root);

	const QString _xsl_context;
	const QString _html_context;
	const QString _ns_context;
};

}

}

#endif // CORE_BALISEDEFINITION_ITEMMODELFACTORY_H
