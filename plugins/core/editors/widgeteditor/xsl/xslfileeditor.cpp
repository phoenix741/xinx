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
#include "editors/widgeteditor/xsl/xslfileeditor.h"
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
#include <utils/xsltparser.h>
#include <plugins/xinxpluginsloader.h>
#include <contentview2/contentview2treemodel.h>
#include <contentview2/contentview2manager.h>

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

/* StyleSheetEditor */

StyleSheetEditor::StyleSheetEditor(QWidget *parent) : TextFileEditor(new XslTextEditor(), parent), m_completionModel(0)
{
	initObjects();
}

StyleSheetEditor::~StyleSheetEditor()
{
	qobject_cast<XslTextEditor*>(textEdit())->setModel(0);
}

void StyleSheetEditor::initObjects()
{
	m_htmlView = new QWebView(this);
	m_htmlView->load(QUrl("about:blank"));
	m_htmlView->setMinimumHeight(100);

	m_sourceView = new XslTextEditor(this);

	connect(textEdit()->editor(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	m_completionModel = 0;

	m_tabWidget = new QTabWidget(this);
	m_tabWidget->setTabShape(QTabWidget::Triangular);
	m_tabWidget->setTabPosition(QTabWidget::South);
	connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabIndexChange(int)));

	XmlPresentationDockWidget * dock = XmlPresentationDockWidget::self();
	if (dock)
	{
		connect(dock, SIGNAL(filenameChanged(QString)), this, SLOT(xmlPresentationDockFilenameChanged(QString)));
	}
}

void StyleSheetEditor::initLayout()
{
	m_tabWidget->addTab(textEdit(), tr("XSL Source"));
	m_tabWidget->addTab(m_sourceView, tr("HTML Source"));
	m_tabWidget->addTab(m_htmlView, tr("Web View"));

	XmlPresentationDockWidget * dock = XmlPresentationDockWidget::self();
	if (dock)
		xmlPresentationDockFilenameChanged(dock->filename());

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget(m_tabWidget);

	hbox->setMargin(0);

	setLayout(hbox);
}

void StyleSheetEditor::updateCodec()
{
	QXmlStreamReader reader(textEdit()->editor()->text());
	while (! reader.atEnd())
	{
		reader.readNext();

		if (reader.isStartDocument())
		{
			setCodec(reader.documentEncoding().toString());
			break;
		}

		if (reader.isStartElement())
		{
			setCodec(TextFileEditor::codec()->name());
			break;
		}
	}
}

void StyleSheetEditor::detectCodec(QIODevice & d)
{
	{
		QXmlStreamReader reader(&d);
		while (! reader.atEnd())
		{
			reader.readNext();

			if (reader.isStartDocument())
			{
				setCodec(reader.documentEncoding().toString().toLatin1());
				break;
			}

			if (reader.isStartElement())
			{
				setCodec(TextFileEditor::codec()->name());
				break;
			}
		}

	}

	d.reset();
}

ContentView2::Parser * StyleSheetEditor::createParser()
{
	return new XslContentView2Parser();
}

ContentView2::CompletionModel * StyleSheetEditor::createModel(QSqlDatabase db, QObject * parent)
{
	if (! m_completionModel)
	{
		m_completionModel = new XslCompletionNodeModel(db, fileContainer(), parent);
		m_completionModel->setCompleteTags(XslCompletionNodeModel::NoTags);

		qobject_cast<XslTextEditor*>(textEdit())->setModel(m_completionModel);
	}
	return m_completionModel;
}

XslCompletionNodeModel * StyleSheetEditor::completionModel() const
{
	return m_completionModel;
}

QString StyleSheetEditor::defaultFileName() const
{
	return tr("noname.xsl");
}

QIcon StyleSheetEditor::icon() const
{
	return QIcon(":/images/typexsl.png");
}

bool StyleSheetEditor::autoIndent()
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

void StyleSheetEditor::xmlPresentationDockFilenameChanged(const QString & filename)
{
	m_tabWidget->setTabEnabled(1, ! filename.isEmpty());
	m_tabWidget->setTabEnabled(2, ! filename.isEmpty());
}

void StyleSheetEditor::tabIndexChange(int index)
{
	XmlPresentationDockWidget * dock = XmlPresentationDockWidget::self();
	if (dock && (! dock->filename().isEmpty()) && ((index == 1) || (index == 2)))
	{
		launchStylesheetParsing(dock->filename());
	}
}

