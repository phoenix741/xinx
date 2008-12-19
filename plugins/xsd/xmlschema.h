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

#ifndef _XMLSCHEMA_H_
#define _XMLSCHEMA_H_

// Qt header
#include <QObject>
#include <QString>

/* Forward declaration */

class QFile;
class XmlSchemaFile;

/* XsdNodeItem */

class XsdItem : public QObject {
	Q_OBJECT
public:
	XsdItem( XmlSchemaFile * schema, QObject * parent = 0 );

	XmlSchemaFile * schema() const;
private:
	XmlSchemaFile * m_schema;
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
private:
	QString m_name, m_type;
	int m_minOccurs, m_maxOccurs;
};

/* XsdSequenceItem */

class XsdSequenceItem : public XsdElementItem {
public:
	XsdSequenceItem( XmlSchemaFile * schema, QObject * parent = 0 );

	QList<XsdElementItem*> elements() const;
};

/* XsdChoiceItem */

class XsdChoiceItem : public XsdSequenceItem {
public:
	XsdChoiceItem( XmlSchemaFile * schema, QObject * parent = 0 );
};

/* XmlSchemaFile */

class XmlSchemaFile : public XsdItem {
	Q_OBJECT
public:
	XmlSchemaFile();
	XmlSchemaFile( QFile * file );

	void loadFromFile( const QString & filename );
	void loadFromFile( QFile * file );
private:
};

#endif /* _XMLSCHEMA_H_ */
