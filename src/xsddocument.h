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


/* XSD Description */

class XSDObject {
	
};

class XSDAnnotation : public XSDObject {
	const QString & documentation() const { return m_documentation; };
private:
	QString m_documentation;	
};

class XSDAnnotedObject : public XSDObject {
public:
	XSDAnnotation * annotation() const { return m_annotation; };
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
	const QString & name() const { return m_name; };
	XSDType * type() const { return m_type; };
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
	int minInclusive() const;
	int maxInclusive() const;
};

class XSDStringRegExp : public XSDRestriction {
public:
	const QRegExp & regexp();
};

class XSDSimpleType : public XSDObject { /* xsl:simpleType */ // Type of attibute
	const QString & name() const;
	const XSDRestriction * restriction();
};

class XSDElement : public XSDAnnotedObject { /* xs:sequence */
public:
	const QString & name() const;
	const QString & typeName() const; 
	const XSDType * type() const;
	
	unsigned int minOccurs() const;
	unsigned int maxOccurs() const; // MAX for unbounded
};

class XSDAttribute : public XSDAnnotedObject {
public:
	const QString & name() const;
	const QString & typeName() const; 
	const QString & defaultValue() const;
	const XSDType * type() const;
	
	const QString & use() const;
};


#endif // __XSDDOCUMENT_H__
