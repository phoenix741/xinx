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
 
#ifndef __XSDDOCUMENT_H__
#define __XSDDOCUMENT_H__

#include <QString>
#include <QList>
#include <QMap>
#include <QRegExp>
#include <QDomElement>

/* XSD Description */

class XSDObject {
protected:
	virtual void loadElement( const QDomElement & ) {};
public:
};

class XSDAnnotation : public XSDObject {
public:
	XSDAnnotation( const QDomElement & element ) { loadElement( element ); };	

	const QString & documentation() const { return m_documentation; };
protected:
	virtual void loadElement( const QDomElement & );
private:
	QString m_documentation;	
};

class XSDAnnotedObject : public XSDObject {
public:
	XSDAnnotedObject( const QDomElement & element ) { loadElement( element ); };	

	XSDAnnotation * annotation() const { return m_annotation; };
protected:
	virtual void loadElement( const QDomElement & );
private:
	XSDAnnotation * m_annotation;
};

class XSDType : public XSDObject {
public:
	const QString & type() const { return m_type; }; // choice, sequence, complexContent (with extension) ...
	const QString & base() const { return m_base; }; // for subclassing
	const QList<XSDObject*> & types() const { return m_types; };
private:
	QString m_type, m_base;
	QList<XSDObject*> m_types;
};

class XSDComplexType : public XSDObject { /* xs:complexType */ // Type of element
public:
	XSDComplexType( const QDomElement & element ) { loadElement( element ); };	

	const QString & name() const { return m_name; };
	XSDType * type() const { return m_type; };
protected:
	virtual void loadElement( const QDomElement & ) {};
private:
	QString m_name;
	XSDType * m_type;
};

class XSDRestriction : public XSDObject {
public:
	const QString & baseType() const { return m_baseType; };
private:
	QString m_baseType;
};

class XSDEnumeration : public XSDRestriction {
public:
	const QList<QString> & list() const { return m_list; };
private:
	QList<QString> m_list;
};

class XSDStringBoundary : public XSDRestriction {
public:
	unsigned int minLength() const { return m_minLength; };
	unsigned int maxLength() const { return m_maxLength; };
private:
	unsigned int m_minLength, m_maxLength;
};

class XSDDecimalBoudary : public XSDRestriction {
public:
	double minInclusive() const { return m_minInclusive; };
	double maxInclusive() const { return m_maxInclusive; };
private:
	double m_minInclusive, m_maxInclusive;
};

class XSDIntegerBoudary : public XSDRestriction {
public:
	int minInclusive() const { return m_minInclusive; };
	int maxInclusive() const { return m_maxInclusive; };
private:
	int m_minInclusive, m_maxInclusive;
};

class XSDStringRegExp : public XSDRestriction {
public:
	const QRegExp & regexp() { return m_regexp; };
private:
	QRegExp m_regexp;
};

class XSDSimpleType : public XSDObject { /* xsl:simpleType */ // Type of attibute
public:
	XSDSimpleType( const QDomElement & element ) { loadElement( element ); };	

	const QString & name() const { return m_name; };
	const XSDRestriction * restriction() { return m_restriction; };
protected:
	virtual void loadElement( const QDomElement & ) {};
private:
	QString m_name;
	XSDRestriction * m_restriction;
};

class XSDElement : public XSDAnnotedObject { /* xs:sequence */
public:
	XSDElement( const QDomElement & element ) : XSDAnnotedObject( element ) { loadElement( element ); };	

	const QString & name() const { return m_name; };
	const QString & typeName() const { return m_typeName; }; 
	const XSDType * type() const { return m_type; };
	
	unsigned int minOccurs() const { return m_minOccurs; };
	unsigned int maxOccurs() const { return m_maxOccurs; }; // MAX for unbounded
protected:
	virtual void loadElement( const QDomElement & ) {};
private:
	QString m_name, m_typeName;
	XSDType * m_type;
	
	unsigned int m_minOccurs, m_maxOccurs;
};

class XSDAttribute : public XSDAnnotedObject {
public:
	XSDAttribute( const QDomElement & element ) : XSDAnnotedObject( element ) { loadElement( element ); };
	
	const QString & name() const { return m_name; };
	const QString & typeName() const { return m_typeName; }; 
	const QString & defaultValue() const { return m_defaultValue; };
	const XSDType * type() const { return m_type; };
	
	const QString & use() const { return m_use; };
protected:
	virtual void loadElement( const QDomElement & );
private:
	QString m_name, m_typeName, m_defaultValue, m_use;
	XSDType * m_type;
};

class XSDSchema : public XSDObject {
public:
	XSDSchema(const QString &);

	const QMap<QString,XSDElement*> & elements() const { return m_elements; };
	const QMap<QString,XSDComplexType*> & complexType() const { return m_complexType; };
	const QMap<QString,XSDSimpleType*> & simpleType() const { return m_simpleType; };
private:
	void loadFileByName( const QString & );
	virtual void loadElement( const QDomElement & );

	QMap<QString,XSDElement*> m_elements;
	QMap<QString,XSDComplexType*> m_complexType;
	QMap<QString,XSDSimpleType*> m_simpleType;
};

#endif // __XSDDOCUMENT_H__
