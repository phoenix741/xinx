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

#ifndef __SNIPET_H__
#define __SNIPET_H__
#pragma once

// Qt header
#include <QString>
#include <QStringList>
#include <QMetaType>

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
	 * Name of the template. Use when show a list of snipet.
	 * \return Name of the template.
	 * \sa setName()
	 */
	const QString & name() const;
	
	/*!
	 * Set the name of the template.
	 * \param name New name of the template.
	 * \sa name()
	 */
	void setName( const QString & name );
	
	/*!
	 * Get the key to use when complete with a snipet.
	 * \return the key to use.
	 * \sa setKey()
	 */
	const QString & key() const;
	 
	/*!
	 * Set the key to use when complete a snipet.
	 * \param The new Key to use.
	 * \sa key();
	 */
	void setKey( const QString & key );
	
	/*!
	 * Type of template. The type of template help to filter.
	 * \return Type of template.
	 * \sa setType()
	 */
	const QString & type() const;
	 
	/*!
	 * Set the type of template.
	 * \param type new type of template.
	 * \sa type()
	 */
	void setType( const QString & type );
	
	/*!
	 * Description of the template. 
	 * \return the description of the template.
	 * \sa setDescription()
	 */
	const QString & description() const;
	
	/*!
	 * Set the description of the template.
	 * \param name New description of the template.
	 * \sa description()
	 */
	void setDescription( const QString & description );
	
	/*!
	 * Return the category to use to store template. The category is used
	 * to class the template in different way.
	 * \return the category to use.
	 * \sa setCategory(), categoryChange()
	 */
	const QString & category() const;
	
	/*!
	 * Set the category used to store the template.
	 * \param value The new category used to store the template.
	 * \sa category(), categoryChange()
	 */
	void setCategory( const QString & category );

	/*!
	 * Icon to show link in the list of template. The icon can be 
	 * a resource.
	 * \return the link of the icon to use.
	 * \sa setIcon()
	 */
	const QString & icon() const;
	
	/*!
	 * Set the icon of the template.
	 * \param name New icon of the template.
	 * \sa icon()
	 */
	void setIcon( const QString & icon );
	
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
	 * List of parameters of the template. This list is a 'pointer', 
	 * the value can be modified. 
	 * \return a list of parameter.
	 */
	QStringList & params();
	const QStringList & params() const;
	
	bool operator==( const Snipet & s ) const;
	bool operator<( const Snipet & s ) const;
private:
	QString m_text, m_name, m_description, m_icon, m_category, m_key, m_type;
	QStringList m_params;
};	


Q_DECLARE_METATYPE(Snipet);

#endif // __SNIPET_H__
