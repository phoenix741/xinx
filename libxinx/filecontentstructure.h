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

// Xinx header
#include "xinxcore.h"
#include <exceptions.h>

// Qt header
#include <QObject>
#include <QString>
#include <QIcon>

class PrivateFileContentElement;

/*!
 * Exception throw when the model can't be updated.
 */
class XINX_EXPORT FileContentException : public XinxException {
public:
	/*!
	 * Create the exception with a message and a line.
	 * \param message Error of the exception.
	 * \param line Line where the error is.
	 */
	FileContentException( QString message, int line, int column );

	/*!
	 * Return the line where the error is.
	 * \return The line of the error.
	 */
	int getLine() const;
	
	/*! 
	 * Return the column where the error is.
	 * \return the column of the error.
	 */
	int getColumn() const;
private:
	int m_line, m_column;
};

/*!
 * The file content parser is an interface for the parser.
 */
class XINX_EXPORT FileContentParser {
public:
	virtual ~FileContentParser() {};
	
	/*!
	 * Construct elements from \e content.
	 */
	virtual void loadFromContent( const QString & content ) = 0;
	/*!
	 * Construct elements from \e filename.
	 */
	virtual void loadFromFile( const QString & filename ) = 0;
	
	//! Load from file \e filename but really charged it when needed.
	virtual void loadFromFileDelayed( const QString & filename ) = 0;
	
	//! Tell if the file is loaded or not.
	virtual bool isLoaded() = 0;
};

/*!
 * The file content element is the base element of a file content structure. Javascript file content and
 * XSL file content is based on a structure file content.
 */
class XINX_EXPORT FileContentElement : public QObject {
	Q_OBJECT
public:
	/*!
	 * Create an element with a name and the line where the element is.
	 * \param name Name of the element.
	 * \param line Line where the element is.
	 */
	FileContentElement( FileContentElement * parent, const QString & name, int line );
	/*! Destroy the element. */
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
	
	/*! Copy the contents of an FileContentElement \e element to this. */
	virtual void copyFrom( FileContentElement * element );
	
	/*! Icon used for show how the element must be. */
	virtual QIcon icon() const;

	/*!
	 * Function to change the name of the element. 
	 * \param name New name of the element.
	 */
	void setName( const QString & name );
	/*!
	 * Function to change the line of the element. 
	 * \param line Line of an element.
	 */
	void setLine( int line );
	/*! Function to change the filename of the element. */
	void setFilename( const QString & filename );
	
protected:
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
	/*! Remove all elements from the list. */
	void clear();
	
	/*!
	 * Search if the list contains the element \e element. The test is made using method \e equals().
	 * If the list contains the element, a pointer to the element is returned, else NULL, is returned.
	 */
	FileContentElement * contains( FileContentElement * element );
	
	/*! Mark this element to be deleted by method \e removeMarkedDeleted(). */
	void markDeleted();
	/*! Mark the element to be keeped and ignored by \e removeMarkedDeleted(). */
	void markKeeped();
	/*! Mark all child element to be deleted by method \e removeMarkedDeleted(). */
	void markAllDeleted();
	/*! Remove from the child list all elements marked to be deleted by \e markDeleted(). */
	void removeMarkedDeleted();
signals:
	/*! Signal emited when an element is modified */
	void updated( FileContentElement * element );
	/*! Signal emited when an element is about to be removed. */
	void aboutToRemove( FileContentElement * element );
	/*! Signal emited when the element is removed. */
	void removed();
	/*! Signal emited when an element is about to be added. */
	void aboutToAdd( FileContentElement * element, int row );
	/*! Signal emited when an element is added. */
	void added();
private:
	PrivateFileContentElement * d;
	friend class PrivateFileContentElement;
};

/*!
 * This class is used to store a  list of \e FileContentElement. This list is updated when new
 * element is added to the list, or when element are removed.
 * This list can be used to represent the File contente tree in a list structure for the completion.
 */
class XINX_EXPORT FileContentElementList : public QObject {
	Q_OBJECT
public:
	/*!  Create a list of \e FileContentElement with the root element \e root. */
	FileContentElementList( FileContentElement * root );
	/*! Destroy the list but not the tree */
	virtual ~FileContentElementList();
	
	/*! Return the list of \e FileContentElement. */
	const QList<FileContentElement*> & list() const;
signals:
	/*! Signal emited when an element is about to be added. */
	void aboutToAdd( int row );
	/*! Signal emited when an element is added. */
	void added();
	/*! Signal emited when an element is about to be removed. */
	void aboutToRemove( int row );
	/*! Signal emited when the element is removed. */
	void removed();
	/*! Signal emited when the list is refreshed */
	void reset();
private slots:
	void refreshList();
	void addElement( FileContentElement * element, int row );
	void removeElement( FileContentElement * element );
private:
	void addElement( FileContentElement * element );
	bool contains( FileContentElement * data );
	void refreshRecursive( FileContentElement * data );
	
	QList<FileContentElement*> m_list;
	QStringList m_files;
	
	FileContentElement * m_root;
};

XINX_EXPORT bool FileContentElementModelObjListSort( FileContentElement * d1, FileContentElement * d2 );

#endif /*FILECONTENTSTRUCTURE_H_*/
