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

#ifndef _XMLSCHEMA_H_
#define _XMLSCHEMA_H_

// Qt header
#include <QObject>
#include <QString>
#include <QStringList>
#include <QDomElement>

/* Forward declaration */

class QIODevice;
class XmlSchemaFile;

/* XsdNodeItem */

class XsdItem : public QObject {
	Q_OBJECT
	Q_PROPERTY( bool readOnly READ readOnly WRITE setReadOnly )
public:
	XsdItem( XmlSchemaFile * schema, QObject * parent = 0 );

	XmlSchemaFile * schema() const;

	bool readOnly() const;
	void setReadOnly( bool readonly );

	virtual void loadFromElement( const QDomElement & e );
private:
	bool m_readOnly;
	XmlSchemaFile * m_schema;
};

/* XsdAnnotationItem */

class XsdAnnotationItem : public XsdItem {
	Q_OBJECT
	Q_PROPERTY( QString documentation READ documentation WRITE setDocumentation )
public:
	XsdAnnotationItem( XmlSchemaFile * schema, QObject * parent = 0, const QString & documentation = QString() );

	const QString & documentation() const;
	void setDocumentation( const QString & value );

	virtual void loadFromElement( const QDomElement & e );
private:
	QString m_documentation;
};

/* XsdAttributeItem */

class XsdAttributeItem : public XsdItem {
	Q_OBJECT
	Q_PROPERTY( QString name READ name WRITE setName )
	Q_PROPERTY( QString value READ value WRITE setValue )
	Q_PROPERTY( QString use READ use WRITE setUse )
public:
	XsdAttributeItem( XmlSchemaFile * schema, QObject * parent = 0, const QString & name = QString(), const QString & value = QString(), const QString & use = QString() );

	const QString & name() const;
	void setName( const QString & value );

	const QString & value() const;
	void setValue( const QString & value );

	const QString & use() const;
	void setUse( const QString & value );
private:
	QString m_name, m_value, m_use;
};

/* XsdSimpleType */

class XsdSimpleType : public XsdItem {
	Q_OBJECT
	Q_PROPERTY( QString name READ name WRITE setName )
public:
	XsdSimpleType( XmlSchemaFile * schema, QObject * parent = 0, const QString & name = QString() );

	const QString & name() const;
	void setName( const QString & value );

	virtual void loadFromElement( const QDomElement & e );
private:
	QString m_name;
};

/* XsdComplexeType */

class XsdComplexeType : public XsdSimpleType {
	Q_OBJECT
public:
	XsdComplexeType( XmlSchemaFile * schema, QObject * parent = 0, const QString & name = QString() );
};

/* XsdElementItem */

class XsdElementItem : public XsdItem {
	Q_OBJECT
	Q_PROPERTY( QString name READ name WRITE setName )
	Q_PROPERTY( QString type READ type WRITE setType )
	Q_PROPERTY( int minOccurs READ minOccurs WRITE setMinOccurs )
	Q_PROPERTY( int maxOccurs READ maxOccurs WRITE setMaxOccurs )
public:
	XsdElementItem( XmlSchemaFile * schema, QObject * parent = 0, const QString & name = QString(), const QString & type = QString(), int minOccurs = 0, int maxOccurs = 0 );

	const QString & name() const;
	void setName( const QString & value );

	const QString & type() const;
	void setType( const QString & value );

	int minOccurs() const;
	void setMinOccurs( int value );

	int maxOccurs() const;
	void setMaxOccurs( int value );

	virtual void loadFromElement( const QDomElement & e );
private:
	QString m_name, m_type;
	int m_minOccurs, m_maxOccurs;
};

/* XsdRestrictionItem */

class XsdRestrictionItem : public XsdItem {
	Q_OBJECT
	Q_PROPERTY( QString minValue READ minValue WRITE setMinValue )
	Q_PROPERTY( QString maxValue READ maxValue WRITE setMaxValue )
	Q_PROPERTY( QStringList enumeration READ enumeration WRITE setEnumeration )
	Q_PROPERTY( QString pattern READ pattern WRITE setPattern )
public:
	XsdRestrictionItem( XmlSchemaFile * schema, QObject * parent = 0, const QString & base = QString() );

	const QString & base();
	void setBase( const QString & value );

	const QString & minValue() const;
	void setMinValue( const QString & value );

	const QString & maxValue() const;
	void setMaxValue( const QString & value );

	const QStringList & enumeration() const;
	void setEnumeration( const QStringList & value );

	const QString & pattern() const;
	void setPattern( const QString & pattern );

	virtual void loadFromElement( const QDomElement & e );
private:
	QString m_minValue, m_maxValue, m_pattern, m_base;
	QStringList m_enumeration;
};

/* XsdSequenceItem */

class XsdSequenceItem : public XsdElementItem {
	Q_OBJECT
public:
	XsdSequenceItem( XmlSchemaFile * schema, QObject * parent = 0 );

	QList<XsdElementItem*> elements() const;

	virtual void loadFromElement( const QDomElement & e );

	static XsdElementItem * createXsdElementItem( XmlSchemaFile * schema, QObject * parent, const QDomElement & e );
};

/* XsdChoiceItem */

class XsdChoiceItem : public XsdSequenceItem {
	Q_OBJECT
public:
	XsdChoiceItem( XmlSchemaFile * schema, QObject * parent = 0 );
};

/* XsdComplexeContent */

class XsdComplexeContentItem : public XsdSequenceItem {
	Q_OBJECT
	Q_PROPERTY( QString base READ base WRITE setBase )
public:
	XsdComplexeContentItem( XmlSchemaFile * schema, QObject * parent = 0, const QString & base = QString() );

	const QString & base() const;
	void setBase( const QString & value );

	virtual void loadFromElement( const QDomElement & e );
private:
	QString m_base;
};

/* XmlSchemaFile */

class XmlSchemaFile : public XsdItem {
	Q_OBJECT
public:
	XmlSchemaFile();
	XmlSchemaFile( QIODevice * file );

	void loadFromFile( const QString & filename );
	void loadFromFile( QIODevice * file, bool isInclude = true );
private:
	XsdItem * createNewItem( XsdItem * parent, const QDomElement & e, bool isInclude );

	QList<XmlSchemaFile*> m_imports, m_includes;
};

#endif /* _XMLSCHEMA_H_ */
