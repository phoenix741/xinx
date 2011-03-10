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

#pragma once
#ifndef __XINXCORE_H__
#define __XINXCORE_H__

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QObject>
#include <QObjectCleanupHandler>

/*!
 * \mainpage XINX Library
 *
 * This page is used to help you develop on XINX (developping new plugins, developping
 * new script, developping a patch for a version of XINX)
 *
 * You will find a documentation about
 *  - \ref XinxPlugin
 *  - Documentation of all classes of the framework. 
 */

/*!
 * All static class add self to the static deleter who delete all static class when the application is stopped
 * (if necessary).
 */
class LIBEXPORT XINXStaticDeleter : public QObjectCleanupHandler
{
	Q_OBJECT
public:
	/*! Intialize default value of object. */
	XINXStaticDeleter();
	/*! Destruct the global object. */
	~XINXStaticDeleter();

	/*!
	 * \deprecated
	 * Add an object to the static deleter class. This function is replaced by
	 * the add() method of the QObjectCleanupHandler.
	 */
	void addObject(QObject * object)
	{
		add(object);
	};

	/*!
	 * Retrieve the global object created at the first call of the this method.
	 */
	static XINXStaticDeleter * self();
private:
	static XINXStaticDeleter * s_self;
};

/*!
 * Number of maximum recent file that can be opened
 */
#define MAXRECENTFILES 10

/*!
* Number of maximum project file that can be opened
*/
#define MAXRECENTPROJECTS 10

/*!
 * Default project options filtre
 */
#define DEFAULT_PROJECT_FILTRE_OPTIONS QDir::AllDirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot

/*!
 * Template used to delete all the object of list with an iterator. Instead of using C++
 * delete, this method called the deleteLater method of the QObject class, if the object
 * is a QObject.
 * Objects are delete when application process messages.
 * \param begin First iterator to delete
 * \param end Last iterator to delete
 * \sa qDeleteAllLater(const Container &c)
 */
template <typename ForwardIterator>
void qDeleteAllLater(ForwardIterator begin, ForwardIterator end)
{
	while (begin != end)
	{
		if (dynamic_cast<QObject*>(*begin))
			dynamic_cast<QObject*>(*begin)->deleteLater();
		else
			delete *begin;
		++begin;
	}
}

/*!
 * Delete all the object of a list later
 * \param c Container where we must delete all object.
 * \sa qDeleteAllLater(ForwardIterator begin, ForwardIterator end)
 */
template <typename Container>
inline void qDeleteAllLater(const Container &c)
{
	qDeleteAllLater(c.begin(), c.end());
}


#endif // __XINXCORE_H__
