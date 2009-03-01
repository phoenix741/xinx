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

#ifndef _XSLCONTENTVIEWPARSER_H_
#define _XSLCONTENTVIEWPARSER_H_

// Xinx header
#include <contentview/contentviewparser.h>

// Qt header
#include <QApplication>
#include <QXmlStreamReader>

class ContentViewNode;
class QTextCodec;

class XslContentViewParser : public ContentViewParser, private QXmlStreamReader {
	Q_DECLARE_TR_FUNCTIONS(XslContentViewParser)
public:
	XslContentViewParser( bool autoDelete = false );
	virtual ~XslContentViewParser();

	QTextCodec * codec() { return m_codec; };
protected:
	virtual bool loadFromDeviceImpl();
private:
	struct struct_xsl_variable {
		bool isParam;
		int line;
		QString name;
		QString value;
	};
	struct struct_script {
		bool isSrc;
		int line;
		QString content;
		QString src;
		QString title;
	};

	void readStyleSheet();
	void readUnknownElement();
	void readVariable();
	void readTemplate( QList<struct_xsl_variable> & t, QList<struct_script> & s );
	void readTemplate();
	QString readElementText();

	ContentViewNode * attacheNewTemplateNode( ContentViewNode * parent, const QString & name, const QString & mode, int line );
	ContentViewNode * attacheNewParamsNode( ContentViewNode * parent, const QString & name, const QString & value,  int line );
	ContentViewNode * attacheNewVariableNode( ContentViewNode * parent, const QString & filename, const QString & value, int line );

	QTextCodec * m_codec;
};

#endif /* _XSLCONTENTVIEWPARSER_H_ */