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
#include "xmltexteditor.h"
#include <borderlayout.h>
#include <plugins/xinxpluginsloader.h>
#include "plugindefinition/coreplugin.h"
#include "docks/datastream/xmlpresentationdockwidget.h"
#include "config/selfwebpluginsettings.h"
#include <core/xinxconfig.h>
#include <project/xinxprojectproject.h>
#include <core/errormanager.h>

// Qt header
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QCompleter>
#include <QWebView>
#include <QSplitter>
#include <QApplication>
#include <QTemporaryFile>
#include <QProcess>
#include <QDebug>
#include <QWebFrame>
#include <QTabWidget>

/* XmlFileEditor */

XmlFileEditor::XmlFileEditor(QWidget *parent) : TextFileEditor(new XmlTextEditor(), parent)
{
}

XmlFileEditor::~XmlFileEditor()
{
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
		ErrorManager::self()->addMessage(lastFileName(), e.m_line, QtCriticalMsg, e);
		return false;
	}
	return true;
}

void XmlFileEditor::updateCodec()
{
	QXmlStreamReader reader(textEdit()->editor()->text());
	while (! reader.atEnd())
	{
		reader.readNext();

		if (reader.isStartDocument())
		{
			setCodecName(reader.documentEncoding().toString());
			break;
		}

		if (reader.isStartElement())
		{
			break;
		}
	}
}

void XmlFileEditor::detectCodec(QIODevice & d)
{
	{
		QXmlStreamReader reader(&d);
		while (! reader.atEnd())
		{
			reader.readNext();

			if (reader.isStartDocument())
			{
				setCodecName(reader.documentEncoding().toString());
				break;
			}

			if (reader.isStartElement())
			{
				break;
			}
		}

	}

	d.reset();
}


