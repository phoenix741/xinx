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

#ifndef CORE_STYLESHEET_ITEMMODELFACTORY_H
#define CORE_STYLESHEET_ITEMMODELFACTORY_H

#include <codecompletion/itemmodelfactory.h>
#include <QApplication>
#include <contentview3/definitions.h>
#include <codecompletion/model.h>

namespace Core
{
namespace BaliseDefinition
{
class XmlContextType;
class XmlBalise;
}
};

namespace Core
{

namespace Stylesheet
{

class XslNodeItem : public CodeCompletion::ContentViewNodeItem
{
public:
	XslNodeItem(const ContentView3::NodePtr & node);
	virtual ~XslNodeItem();

	virtual void insertCompletionStart(CodeCompletion::Context context, TextFileEditor* editor, QDocumentCursor & cursor);
	virtual void insertCompletionEnd(CodeCompletion::Context context, TextFileEditor* editor, QDocumentCursor & cursor);
};

class XslTemplateItem : public XslNodeItem
{
public:
	XslTemplateItem(const ContentView3::NodePtr& node);
	virtual ~XslTemplateItem();

	virtual void execute(CodeCompletion::Context context, TextFileEditor* editor);
};

class ItemModelFactory : public CodeCompletion::ItemModelFactory
{
	Q_DECLARE_TR_FUNCTIONS(ItemModelFactory)
public:
	ItemModelFactory();
	virtual ~ItemModelFactory();

	virtual void generate();
private:
	void addWithParamName(BaliseDefinition::XmlContextType* c, QString contextType, ContentView3::NodePtr node);
	void addCallTemplate(Core::BaliseDefinition::XmlContextType * c, const QString& contextType, ContentView3::NodePtr node);
	void addApplyTemplatesSelect(Core::BaliseDefinition::XmlContextType* c, const QString& contextType, ContentView3::NodePtr node);
	void addApplyTemplatesMode(Core::BaliseDefinition::XmlContextType* c, const QString& contextType, ContentView3::NodePtr node);
	void addOther(Core::BaliseDefinition::XmlContextType* c, const QString& contextType, ContentView3::NodePtr node, bool local, Core::BaliseDefinition::XmlBalise balise);

	void addNode(const QString & contextType, ContentView3::NodePtr node, QString path, bool import);
};

}

}

#endif // CORE_STYLESHEET_ITEMMODELFACTORY_H
