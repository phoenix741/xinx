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

#ifndef XINXSINGLETON_H
#define XINXSINGLETON_H

// Qt header
#include <QObject>

// Xinx header
#include "xinxcore.h"

template<class T> class LIBEXPORT XinxLibSingleton : public QObject
{
public:
	~XinxLibSingleton()
	{
		
	}

	static T * self()
	{
		if (_self == NULL)
		{
			_self = new T;
			_self->initialisation();
		}

		return _self;
	}

protected:
	XinxLibSingleton()
	{
		XINXStaticDeleter::self()->addObject(this);
	}

	virtual void initialisation()
	{

	}

private:
	static T * _self;
};

template <typename T> T * XinxLibSingleton<T>::_self = NULL;

template<class T> class XinxSingleton : public QObject
{
public:
	~XinxSingleton()
	{

	}

	static T * self()
	{
		if (_self == NULL)
		{
			_self = new T;
			_self->initialisation();
		}

		return _self;
	}

protected:
	XinxSingleton()
	{
		XINXStaticDeleter::self()->addObject(this);
	}

	virtual void initialisation()
	{

	}

private:
	static T * _self;
};

template <typename T> T * XinxSingleton<T>::_self = NULL;

#endif // XINXSINGLETON_H
