/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#ifndef FILECONTENTSTRUCTURE_H_
#define FILECONTENTSTRUCTURE_H_

// Qt header
#include <QString>

class PrivateFileContentElement;

/*!
 * The file content element is the base element of a file content structure. Javascript file content and
 * XSL file content is based on a structure file content.
 */
class FileContentElement {
public:
	/*!
	 * Create an element with a name and the line where the element is.
	 * \param name Name of the element.
	 * \param line Line where the element is.
	 */
	FileContentElement( FileContentElement * parent, const QString & name, int line );
	/*!
	 * Destroy the element.
	 */
	virtual ~FileContentElement();
	
	/*!
	 * Name of the element.
	 * \return Return the name of the element.
	 */
	virtual const QString & name();
	/*!
	 * Return the line where the element is.
	 * \return the line of the element.
	 */
	int line();
	
	/*!
	 * Count the number of child of the element.
	 * \return the number of child.
	 */
	virtual int rowCount();
	/*!
	 * Return the index-th child element of the object. Return null if i can't.
	 * \param index Index of the child element.
	 * \return The child element.
	 */
	virtual FileContentElement * element( int index );
	
	/*!
	 * Return the parent of this element.
	 * \return The parent of this element.
	 */
	virtual FileContentElement * parent();
	 
	/*!
	 * Return the row index in the parent list.
	 * \return The index of the element
	 */ 
	virtual int row();
protected:
	/*!
	 * Function to change the name of the element. This function is only accessible
	 * by children.
	 * \param name New name of the element.
	 */
	void setName( const QString & name );
	/*!
	 * Function to change the line of the element. This function is only accessible
	 * by childre of the class.
	 * \param line Line of an element.
	 */
	void setLine( int line );
private:
	PrivateFileContentElement * d;
	friend class PrivateFileContentElement;
};

#endif /*FILECONTENTSTRUCTURE_H_*/
