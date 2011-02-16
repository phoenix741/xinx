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

#ifndef CORE_JAVASCRIPT_CONTEXT_H
#define CORE_JAVASCRIPT_CONTEXT_H

#include <codecompletion/context.h>
#include <QHash>
#include <QStack>

#define JS_CONTEXT_TYPE "JS"

namespace Core
{

namespace JavaScript
{

class JsContextType : public CodeCompletion::ContextType
{
public:
	enum Position
	{
		NONE				= 0,
		LONG_COMMENT		= 1, //!< Start with /* and end with */
		SHORT_COMMENT		= 2, //!< Start with // ...
		FUNCTION_PARAMS		= 3, //!< Edit the parameters of function
		FUNCTION_CONTENT	= 4  //!< Edit the content of the function
	};

	JsContextType();
	virtual ~JsContextType();

	Position position();
	void setPosition(Position value);

	const QString & functionName() const;
	void setFunctionName(const QString & value);

	virtual QString type() const;
	virtual bool operator==(const ContextType & other) const;
private:
	Position _position;
	QString _function_name;
};

}

}

#endif // CORE_JAVASCRIPT_CONTEXT_H
