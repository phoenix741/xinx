/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

#ifndef SNIPETCATEGORY_H
#define SNIPETCATEGORY_H

/*!
 * A snipet category is used to organize your snipet. A category can containt
 * both sub-category and snipet element.
 *
 * A category is represent by it's name and a little description. In the disk
 * a category is saved as a directory that's contains snipet and a file named
 * \e category.xml. This file contains meta-data of the category, has real
 * name, the description, and a script, that tell if the category is available
 * for the project, or for the stylesheet.
 */
class SnipetCategory {
public:
	SnipetCategory();
	SnipetCategory( const QString & name );

	const QString & id();
	/*!
	 * Set the id of the category. The id is initialize with the name of the
	 * category, but is keeped when renaming. This is use internally and for
	 * the directory structure.
	 * Use this method carrefully
	 * \sa id();
	 */
	void setId( const QString & value );

	const QString & name();
	void setName( const QString & value );

	const QString & description();
	void setDescription( const QString & value );

	const QString & availableScript() const;
	void setAvailableScript( const QString & value );

	const QList<SnipetCategory> & subCategory() const;
	QList<SnipetCategory> subCategory();

	const QList<SnipetElement> & elements() const;
	QList<SnipetElement> & elements() const;
};

#endif // SNIPETCATEGORY_H
