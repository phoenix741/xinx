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

/*!
 * \file contentviewparser.h
 * \brief Contains the element for manage the parser.
 */

#ifndef __CONTENTVIEWPARSER_H__
#define __CONTENTVIEWPARSER_H__

// Xinx header
#include <core/exceptions.h>

// Qt header
#include <QCoreApplication>

class ContentViewNode;

/*!
 * \class ContentViewException
 * \brief Exception throw when the model can't be updated.
 */
class  ContentViewException : public XinxException {
public:
	/*!
	 * Create the exception with a message and a line.
	 * \param message Error of the exception.
	 * \param line Line where the error is.
	 */
	ContentViewException( QString message, QString filename, int line, int column = 0 );

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

	/*!
	 * Return the filename where find the error
	 * \return the filename of the exception.
	 */
private:
	int m_line, m_column;
	QString m_filename;
};


/*!
 * \class ContentViewParser
 * \brief The content view parser create the content view tree.
 *
 * The parser read the content view tree and return the root element :
 * a content view node. The parser must be sub-classed and must redefine
 * the method \e loadFromDevice().
 *
 */
class ContentViewParser {
	Q_DECLARE_TR_FUNCTIONS(ContentViewParser);
public:
	/*!
	 * Create a content view parser used to fill the content of a node.
	 * If \e autoDelete is se to true, the object will be self destroyed after
	 * the content of the node read.
	 */
	ContentViewParser( bool autoDelete = false );
	virtual ~ContentViewParser();

	/*! Load the content of the givent device and return true if sucessfully loaded */
	virtual void loadFromMember();
	/*! Load the content of the device. and return the true if sucessfully loaded */
	virtual void loadFromDevice( ContentViewNode * rootNode, QIODevice * device );
	/*! Construct elements from \e content. */
	virtual void loadFromContent( ContentViewNode * rootNode, const QByteArray & content );
	/*! Construct elements from \e filename. */
	virtual void loadFromFile( ContentViewNode * rootNode, const QString & filename );

	/*! Set the root node */
	void setRootNode( ContentViewNode * node );
	/*! Return the root node */
	ContentViewNode * rootNode() const;

	/*! Set the filename */
	void setFilename( const QString & filename );
	/*! Set the device */
	void setInputDevice( QIODevice * device );
	/*! Return the device */
	QIODevice * inputDevice() const;

	/*! Set the Id used on attach and detach */
	void setAttachId( unsigned long value );
	/*! Return the id */
	unsigned long attachId();

	/*! Change the autodelete member to \e value. */
	void setAutoDelete( bool value );
	/*! Return wethere the object will be auto deleted */
	bool isAutoDelete() const;

	/*! Set a decalage when attach a node to the parent */
	void setDecalage( int line );
	/*! Return the current decalage */
	int decalage() const;
protected:
	/*!
	 * Method to redefine to implement the parser. The device, and root element
	 * to use can be find in method \e inputDevice() and \e rootNode()
	 */
	virtual void loadFromDeviceImpl() = 0;

	/*!
	 * Create an ContentViewNode (using ContentViewCache) and attach it to the node
	 */
	void createContentViewNode( ContentViewNode * parent, const QString & filename );
	/*!
	 * Attach the node \e child to \e parent if this node isn't already attached.
	 */
	ContentViewNode * attachNode( ContentViewNode * parent, ContentViewNode * child );

	/*!
	 * Load all child from the given \e rootNode for future detach
	 */
	void loadAttachedNode( ContentViewNode * rootNode );
	//! Detach all node again in the list
	void detachAttachedNode();
	//! Remove \e rootNode from the attachedNodeList
	inline void removeAttachedNode( ContentViewNode * rootNode );
	//! Remove all node in the list
	inline void removeAttachedNodes() { m_attachedNode.clear(); }

	//! Return the location (absolute path) of the filename, with the help of the \e parent node
	QString locationOf( ContentViewNode * parent, const QString & filename );
private:
	QList< QPair<ContentViewNode*,ContentViewNode*> > m_attachedNode;
	bool m_autoDelete, m_alreadyRunning;
	ContentViewNode * m_rootNode;
	QIODevice * m_device;
	unsigned long m_id, m_decaledLine;
};

#endif /* __CONTENTVIEWPARSER_H__ */
