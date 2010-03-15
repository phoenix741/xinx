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
#include "htmlfileeditor.h"
#include "editors/prettyprint/xmlprettyprinter.h"
#include "editors/models/xsl/xslcompletionnodemodel.h"
#include "editors/models/xsl/xslcontentviewparser.h"
#include "editors/widgeteditor/xsl/xsltexteditor.h"
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

/* HtmlFileEditor */

HtmlFileEditor::HtmlFileEditor(QWidget *parent) : TextFileEditor(new XslTextEditor(), parent), m_htmlTempFile(0)
{
	initObjects();
}

HtmlFileEditor::~HtmlFileEditor()
{
	qobject_cast<XslTextEditor*>(textEdit())->setModel(0);
	delete m_completionModel;
}

void HtmlFileEditor::initObjects()
{
	m_htmlView = new QWebView(this);
	m_htmlView->load(QUrl("about:blank"));
	m_htmlView->setMinimumHeight(100);

	m_completionModel = 0;

	m_tabWidget = new QTabWidget(this);
	m_tabWidget->setTabShape(QTabWidget::Triangular);
	m_tabWidget->setTabPosition(QTabWidget::South);

	connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabIndexChange(int)));
}

ContentView2::CompletionModel * HtmlFileEditor::createModel(QSqlDatabase db, QObject * parent)
{
	if (! m_completionModel)
	{
		m_completionModel = new XslCompletionNodeModel(db, fileContainer(), parent);
		m_completionModel->setCompleteTags(XslCompletionNodeModel::Html);

		qobject_cast<XslTextEditor*>(textEdit())->setModel(m_completionModel);
	}
	return m_completionModel;
}

void HtmlFileEditor::initLayout()
{
	m_tabWidget->addTab(textEdit(), tr("HTML Source"));
	m_tabWidget->addTab(m_htmlView, tr("Web View"));

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget(m_tabWidget);

	hbox->setMargin(0);

	setLayout(hbox);
}

XslCompletionNodeModel * HtmlFileEditor::completionModel() const
{
	return m_completionModel;
}

QString HtmlFileEditor::defaultFileName() const
{
	return tr("noname.html");
}

QIcon HtmlFileEditor::icon() const
{
	return QIcon(":/images/typehtml.png");
}

QTextCodec * HtmlFileEditor::codec() const
{
	return QTextCodec::codecForHtml(textEdit()->toPlainText().toLocal8Bit(), TextFileEditor::codec());
}

bool HtmlFileEditor::autoIndent()
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
		addNewErrorMessages(e.m_line, e.getMessage(), ERROR_MESSAGE);
		return false;
	}
	return true;
}

void HtmlFileEditor::tabIndexChange(int index)
{
	if (index == 1)
	{
		showHtml();
	}
}

void HtmlFileEditor::showHtml()
{
	m_tabWidget->setCurrentIndex(1);

	// Get HTML text
	QString htmlText = textEdit()->toPlainText();

	QString moduleInternetAdresse = XINXProjectManager::self()->project() ?
	                                QDir(XINXProjectManager::self()->project()->projectPath()).absoluteFilePath(
	                                    XINXProjectManager::self()->project()->readProperty("moduleInternetAdresse").toString()
	                                ) : lastFileName();

	m_htmlView->setHtml(htmlText, QUrl(moduleInternetAdresse));
}

