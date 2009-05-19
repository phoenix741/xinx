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
#include "xmlprettyprinter.h"
#include "xslcompletionnodemodel.h"
#include "xslcontentviewparser.h"
#include "xmltexteditor.h"
#include <contentview/contentviewnode.h>
#include <contentview/contentviewmodel.h>
#include <borderlayout.h>
#include <plugins/xinxpluginsloader.h>
#include "../xmlpres/xmlpres.h"
#include "../xmlpres/xmlpresentationdockwidget.h"
#include "config/selfwebpluginsettings.h"
#include <core/xinxconfig.h>
#include <core/xinxproject.h>

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

/* HtmlFileEditor */

HtmlFileEditor::HtmlFileEditor( QWidget *parent ) : TextFileEditor( new XslTextEditor(), parent ) {
	m_completionModel = new XslCompletionNodeModel( 0, this );
	m_completionModel->setCompleteTags( XslCompletionNodeModel::Html );

	QCompleter * completer = new QCompleter( textEdit() );
	completer->setModel( m_completionModel );
	textEdit()->setCompleter( completer );
	qobject_cast<XslTextEditor*>( textEdit() )->setModel( m_completionModel );
}

HtmlFileEditor::~HtmlFileEditor() {
	qobject_cast<XslTextEditor*>( textEdit() )->setModel( 0 );
	delete m_completionModel;
}

XslCompletionNodeModel * HtmlFileEditor::completionModel() const {
	return m_completionModel;
}

QString HtmlFileEditor::defaultFileName() const {
	return tr( "noname.html" );
}

QIcon HtmlFileEditor::icon() const {
	return QIcon( ":/images/typehtml.png" );
}

QTextCodec * HtmlFileEditor::codec() const {
	return QTextCodec::codecForHtml( textEdit()->toPlainText().toLocal8Bit(), TextFileEditor::codec() );
}

bool HtmlFileEditor::autoIndent() {
	try {
		XMLPrettyPrinter prettyPrinter;
		prettyPrinter.setText( textEdit()->toPlainText() );
		prettyPrinter.process();

		textEdit()->textCursor().beginEditBlock();
		textEdit()->editor()->selectAll();
		textEdit()->textCursor().removeSelectedText();
		textEdit()->textCursor().movePosition( 1, QDocumentCursor::Start );
		textEdit()->textCursor().insertText( prettyPrinter.getResult() );
		textEdit()->textCursor().endEditBlock();
	} catch( XMLPrettyPrinterException e ) {
		setMessage( e.getMessage() );
		return false;
	}
	return true;
}

/* XmlFileEditor */

XmlFileEditor::XmlFileEditor( QWidget *parent ) : TextFileEditor( new XmlTextEditor(), parent ), m_codec( 0 ) {
}

XmlFileEditor::~XmlFileEditor() {
}

QTextCodec * XmlFileEditor::codec() const {
	if( m_codec )
		return m_codec;
	else
		return TextFileEditor::codec();
}

QString XmlFileEditor::defaultFileName() const {
	return tr( "noname.xml" );
}

QIcon XmlFileEditor::icon() const {
	return QIcon( ":/images/typexml.png" );
}

bool XmlFileEditor::autoIndent() {
	try {
		XMLPrettyPrinter prettyPrinter;
		prettyPrinter.setText( textEdit()->toPlainText() );
		prettyPrinter.process();

		textEdit()->textCursor().beginEditBlock();
		textEdit()->editor()->selectAll();
		textEdit()->textCursor().removeSelectedText();
		textEdit()->textCursor().movePosition( 1, QDocumentCursor::Start );
		textEdit()->textCursor().insertText( prettyPrinter.getResult() );
		textEdit()->textCursor().endEditBlock();
	} catch( XMLPrettyPrinterException e ) {
		setMessage( e.getMessage() );
		return false;
	}
	return true;
}

void XmlFileEditor::loadFromDevice( QIODevice & d ) {
	{
		QXmlStreamReader reader( &d );
		while( ! reader.atEnd() ) {
			reader.readNext();

			if( reader.isStartDocument() ) {
				m_codec = QTextCodec::codecForName( reader.documentEncoding().toString().toLatin1() );
				break;
			}

			if( reader.isStartElement() ) {
				m_codec = TextFileEditor::codec();
				break;
			}
		}
	}
	d.reset();
	TextFileEditor::loadFromDevice( d );
}

