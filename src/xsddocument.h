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
#include <QMultiMap>
#include <QRegExp>
#include <QDomElement>

/* XSD Description */

class XSDObject {
public:
	XSDObject();
	virtual ~XSDObject();

protected:
	virtual void loadElement( const QDomElement & );
public:
};

class XSDAnnotation : public XSDObject {
public:
	XSDAnnotation( const QDomElement & element );	

	const QString & documentation() const { return m_documentation; };
protected:
	virtual void loadElement( const QDomElement & );
private:
	QString m_documentation;	
};

class XSDAnnotedObject : public XSDObject {
public:
	XSDAnnotedObject( const QDomElement & element );	
	virtual ~XSDAnnotedObject();

	XSDAnnotation * annotation() const { return m_annotation; };
protected:
	virtual void loadElement( const QDomElement & );
private:
	XSDAnnotation * m_annotation;
};

class XSDElementType : public XSDAnnotedObject {
public:
	XSDElementType( const QDomElement & element );
	virtual ~XSDElementType();

	enum ElementType { NONE = 0, CHOICE = 1, SEQUENCE = 2, COMPLEXCONTENT = 3, ALL = 4, UNKNOWN = -1 };

	enum ElementType type() const { return m_type; }; // choice, sequence, complexContent (with extension) ...
	const QString & base() const { return m_base; }; // for subclassing
	const QList<XSDObject*> & types() const { return m_types; };
protected:
	virtual void loadElement( const QDomElement & );
private:
	ElementType m_type;
	QString m_base;
	QList<XSDObject*> m_types;
};

class XSDComplexType : public XSDObject { /* xs:complexType */ // Type of element
public:
	XSDComplexType( const QDomElement & element ) { loadElement( element ); };	

	const QString & name() const { return m_name; };
	XSDElementType * type() const { return m_type; };
protected:
	virtual void loadElement( const QDomElement & ) {};
private:
	QString m_name;
	XSDElementType * m_type;
};

class XSDElement : public XSDObject { /* xs:element */
public:
	XSDElement( const QDomElement & element );	
	virtual ~XSDElement();

	const QString & name() const { return m_name; };
	const QString & typeName() const { return m_typeName; }; 
	const XSDElementType * type() const { return m_type; };
	
	unsigned int minOccurs() const { return m_minOccurs; };
	unsigned int maxOccurs() const { return m_maxOccurs; }; // MAX for unbounded
protected:
	virtual void loadElement( const QDomElement & ) {};
private:
	QString m_name, m_typeName;
	XSDElementType * m_type;
	
	unsigned int m_minOccurs, m_maxOccurs;
};

class XSDSchema;
class QValidator;

class XSDRestriction : public XSDObject {
public:
	XSDRestriction( const QDomElement & element );

	const QString & baseType() const { return m_baseType; };
	const QMultiMap<QString,QString> & properties() { return m_properties; };
	
	QString getProperty( const QString &, XSDSchema * = NULL );
	QList<QString> getProperties( const QString &, XSDSchema * = NULL );
	
	QList<QString> enumerations( XSDSchema * = NULL );
	QString whiteSpace( XSDSchema * = NULL );
	int minLength( XSDSchema * = NULL );
	int maxLength( XSDSchema * = NULL );
	int length( XSDSchema * = NULL );
	QRegExp pattern( XSDSchema * = NULL );
	
	int totalDigits( XSDSchema * = NULL );
	int fractionDigits( XSDSchema * = NULL );

	QString minInclusive( XSDSchema * = NULL );
	QString minExclusive( XSDSchema * = NULL );
	QString maxInclusive( XSDSchema * = NULL );
	QString maxExclusive( XSDSchema * = NULL );

	QValidator * validator();
protected:
	virtual void loadElement( const QDomElement & );
private:
	QString m_baseType;
	QMultiMap<QString,QString> m_properties;
};

class XSDAttributeType : public XSDAnnotedObject {
public:
	XSDAttributeType( const QDomElement & element );
	virtual ~XSDAttributeType();

	XSDRestriction * restriction() { return m_restriction; };
protected:
	virtual void loadElement( const QDomElement & );
private:	
	XSDRestriction * m_restriction;
};

class XSDSimpleType : public XSDObject { /* xsl:simpleType */ // Type of attibute
public:
	XSDSimpleType( const QDomElement & element );	
	virtual ~XSDSimpleType();

	const QString & name() const { return m_name; };
	XSDAttributeType * type() const { return m_type; };
protected:
	virtual void loadElement( const QDomElement & );
private:
	QString m_name;
	XSDAttributeType * m_type;
};

class XSDAttribute : public XSDObject {
public:
	XSDAttribute( const QDomElement & element );
	virtual ~XSDAttribute();
	
	const QString & name() const { return m_name; };
	const QString & typeName() const { return m_typeName; }; 
	const QString & defaultValue() const { return m_defaultValue; };
	const XSDAttributeType * type() const { return m_type; };
	
	const QString & use() const { return m_use; };
protected:
	virtual void loadElement( const QDomElement & );
private:
	QString m_name, m_typeName, m_defaultValue, m_use;
	XSDAttributeType * m_type;
};

class XSDSchema : public XSDObject {
public:
	XSDSchema( const QString & );
	virtual ~XSDSchema();

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
