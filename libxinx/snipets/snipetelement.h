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

#ifndef _SNIPETELEMENT_H_
#define _SNIPETELEMENT_H_
#pragma once

/*!
 * This class describe a snipet in XINX. A snipet is a piece of
 * code added in your current developpent on the demand.
 *
 * The text can containt parameter : %1 %2 %3 %4 ... and script
 * with balise <? and ?>
 */
class SnipetElement {
public:
	/*!
	 * Init some member with default value
	 */
	SnipetElement();

	/*!
	 * Name of the snipet. Use when show a list of snipet.
	 * \return Name of the template.
	 * \sa setName()
	 */
	const QString & name() const;
	/*!
	 * Set the name of the snipet.
	 * \param name New name of the template.
	 * \sa name()
	 */
	void setName( const QString & value );

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
	void setKey( const QString & value );

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
	 * Return if the snipet must be automatically called when
	 * user write the text.
	 * \return true if the snipet must be automatique.
	 * \sa setAutomatiqueCall()
	 */
	bool isAutomatiqueCall() const;
	/*!
	 * Set the value to true to call the snipet automatically.
	 * \param value Set true if the snipet must be called when user hit the key
	 * \sa isAutomatiqueCall()
	 */
	void setAutomatiqueCall( bool value );

	/*!
	 * List of the extentions that can be used with this snipet.
	 * \return List of snipet.
	 * \sa setExtentionsList()
	 */
	const QStringList & extentionsList() const;
	/*!
	 * Set the list of the extentions that can be used with this snipet.
	 * \return List of snipet.
	 * \sa extentionsList()
	 */
	void setExtentionsList( const QStringList & value );

	/*!
	 * This method retrieve the text used to add. This text contains %1,
	 * %2, %3. Script can be used with <? and ?>
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
	 * Define the script used to determine if the snipet can be
	 * enable or not.
	 * \return the script
	 * \sa setScript()
	 */
	const QString & script() const;
	/*!
	 * Set the script to value.
	 * \param value the value contains the script
	 * \sa script()
	 */
	void setScript( const QString & value );

	/*!
	 * List of parameters of the template. This list is a 'pointer',
	 * the value can be modified.
	 * \return a list of parameter.
	 */
	QList< QPair<QString,QString> > & params();
	const QList< QPair<QString,QString> > & params() const;

	bool operator==( const Snipet & s ) const;
	bool operator<( const Snipet & s ) const;
private:
	/*!
	 * Save the content of member in the XML document structure.
	 * It's can be used by ScriptManager to backup all Snipet, or SnipetList
	 * to import/export script.
	 *
	 * The Snipet will have this structure :
	 *
	 * <Snipet [ category="Buttons Widget" ]
	 *         key="delete"
	 *         icon=":/...../.......png"
	 *         [ type="xsl" ]
	 *         name="Delete Button"
	 *         automatique="true/false">
	 * 	<Description>.............</Description>
	 *  <Category name="toto">
	 * 	  <Category name="titi"/>
	 *  </Category>
	 *  <Text>................</Text>
	 *  <Param name="....." defaultValue="......"/>
	 *  <Param name="....." defaultValue="......"/>
	 *  <Param name="....." defaultValue="......"/>
	 *  <Script>............</Script>
	 *  <Extention value="........."/>
	 *  <Extention value="........."/>
	 *  <Extention value="........."/>
* </Snipet>
	 * \sa loadFromDom()
	 */
	QDomElement saveToDom( QDomDocument & document ) const;
	/*!
	 * Load the content of an XML document structure in member.
	 * It's can be used by ScriptManager to backup all Snipet, or SnipetList
	 * to import/export script.
	 * \sa saveToDom()
	 */
	void loadFromDom( const QDomElement & element );
	friend class SnipetList;
	friend class SnipetManager;

	QString m_name, m_description, m_key, m_icon, m_text, m_script;
	QStringList m_extentionsList;
	bool m_isAutomatiqueCall;
	QList< QPair<QString,QString> > m_params;
};

#endif /* _SNIPETELEMENT_H_ */