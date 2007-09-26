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
 
#ifndef _EDITORCOMPLETION_H_
#define _EDITORCOMPLETION_H_

// Xinx header
#include "exceptions.h"

// Qt header
#include <QList>
#include <QStringList>

class QDomElement;

/*!
 * Exception created when the file loaded isn't a Completion File (not a XML file
 * or have a wrong format).
 */
class NotCompletionFileException : public XinxException {
public:
  /*!
   * Constructor of the exception, show a message on the console, when exception is
   * declanched.
   * \param message Message to be show on the console.
   */
  NotCompletionFileException( const QString & message ) : XinxException( message ) {
  }
};

/*!
 * Represent an XML attribute on the competion file.
 * \code
 *   <root>
 *   	<balise  attribute1="value1" attribute2="value2">text </balise>
 *   	<balise2 attribute3="value1" attribute4="value2">text2</balise>
 *   </root>
 * \endcode
 *
 * In the code above, the text \e attribute1 and \e attribute2 represents the name of the completion
 * XML attribute. \e value1 and \e value2 represent the \e Values in the class. The class can have
 * multiple value. In this case user can complete on the list of value. If a default value is proposed
 * when user complete on the attribute, the default value is automaticaly written.
 *
 * If the attribute is default, it will be written automaticaly on validation of the balise namec while 
 * completion.
 */
class CompletionXMLAttribute {
public:
	/*!
	 * Constructor of the class. Load from XML the content of the completion. 
	 * \param node XML Node where element must be read.
	 */
	CompletionXMLAttribute( const QDomElement & node );
	/*!
	 * Destructor of the class
	 */
	~CompletionXMLAttribute();

	/*!
	 * Name of the attribute.
	 * \return The name of the attribute
	 */
	const QString & name() const { return m_name; };
	/*!
	 * If the attribute is the default attribute of the balise, the attribute is automaticaly
	 * wrote on editor when the balise is completed.
	 * \return True if the attribute is the default value
	 */
	bool isDefault() const { return m_isDefault; };
	
	/*!
	 * Return the list of possible value for the attribute. The user can complete the value with
	 * the list.
	 * \return List of authorized value.
	 * \sa defaultValue()
	 */
	const QStringList & values() const { return m_values; };
	/*!
	 * If there is a default value, this method return the index in the liste above.
	 * \return The index of default value, else return -1
	 * \sa values()
	 */
	int defaultValue() const { return m_defaultValue; };
private:
	QString m_name;
	bool m_isDefault;
	
	QStringList m_values;
	int m_defaultValue;
};

/*!
 * Class to complete on Balise of a XML file.
 * \code
 *   <root>
 *   	<balise  attribute1="value1" attribute2="value2">text </balise>
 *   	<balise2 attribute3="value1" attribute4="value2">text2</balise>
 *   </root>
 * \endcode
 * In the code above \e balise and \e balise2 is two balises on a XML Document.
 * The user can complete on a choice of predifined balise.
 *
 * A balise is under a category (XSL or HTML), and contains one or more attribute.
 * A balise can contains attribute.
 * 
 * \code
 *   <xsl:choose>
 *	 	<xsl:when test=""/>
 * 		<xsl:otherwise/>
 *   </xsl:choose>
 * \endcode
 *
 * By example, if \e xsl:choose contains two balise : \e xsl:when and \e xsl:otherwise, when
 * user complete on \e xsl:choose, the two childrens balises is automaticaly added.
 */
class CompletionXMLBalise {
public:
	/*!
	 * Contructor of the Completion balise
	 * \param category category where the balise is.
	 * \param node XML document from which the balise is contruct.
	 */
	CompletionXMLBalise( const QString & category, const QDomElement & node );
	/*!
	 * Destuctor of Completion XML Balise 
	 */
	~CompletionXMLBalise();

	/*!
	 * Name of the balise
	 * \return The name of the balise
	 */
	const QString & name() const { return m_name; };
	/*!
	 * Category of the balise (e.g. XSL, HTML)
	 * \return The category of the balise 
	 */
	const QString & category() const { return m_category; };
	/*!
	 * Default state of the balise. If the balise have default state to true and the balise
	 * is a children from another balise. The balise is show on the editor.
	 * \return true if the balise is the default balise from a list of children balise.
	 */
	bool isDefault() const { return m_isDefault; };
	
	/*!
	 * List of attributes of the balise
	 * \return The list of attributes in a QList
	 */
	const QList<CompletionXMLAttribute*> & attributes() const { return m_attributes; };
	/*!
	 * List of children balise of the current balise.
	 * \return A list of balise
	 */
	const QList<CompletionXMLBalise*> & balises() const { return m_balises; };
	
	/*!
	 * Return a balise from it's name.
	 * \param name Name of the balise to find.
	 * \return A balise coresponding of the param name
	 * \todo Use a hashtable for searching on name
	 */
	CompletionXMLBalise* balise( const QString & name ) const;
	/*!
	 * Return a attribute from it's name.
	 * \param name Name of the attribute.
	 * \return Attribute corresponding of the param name.
	 * \todo Use a hashtable for searching on name
	 */
	CompletionXMLAttribute* attribute( const QString & name ) const;
private:	
	QString m_name;
	bool m_isDefault;
	
	QString m_category;
	
	QList<CompletionXMLAttribute*> m_attributes;
	QList<CompletionXMLBalise*> m_balises;
};

/*!
 * Class provides completion on the document. This class read actually only XSL Completion part.
 * This class containts a list of balise from which the user can complete.
 * \todo Read completion on JavaScript.
 */
class Completion {
public:
	/*! 
	 * Create a Completion structure.
	 * \param name File name to use to read the document.
	 * \throw ENotCompletionFile if the document isn't valid
	 */
	Completion( const QString & name = QString() );
	/*!
	 * Destructor of the completion structure.
	 */
	virtual ~Completion();

	/*!
	 * List of all balise from which the user can complete (include children balise).
	 * \return A list of balise
	 */
	const QList<CompletionXMLBalise*> xmlBalises();
	/*! 
	 * Return a balise class corresponding on name in parameters.
	 * \param name Name used to find the balise
	 * \return A balise corresponding on name
	 * \todo Use a hashtable for searching on name
	 */
	CompletionXMLBalise* balise( const QString & name ) const;

	/*!
	 * Path of the file name used for completion.
	 * \return The path of file name used for completion.
	 * \sa setPath()
	 */
	const QString & path() const;
	/*!
	 * Set the path of the completion file.
	 * \param name New name of the path.
	 * \throw ENotCompletionFile if the document isn't valid.
	 * \sa path()
	 */
	void setPath( const QString & name );
	
protected:
	/*!
	 * Load the completion file and charged it in memory.
	 */
	void load();
private:
	QList<CompletionXMLBalise*> m_xmlBalises;
	QString m_name;
};

#endif
