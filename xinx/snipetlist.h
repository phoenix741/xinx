/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

#ifndef __SNIPETLIST_H__
#define __SNIPETLIST_H__

// Xinx header
#include <exceptions.h>
#include "snipet.h"

// Qt header
#include <QObject>
#include <QString>

/*!
 * \class SnipetListException
 * The snipet list exception, is throw when Snipet list load or save file.
 */
class SnipetListException : public XinxException {
public:
	/*!
	 * Exception throw by the Snipet list.
	 * \param message Message of the error.
	 */
	SnipetListException( const QString & message );
};


/*!
 * The snipet list contains all snipet defined in XINX. This snipet is stored in a
 * file in XML format.
 */
class SnipetList : public QList<Snipet> {
public:
	SnipetList();
	virtual ~SnipetList();

	int indexOf( const QString & key, int from = 0 ) const;

	/*!
	 * Save the snipet list into a file.
	 * \param filename The filename where we want save snipet.
	 * \throw SnipetListException
	 */
	void saveToFile( const QString & filename );
	/*!
	 * Load the snipet from a file.
	 * \param filename The filename used to load snipet.
	 * \throw SnipetListException
	 */
	void loadFromFile( const QString & filename );

	/*!
	 * List of categories used by templates
	 * \return List of template.
	 */
	QStringList categories() const;

	/*!
	 * List all the snipet of a the category \e category
	 */
	SnipetList categorie( const QString & category );
private:
};

class SnipetListManager : public QObject {
	Q_OBJECT
public:
	SnipetListManager();
	~SnipetListManager();

	static SnipetListManager * self();

	const SnipetList & snipets() const;
	void setSnipets( const SnipetList & list );
public slots:
	void loadFromSnipetFile();
	void saveToSnipetFile();
signals:
	void listChanged();
private:
	static SnipetListManager * s_self;

	SnipetList m_snipets;
};

#endif // __SNIPETLIST_H__
