/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2006-2010 by Ulrich Van Den Hekke                         *
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

#pragma once
#ifndef _XSLCONTENTVIEW2PARSER_H_
#define _XSLCONTENTVIEW2PARSER_H_

// Xinx header
#include <contentview2/contentview2parser.h>

// Qt header
#include <QApplication>
#include <QXmlStreamReader>
#include <QAbstractMessageHandler>

class QTextCodec;

/* XslContentViewParser */

class XslContentView2Parser : public ContentView2::Parser, private QXmlStreamReader, private QAbstractMessageHandler
{
	Q_DECLARE_TR_FUNCTIONS(XslContentView2Parser)
public:
	XslContentView2Parser();
	virtual ~XslContentView2Parser();

	virtual void load();

private:
	virtual void handleMessage(QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation);

	struct struct_xsl_variable
	{
		bool isParam;
		int line;
		QString name;
		QString value;
	};
	struct struct_script
	{
		int line;
		QString src;
		QString content;
		QString title;
	};

	void readStyleSheet();
	void readUnknownElement();
	void readVariable();
	void readTemplate(QList<struct_xsl_variable> & t, QList<struct_script> & s);
	void readTemplate();
	QString readElementText();

	ContentView2::Node attacheNewTemplateNode(ContentView2::Node parent, const QString & name, const QString & mode, int line);
	void attacheNewParamsNode(ContentView2::Node parent, const QString & name, const QString & value,  int line);
	void attacheNewVariableNode(ContentView2::Node parent, const QString & filename, const QString & value, int line);

	QTextCodec * m_codec;
};


#endif /* _XSLCONTENTVIEW2PARSER_H_ */