void StyleSheetEditor::launchStylesheetParsing(const QString & xmlfile)
{
	XsltParser * xsltParser = 0;
	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		if (plugin->isActivated() && qobject_cast<IXinxXsltParser*>(plugin->plugin()))
		{
			xsltParser = qobject_cast<IXinxXsltParser*>(plugin->plugin())->createXsltParser();
			if (xsltParser) break;
		}
	}

	if (! xsltParser)
		xsltParser = new XsltParser();

	QString moduleInternetAdresse = lastFileName();
	if (XINXProjectManager::self()->project())
	{
		const QString resultPageAdresse = XINXProjectManager::self()->project()->readProperty("moduleInternetAdresse").toString();
		if (QFileInfo(resultPageAdresse).isRelative())
		{
			moduleInternetAdresse = QDir(XINXProjectManager::self()->project()->projectPath()).absoluteFilePath(resultPageAdresse);
		}
		else
		{
			moduleInternetAdresse = resultPageAdresse;
		}
	}

	QString result;
	QString utf8Text = textEdit()->toPlainText();
	QByteArray byte  = codec()->fromUnicode(utf8Text);

	ErrorManager::self()->clearMessages(textEdit()->filename());

	if (! xsltParser->loadStylesheet(textEdit()->filename())) goto error;
	if (! xsltParser->loadXmlFile(xmlfile)) goto error;
	if (! xsltParser->process())              goto error;

	result = xsltParser->getOutput();
	m_sourceView->setPlainText(result);
	m_htmlView->setHtml(result, QUrl::fromLocalFile(moduleInternetAdresse));

error:
	foreach(const XsltParser::ErrorMessage & e, xsltParser->errors())
	{
		ErrorManager::self()->addMessage(textEdit()->filename(), e.line, e.isWarning ? ErrorManager::MessageWarning : ErrorManager::MessageError, e.message);
	}

	delete xsltParser;
}

XmlPresentationDockWidget * StyleSheetEditor::xmlPresentationDockWidget()
{
	foreach(XinxPluginElement * element, XinxPluginsLoader::self()->plugins())
	{
		CorePlugin * plugin = qobject_cast<CorePlugin*>(element->plugin());
		if (plugin)
			return plugin->dock();
	}
	return 0;
}

void StyleSheetEditor::cursorPositionChanged()
{
	if (model())
	{
		XslTextEditor * te = qobject_cast<XslTextEditor*>(textEdit());
		QList<XPathBalise> p = te->xpath(
		                           te->textCursor(),
		                           QStringList() << "xsl:template" << "xsl:variable" << "xsl:param",
		                           QString(),
		                           QStringList() << "mode" << "name" << "match"
		                       );

		ContentView2::Node n = rootNode();
		while (p.size())
		{
			XPathBalise b = p.last();
			p.removeLast();

			const QString baliseName  = b.name;
			const QString baliseAttributeName  = b.attributes["name"];
			const QString baliseAttributeMatch = b.attributes["match"];
			const QString baliseAttributeMode  = b.attributes["mode"];

			const QList<int> childs = n.childs(ContentView2::Manager::self()->database());

			foreach(uint childId, childs)
			{
				ContentView2::Node child(ContentView2::Manager::self()->database(), childId);

				const QString childName  = child.data().toString();
				const QString childType  = child.data(ContentView2::Node::NODE_TYPE).toString();
				const QString childMode  = child.data(ContentView2::Node::NODE_USER_VALUE).toString();

				if ((baliseName == "xsl:template") && (childType == "XslTemplate"))
				{
					if (((childName == baliseAttributeName) || (childName == baliseAttributeMatch)) && (childMode == baliseAttributeMode))
					{
						n = child;
						break;
					}
				}
				else if ((baliseName == "xsl:variable") && (childType == "XslVariable"))
				{
					if (childName == baliseAttributeName)
					{
						n = child;
						break;
					}
				}
				else if ((baliseName == "xsl:param") && (childType == "XslParam"))
				{
					if (childName == baliseAttributeName)
					{
						n = child;
						break;
					}
				}
			}
		}
		QModelIndex indexToSelect = contentViewModel()->index(n);

		emit positionInEditorChanged(indexToSelect);
	}
}
