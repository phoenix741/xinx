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

#ifndef CODECOMPLETION_CONTEXT_H
#define CODECOMPLETION_CONTEXT_H

// Xinx header
#include <core/lib-config.h>
#include <contentview3/definitions.h>

// Qt header
#include <QString>
#include <QList>
#include <QExplicitlySharedDataPointer>
#include <QObject>

namespace CodeCompletion
{

class PrivateContext;

class LIBEXPORT ContextType
{
public:
	ContextType();
	virtual ~ContextType();

	virtual QString type() const = 0;
	virtual bool operator==(const ContextType & other) const;
private:
	Q_DISABLE_COPY(ContextType)
};

class LIBEXPORT Context
{
public:
	Context();
	Context(const Context & other);
	~Context();

	const QString & filename() const;
	void setFilename(const QString & filename);

	const QString & prefix() const;
	void setPrefix(const QString & prefix);

	ContentView3::FilePtr fileStrongRef() const;
	ContentView3::FilePtrWeak file() const;
	void setFile(ContentView3::FilePtrWeak file);

	ContextType* context(const QString & type) const;
	QList<ContextType*> contexts();
	void addContext(ContextType * context);
	void clear();

	Context & operator=(const Context & other);
	bool operator==(const Context & other) const;
	bool operator!=(const Context & other) const;
private:
	QExplicitlySharedDataPointer<PrivateContext> d;
};

}

#endif // CODECOMPLETION_CONTEXT_H