/* StyleSheetEditor */

StyleSheetEditor::StyleSheetEditor( QWidget *parent ) : ContentViewTextEditor( new XslContentViewParser(), new XslTextEditor(), parent ) {
	m_parsingProcess  = new QProcess( this );
	m_parsingProcess->setWorkingDirectory( XINXProjectManager::self()->project()->projectPath() );
	m_parsingProcess->setProcessChannelMode( QProcess::MergedChannels );
	connect( m_parsingProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(parsingFinished()) );

	m_htmlView = new QWebView( this );
	m_htmlView->load( QUrl( "about:blank" ) );
	m_htmlView->setMinimumHeight( 100 );
	splitter()->addWidget( m_htmlView );
	m_htmlView->hide();

	connect( textEdit()->editor(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()) );

	m_completionModel = new XslCompletionNodeModel( rootNode(), this );
	m_completionModel->setCompleteTags( XslCompletionNodeModel::NoTags );

	QCompleter * completer = new QCompleter( textEdit() );
	completer->setModel( m_completionModel );
	textEdit()->setCompleter( completer );
	qobject_cast<XslTextEditor*>( textEdit() )->setModel( m_completionModel );
}

StyleSheetEditor::~StyleSheetEditor() {
	ContentViewParser * p = parser();
	setParser( 0 );
	qobject_cast<XslTextEditor*>( textEdit() )->setModel( 0 );
	delete m_completionModel;
	delete p;
}

void StyleSheetEditor::parsingFinished() {
	if( m_parsingOutputFile ) {
		m_parsingOutputFile->open();
		QTextStream outputFileText( m_parsingOutputFile );
		m_parsingOutput = outputFileText.readAll();

		delete m_parsingOutputFile; m_parsingOutputFile = 0;
		delete m_parsingStyleSheetFile; m_parsingStyleSheetFile = 0;

		QString error = m_parsingProcess->readAllStandardOutput();
		if( ! error.isEmpty() ) {
			QMessageBox::critical( qApp->activeWindow(), tr("Stylesheet Parsing"), error );
		}
	}

	// Show Stylesheet
	m_htmlView->show();
	m_htmlView->setHtml( m_parsingOutput );
}

QTextCodec * StyleSheetEditor::codec() const {
	if( dynamic_cast<XslContentViewParser*>( parser() ) && dynamic_cast<XslContentViewParser*>( parser() )->codec() )
		return dynamic_cast<XslContentViewParser*>( parser() )->codec();
	else
		return ContentViewTextEditor::codec();
}

XslCompletionNodeModel * StyleSheetEditor::completionModel() const {
	return m_completionModel;
}

QString StyleSheetEditor::defaultFileName() const {
	return tr( "noname.xsl" );
}

QIcon StyleSheetEditor::icon() const {
	return QIcon( ":/images/typexsl.png" );
}

void StyleSheetEditor::searchWord( const QString & word ) {
	if( ContentViewNode * n = m_completionModel->nodeOfWord( word ) ) {
		emit open( n->fileName(), n->line() );
		return;
	}
	QMessageBox::information( this, tr("Search Word"), tr("Word %1 not found").arg( word ) );
}

bool StyleSheetEditor::autoIndent() {
	try {
		XMLPrettyPrinter prettyPrinter;
		prettyPrinter.setText( textEdit()->toPlainText() );
		prettyPrinter.process();

		textEdit()->textCursor().beginEditBlock();
		textEdit()->editor()->selectAll();
		textEdit()->textCursor().removeSelectedText();
		textEdit()->textCursor().movePosition( 1, QDocumentCursor::Start );
		textEdit()->textCursor().insertText( prettyPrinter.getResult() );
		textEdit()->textCursor().endEditBlock();
	} catch( XMLPrettyPrinterException e ) {
		setMessage( e.getMessage() );
		return false;
	}
	return true;
}

