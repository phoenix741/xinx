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

// Xinx header
#include "xmlfileeditor.h"
#include "editors/prettyprint/xmlprettyprinter.h"
#include "editors/models/xsl/xslcompletionnodemodel.h"
#include "editors/models/xsl/xslcontentviewparser.h"
#include "xmltexteditor.h"
#include <borderlayout.h>
#include <plugins/xinxpluginsloader.h>
#include "plugindefinition/coreplugin.h"
#include "docks/datastream/xmlpresentationdockwidget.h"
#include "config/selfwebpluginsettings.h"
#include <core/xinxconfig.h>
#include <project/xinxproject.h>

// Qt header
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QCompleter>
#include <QTextCodec>
#include <QWebView>
#include <QSplitter>
#include <QApplication>
#include <QTemporaryFile>
#include <QProcess>
#include <QDebug>
#include <QWebFrame>
#include <QTabWidget>

/* XmlFileEditor */

XmlFileEditor::XmlFileEditor(QWidget *parent) : TextFileEditor(new XmlTextEditor(), parent), m_codec(0)
{
	initObjects();
}

XmlFileEditor::~XmlFileEditor()
{
}

void XmlFileEditor::initObjects()
{
}

QTextCodec * XmlFileEditor::codec() const
{
	if (m_codec)
		return m_codec;
	else
		return TextFileEditor::codec();
}

QString XmlFileEditor::defaultFileName() const
{
	return tr("noname.xml");
}

QIcon XmlFileEditor::icon() const
{
	return QIcon(":/images/typexml.png");
}

bool XmlFileEditor::autoIndent()
{
	try
	{
		XMLPrettyPrinter prettyPrinter;
		prettyPrinter.setText(textEdit()->toPlainText());
		prettyPrinter.process();

		textEdit()->textCursor().beginEditBlock();
		textEdit()->editor()->selectAll();
		textEdit()->textCursor().removeSelectedText();
		textEdit()->textCursor().movePosition(1, QDocumentCursor::Start);
		textEdit()->textCursor().insertText(prettyPrinter.getResult());
		textEdit()->textCursor().endEditBlock();
	}
	catch (XMLPrettyPrinterException e)
	{
		ErrorManager::self()->addMessage(lastFileName(), e.m_line, ErrorManager::MessageError, e);
		return false;
	}
	return true;
}

void XmlFileEditor::loadFromDevice(QIODevice & d)
{
	{
		QXmlStreamReader reader(&d);
		while (! reader.atEnd())
		{
			reader.readNext();

			if (reader.isStartDocument())
			{
				m_codec = QTextCodec::codecForName(reader.documentEncoding().toString().toLatin1());
				break;
			}

			if (reader.isStartElement())
			{
				m_codec = TextFileEditor::codec();
				break;
			}
		}
	}
	d.reset();
	TextFileEditor::loadFromDevice(d);
}

