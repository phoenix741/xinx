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

#include <codecompletion/pool.h>
#include <codecompletion/contextparser.h>
#include <codecompletion/itemmodelfactory.h>
#include <plugins/xinxpluginsloader.h>
#include <plugins/xinxpluginelement.h>
#include <plugins/interfaces/codecompletion.h>
#include <core/xinxconfig.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>
#include <codecompletion/model.h>
#include <codecompletion/iteminterface.h>

#include <QMultiHash>
#include <QStringList>

namespace CodeCompletion
{


class PrivatePool
{
public:
	XinxPluginsLoader * _loader;
	QMultiHash<QString,ContextParser*> _parsers;
	QMultiHash<QString,ItemModelFactory*> _factories;
};

/*!
 * \class Pool
 *
 * \note This class doesn't use lock on resource, for performance reason. Methods are normally called once at once. If this is bug generator, we add lock in this class.
 */

Pool::Pool(XinxPluginsLoader * loader) : d(new PrivatePool)
{
	d->_loader = loader;
	connect(loader, SIGNAL(pluginActivated(QString)), this, SLOT(updateParsers()));
	connect(loader, SIGNAL(pluginDesactivated(QString)), this, SLOT(updateParsers()));
}

Pool::~Pool()
{
	qDeleteAll(d->_factories.values());
	qDeleteAll(d->_parsers.values());
}

void Pool::updateParsers()
{
	qDeleteAll(d->_factories.values());
	d->_factories.clear();
	qDeleteAll(d->_parsers.values());
	d->_parsers.clear();

	foreach(XinxPluginElement * element, d->_loader->plugins())
	{
		ICodeCompletionPlugin * interface = qobject_cast<ICodeCompletionPlugin*> (element->plugin());
		if (element->isActivated() && interface)
		{
			QList<ContextParser*> parsers = interface->createContextParser();
			QList<ItemModelFactory*> factories = interface->createItemModelFactory();
			foreach(ContextParser* p, parsers)
			{
				d->_parsers.insert(element->name(), p);
			}

			foreach(ItemModelFactory* p, factories)
			{
				d->_factories.insert(element->name(), p);
			}
		}
	}
}

void Pool::generate(ItemInterface * interface, CodeCompletion::Context context)
{
	Q_ASSERT_X(interface, "Pool::generate", "ItemInterface to populate model must not be null");

	QStringList contextsType = interface->contextsType();
	foreach(const QString & contextType, contextsType)
	{
		interface->prepareRemovingOldItems(contextType);
	}

	QHashIterator<QString, ItemModelFactory*> i(d->_factories);
	while (i.hasNext())
	{
		i.next();

		ItemModelFactory* parser = i.value();

		parser->setItemInterface(interface);
		parser->setContext(context);

		parser->generate();
	}

	foreach(const QString & contextType, contextsType)
	{
		interface->removeOldItems(contextType);
	}
}

void Pool::updateContext(TextFileEditor * editor, CodeCompletion::Context context)
{
	context.setPrefix(editor->textEdit()->textUnderCursor(editor->textEdit()->textCursor()));

	QHashIterator<QString, ContextParser*> i(d->_parsers);
	while (i.hasNext())
	{
		i.next();

		ContextParser* parser = i.value();

		parser->setEditor(editor);
		parser->setContext(context);

		parser->updateContext();
	}
}

}