void StyleSheetEditor::launchStylesheetParsing( const QString & xmlfile ) {
	if( ! XINXProjectManager::self()->project() ) {
		qWarning( qPrintable( tr("Please use the project mode for use parsing ;)") ) );
		return;
	}

	try {
		m_parsingOutput = "";
		m_parsingOutputFile     = new QTemporaryFile();
		m_parsingStyleSheetFile = new QTemporaryFile();

		// Output file
		if( ! m_parsingOutputFile->open() ) {
			qWarning( qPrintable( tr("Can't open file %1.").arg( m_parsingOutputFile->fileName() ) ) );
			return;
		}
		QString outputFileName = m_parsingOutputFile->fileName();
		m_parsingOutputFile->close();

		// Parsing Stylesheet
		if( ! m_parsingStyleSheetFile->open() ) {
			qWarning( qPrintable( tr("Can't open file %1.").arg( m_parsingStyleSheetFile->fileName() ) ) );
			return;
		}
		QString styleSheetFileName = m_parsingStyleSheetFile->fileName();

		QTextStream styleSheetFileText( m_parsingStyleSheetFile );
		styleSheetFileText.setCodec( codec() ); // Use the real codec on save
		styleSheetFileText << textEdit()->toPlainText();

		m_parsingStyleSheetFile->close();

		// Parsing Stylesheet
		QString parserType = SelfWebPluginSettings::self()->config().stylesheetParsing.parser.type;
		if( parserType == "oracle" ) {
			QString tool = XINXConfig::self()->getTools( "java" );

			m_parsingProcess->start( tool, QStringList() << "-classpath" << SelfWebPluginSettings::self()->config().stylesheetParsing.parser.oracleParser.classPath << SelfWebPluginSettings::self()->config().stylesheetParsing.parser.oracleParser.mainClass << xmlfile << styleSheetFileName << outputFileName );
		} else if( parserType == "xsltproc" ) {
			QString tool = XINXConfig::self()->getTools( "xsltproc" );

			m_parsingProcess->start( tool, QStringList() << "-o" << outputFileName << "--path" << XINXProjectManager::self()->project()->processedSearchPathList().join( " " ) << styleSheetFileName << xmlfile );
		} else if( parserType == "internal" ) {
			QMessageBox::critical( qApp->activeWindow(), tr("Stylesheet Parsing"), tr("The internal parsing is not yet supported. Please choose other") );
			return;
		}
	} catch( ToolsNotDefinedException e ) {
		return;
	}
}

XmlPresentationDockWidget * StyleSheetEditor::xmlPresentationDockWidget() {
	foreach( XinxPluginElement * element, XinxPluginsLoader::self()->plugins() ) {
		XmlPresPlugin * plugin = qobject_cast<XmlPresPlugin*>( element->plugin() );
		if( plugin )
			return plugin->dock();
	}
	return 0;
}

void StyleSheetEditor::cursorPositionChanged() {
	if( model() ) {
		XslTextEditor * te = qobject_cast<XslTextEditor*>( textEdit() );
		QList<XPathBalise> p = te->xpath(
		                                        te->textCursor(),
		                                        QStringList() << "xsl:template" << "xsl:variable" << "xsl:param",
		                                        QString(),
		                                        QStringList() << "mode" << "name" << "match"
		                                      );

		ContentViewNode * n = rootNode();
		while( p.size() ) {
			XPathBalise b = p.last();
			p.removeLast();

			foreach( ContentViewNode * child, n->childs() ) {
				if( ( b.name == "xsl:template" ) && ( child->data( ContentViewNode::NODE_TYPE ).toString() == "XslTemplate" ) ) {
					if( ( ( child->data().toString() == b.attributes["name"] ) || ( child->data().toString() == b.attributes["match"] ) ) && ( child->data( ContentViewNode::NODE_USER_VALUE ).toString() == b.attributes["mode"] ) ) {
						n = child;
						break;
					}
				} else if( ( b.name == "xsl:variable" ) && ( child->data( ContentViewNode::NODE_TYPE ).toString() == "XslVariable" )  ) {
					if( child->data().toString() == b.attributes["name"] ) {
						n = child;
						break;
					}
				} else if( ( b.name == "xsl:param" ) && ( child->data( ContentViewNode::NODE_TYPE ).toString() == "XslParam" )  ) {
					if( child->data().toString() == b.attributes["name"] ) {
						n = child;
						break;
					}
				}
			}
		}
		QModelIndex indexToSelect = contentViewModel()->index( n );

		emit positionInEditorChanged( indexToSelect );
	}
}
