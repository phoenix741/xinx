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
#include <QObject>
#include <QString>
#include <QIcon>
#include <QReadWriteLock>

class PrivateFileContentElement;

class FileContentParser {
public:
	virtual ~FileContentParser();
	
	virtual void loadFromContent( const QString & content ) = 0;
	virtual void loadFromFile( const QString & filename ) = 0;
};

/*!
 * The file content element is the base element of a file content structure. Javascript file content and
 * XSL file content is based on a structure file content.
 */
class FileContentElement : public QObject {
	Q_OBJECT
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
	virtual const QString & name() const;
	virtual QString displayName() const;
	virtual QString displayTips() const;
	/*!
	 * Return the line where the element is.
	 * \return the line of the element.
	 */
	int line();
	/*!
	 * File name where is the element. If the file is unknown, Null string is returned.
	 */
	const QString & filename() const;
	
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
	
	/*!
	 * Test if an element is equals to another FileContentElement \e element. If the element is equals 
	 * the method return \e true.
	 */
	virtual bool equals( FileContentElement * element );
	
	/*!
	 * Copy the contents of an FileContentElement \e element to this.
	 */
	virtual void copyFrom( FileContentElement * element );
	
	/*!
	 * Icon used for show how the element must be.
	 */
	virtual QIcon icon() const;

	QReadWriteLock & locker();
protected:
	/*!
	 * Function to change the name of the element. This function is only accessible
	 * by children.
	 * \param name New name of the element.
	 */
	void setName( const QString & name );
	/*!
	 * Function to change the line of the element. This function is only accessible
	 * by children of the class.
	 * \param line Line of an element.
	 */
	void setLine( int line );
	/*!
	 * Function to change the filename of the element. This function is only accessible by
	 * children of the class.
	 */
	void setFilename( const QString & filename );
	
	/*!
	 * Remove an element from the child list. The signal \e aboutToRemove() and \e removed() is emited
	 * while deleting the line \e index.
	 */
	void remove( int index );
	/*!
	 * Append an element at the end of the list. Signals, \e aboutToAdd() and \e added() is emited while
	 * adding the element \e element. If the element is already in the list, the elements is not added but 
	 * updated and the signal \e updated() is emited.
	 */
	FileContentElement * append( FileContentElement * element );
	/*!
	 * Remove all elements from the list.
	 */
	void clear();
	
	/*!
	 * Search if the list contains the element \e element. The test is made using method \e equals().
	 * If the list contains the element, a pointer to the element is returned, else NULL, is returned.
	 */
	FileContentElement * contains( FileContentElement * element );
	
	/*!
	 * Mark this element to be deleted by method \e removeMarkedDeleted().
	 */
	void markDeleted();
	/*!
	 * Mark the element to be keeped and ignored by \e removeMarkedDeleted().
	 */
	void markKeeped();
	/*!
	 * Mark all child element to be deleted by method \e removeMarkedDeleted().
	 */
	void markAllDeleted();
	/*!
	 * Remove from the child list all elements marked to be deleted by \e markDeleted().
	 */
	void removeMarkedDeleted();

	/*!
	 * Change \e value to false if append and remove don't emit signals
	 */
	void setFlagEmit( bool value );
signals:
	void updated( FileContentElement * element );
	void aboutToRemove( FileContentElement * element );
	void removed();
	void aboutToAdd( FileContentElement * element, int row );
	void added();
private:
	PrivateFileContentElement * d;
	friend class PrivateFileContentElement;
};

bool FileContentElementModelObjListSort( FileContentElement * d1, FileContentElement * d2 );

#endif /*FILECONTENTSTRUCTURE_H_*/
