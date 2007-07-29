/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#ifndef __SNIPET_H__
#define __SNIPET_H__

#include <QString>
#include <QStringList>

class PrivateSnipet;

/*!
 * The snipet is a sort of template but it's just called snipet ;)
 * So, this class describe the template with a text and params. 
 *
 * The text contains %1, %2, ... this data will be replaced by params.
 * The params of this class, is just names. 
 */
class Snipet {
public:
	/*!
	 * Create an empty template.
	 */
	Snipet();
	/*!
	 * Destroy the template 
	 */ 
	virtual ~Snipet();
	
	/*!
	 * This method retrieve the text used to add. This text contains %1,
	 * %2, %3.
	 * \return the text to add with '%1', ...
	 * \sa setText()
	 */
	const QString & text() const;
	/*!
	 * This method set the text of the snipet.
	 * \param value The new text of the snipet.
	 * \sa text()
	 */
	void setText( const QString & value );
	
	/*!
	 * List of parameter of the snipet. This list is a 'pointer', the value can be modified.
	 * \return a list of parameter.
	 */
	QStringList & params();
private:
	PrivateSnipet * d;
	friend class PrivateSnipet;
};

#endif // __SNIPET_H__
