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
#include <QMutexLocker>
#include <QDebug>

// Std header
#include <iostream>
#include <typeinfo>

// Xinx header
#include "xinxcore.h"

template<class T> class LIBEXPORT XinxLibSingleton : public QObject
{
public:
	~XinxLibSingleton()
	{
		_self = 0;
	}

	static T * self()
	{
		if (_self == NULL)
		{
			qDebug() << "Lock singleton of type " << typeid(T).name();
			QMutexLocker locker(&_self_mutex);
			if (_self == NULL)
			{
				qDebug() << "Pre-create singleton of type " << typeid(T).name();
				_self = new T;
				qDebug() << "Post-create singleton of type " << typeid(T).name();
				_self->initialisation();
				qDebug() << "Post-init singleton of type " << typeid(T).name();
			}
		}

		return _self;
	}

	static T * directSelf()
	{
		return _self;
	}
protected:
	XinxLibSingleton()
	{
		setObjectName(typeid(T).name());
		XINXStaticDeleter::self()->addObject(this);
	}

	virtual void initialisation()
	{

	}

private:
	static T * _self;
	static QMutex _self_mutex;
};

template <typename T> T * XinxLibSingleton<T>::_self = NULL;
template <typename T> QMutex XinxLibSingleton<T>::_self_mutex;

template<class T> class XinxSingleton : public QObject
{
public:
	~XinxSingleton()
	{
		_self = 0;
	}

	static T * self()
	{
		if (_self == NULL)
		{
			qDebug() << "Lock singleton of type " << typeid(T).name();
			QMutexLocker locker(&_self_mutex);
			if (_self == NULL)
			{
				qDebug() << "Pre-create singleton of type " << typeid(T).name();
				_self = new T;
				qDebug() << "Post-create singleton of type " << typeid(T).name();
				_self->initialisation();
				qDebug() << "Post-init singleton of type " << typeid(T).name();
			}
		}

		return _self;
	}

	static T * directSelf()
	{
		return _self;
	}
protected:
	XinxSingleton()
	{
		setObjectName(typeid(T).name());
		XINXStaticDeleter::self()->addObject(this);
	}

	virtual void initialisation()
	{

	}

private:
	static T * _self;
	static QMutex _self_mutex;
};

template <typename T> T * XinxSingleton<T>::_self = NULL;
template <typename T> QMutex XinxSingleton<T>::_self_mutex;

#endif // XINXSINGLETON_H
