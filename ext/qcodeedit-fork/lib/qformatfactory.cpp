/****************************************************************************
**
** Copyright (C) 2006-2008 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
** 
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qformatfactory.h"

/*!
	\file qformatfactory.cpp
	\brief Implementation of QFormatFactory
	
	\see QFormatFactory
*/

/*!
	\ingroup language
	@{
	
	\class QFormatFactory
	\brief A storage/configuration class for shared highlighing formats
	
	It stores text format used by highlighting interfaces.
	
	\see QLanguageFactory
	\see QHighlighter
*/

#include "qformat.h"

#include <QFile>
#include <QDomText>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>

#define QFORMAT_VERSION "1.0"

/*!
	\brief Constructor
	\param f File used to store text formats configuration (INI)
*/
QFormatFactory::QFormatFactory(const QString& f, QObject *p)
 : QObject(p), m_settings(f)
{
	QFile settings(f);
	
	setFormat("normal", QFormat());
	
	if ( settings.open(QFile::ReadOnly | QFile::Text) )
	{
		QDomDocument doc;
		doc.setContent(&settings);
		
		if ( doc.documentElement().attribute("version") < QFORMAT_VERSION )
		{
			qWarning("Format encoding version mismatch : [found]%s != [expected]%s",
					qPrintable(doc.documentElement().attribute("version")),
					QFORMAT_VERSION);
			
			return;
		}
		
		QDomElement e, c;
		QDomNodeList l, f = doc.documentElement().elementsByTagName("format");
		
		for ( int i = 0; i < f.count(); i++ )
		{
			e = f.at(i).toElement();
			l = e.childNodes();
			
			QFormat fmt;
			
			for ( int i = 0; i < l.count(); i++ )
			{
				c = l.at(i).toElement();
				
				if ( c.isNull() )
					continue;
				
				QString field = c.tagName(),
						value = c.firstChild().toText().data();
				
				if ( field == "bold" )
					fmt.weight = ((value == "true") || (value.toUInt() == 1))
										? QFont::Bold : QFont::Normal;
				else if ( field == "italic" )
					fmt.italic = ((value == "true") || (value.toUInt() == 1));
				else if ( field == "underline" )
					fmt.underline = ((value == "true") || (value.toUInt() == 1));
				else if ( field == "strikeout" )
					fmt.strikeout = ((value == "true") || (value.toUInt() == 1));
				else if ( field == "color" )
					fmt.foreground = QColor(value);
				
			}
			
			setFormat(e.attribute("id"), fmt);
		}
	}
}

/*!
	\brief Destructor
	Save the modification of the factory to the file specified to constructor
*/
QFormatFactory::~QFormatFactory()
{
	QFile settings(m_settings);
	
	if ( settings.open(QFile::WriteOnly | QFile::Text) )
	{
		QDomDocument doc("QXF");
		
		QDomElement root = doc.createElement("QXF");
		root.setAttribute("version", QFORMAT_VERSION);
		
		for ( int i = 1; i < m_formatKeys.count(); i++ )
		{
			QDomText t;
			QDomElement f, c = doc.createElement("format");
			
			c.setAttribute("id", m_formatKeys.at(i));
			
			const QFormat& fmt = m_formatValues.at(i);
			
			f = doc.createElement("bold");
			t = doc.createTextNode((fmt.weight == QFont::Bold) ? "true" : "false");
			f.appendChild(t);
			c.appendChild(f);
			
			f = doc.createElement("italic");
			t = doc.createTextNode(fmt.italic ? "true" : "false");
			f.appendChild(t);
			c.appendChild(f);
			
			f = doc.createElement("underline");
			t = doc.createTextNode(fmt.underline ? "true" : "false");
			f.appendChild(t);
			c.appendChild(f);
			
			f = doc.createElement("strikeout");
			t = doc.createTextNode(fmt.strikeout ? "true" : "false");
			f.appendChild(t);
			c.appendChild(f);
			
			f = doc.createElement("color");
			t = doc.createTextNode(fmt.foreground.name());
			f.appendChild(t);
			c.appendChild(f);
			
			root.appendChild(c);
		}
		
		doc.appendChild(root);
		
		settings.write(doc.toByteArray(4));
	}
}

/*!
	\return A list of available format keys
*/
QStringList QFormatFactory::formats() const
{
	return m_formatKeys.toList();
}

/*!
	\return The format key associated to integer format id \a ifid
*/
QString QFormatFactory::id(int ifid) const
{
	return m_formatKeys.at(ifid);
}

/*!
	\return The integer format id associated to format key \a fid
*/
int QFormatFactory::id(const QString& sfid) const
{
	int idx = m_formatKeys.indexOf(sfid);
	
	return (idx == -1) ? 0 : idx;
}

/*!
	\return The text format associated to format key \a fid
*/
QFormat QFormatFactory::format(int ifid) const
{
	//qDebug("Querying format id %i within ]-1, %i[", ifid, m_formatValues.count());
	
	return (ifid < m_formatValues.count()) ? m_formatValues.at(ifid) : QFormat();
}

/*!
	\return The text format associated to format key \a fid
*/
QFormat QFormatFactory::format(const QString& sfid) const
{
	return format(id(sfid));
}

/*!
	\brief Set text format for key
	\param fid Format key
	\param fmt Format value
*/
void QFormatFactory::setFormat(const QString& fid, const QFormat& fmt)
{
	const int idx = m_formatKeys.indexOf(fid);
	
	if ( idx != -1 )
	{
		m_formatValues[idx] = fmt;
	} else {
		
		//qDebug("adding format %s [%i]", qPrintable(fid), m_formatKeys.count());
		
		m_formatKeys << fid;
		m_formatValues << fmt;
	}
}

/*! @} */
