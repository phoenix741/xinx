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
#ifndef __SNIPETLIST_H__
#define __SNIPETLIST_H__

// Xinx header
#include <core/lib-config.h>
#include <snipets/snipet.h>

// Qt header
#include <QObject>
#include <QString>

/*!
 * The snipet list contains all snipet defined in XINX. This snipet is stored in a
 * file in XML format.
 */
class LIBEXPORT SnipetList : public QList<Snipet>
{
public:
	SnipetList();
	virtual ~SnipetList();

	int indexOf(const QString & key, int from = 0) const;

	/*!
	 * Save the snipet list into a file.
	 * \param filename The filename where we want save snipet.
	 * \throw SnipetListException
	 */
	void saveToFile(const QString & filename);
	/*!
	 * Load the snipet from a file.
	 * \param filename The filename used to load snipet.
	 * \throw SnipetListException
	 */
	void loadFromFile(const QString & filename);

	/*!
	 * List of categories used by templates
	 * \return List of template.
	 */
	QStringList categories() const;

	/*!
	 * List all the snipet of a the category \e category
	 */
	SnipetList categorie(const QString & category);
private:
};

#endif // __SNIPETLIST_H__
