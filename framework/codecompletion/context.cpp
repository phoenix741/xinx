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

// Xinx header
#include "context.h"
#include <codecompletion/contexttype.h>
#include <contentview3/file.h>

// Qt header
#include <QHash>

namespace CodeCompletion
{

/* PrivateContext */

class PrivateContext : public QSharedData
{
public:
	PrivateContext();
	~PrivateContext();

	QString _filename, _prefix;
	QHash<QString,ContextType*> _context;
	ContentView3::FilePtrWeak _file;
};

PrivateContext::PrivateContext()
{

}

PrivateContext::~PrivateContext()
{
	qDeleteAll(_context);
}

/* Context */

Context::Context()
{
	d = new PrivateContext;
}

Context::Context(const Context& other): d(other.d)
{

}

Context::~Context()
{

}

Context& Context::operator=(const Context& other)
{
	this->d = other.d;
	return *this;
}

const QString & Context::prefix() const
{
	return d->_prefix;
}

void Context::setPrefix(const QString & prefix)
{
	d->_prefix = prefix;
}

void Context::setFilename(const QString& filename)
{
	d->_filename = filename;
}

const QString& Context::filename() const
{
	return d->_filename;
}

ContentView3::FilePtr Context::fileStrongRef() const
{
	return d->_file.toStrongRef();
}

ContentView3::FilePtrWeak Context::file() const
{
	return d->_file;
}

void Context::setFile(ContentView3::FilePtrWeak file)
{
	d->_file = file;
}

void Context::addContext(CodeCompletion::ContextType* context)
{
	if (d->_context.contains(context->type()))
	{
		delete d->_context.value(context->type());
		d->_context.remove(context->type());
	}
	d->_context.insert(context->type(), context);
}

void Context::clear()
{
	qDeleteAll(d->_context.values());
	d->_context.clear();
}

QList< ContextType* > Context::contexts()
{
	return d->_context.values();
}

ContextType* Context::context(const QString& type) const
{
	return d->_context.value(type);
}

bool Context::operator==(const Context& other) const
{
	if (d->_filename != other.d->_filename)
		return false;

	if (d->_context.count() != other.d->_context.count())
		return false;

	foreach(ContextType * contextType, d->_context)
	{
		bool finded = false;
		foreach(ContextType * otherContextType, other.d->_context)
		{
			if (contextType->operator==(*otherContextType))
			{
				finded = true;
				break;
			}
		}

		if (!finded)
		{
			return false;
		}
	}

	return true;
}

bool Context::operator!=(const Context & other) const
{
	return !(*this == other);
}
}
