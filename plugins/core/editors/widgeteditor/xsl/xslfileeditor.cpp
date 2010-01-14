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
#include <contentview/contentviewnode.h>
#include <contentview/contentviewmodel.h>
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

/* StyleSheetEditor */

StyleSheetEditor::StyleSheetEditor( QWidget *parent ) : ContentViewTextEditor( new XslContentViewParser(), new XslTextEditor(), parent ) {
	initObjects();
}

StyleSheetEditor::~StyleSheetEditor() {
	ContentViewParser * p = parser();
	setParser( 0 );
	qobject_cast<XslTextEditor*>( textEdit() )->setModel( 0 );
	delete m_completionModel;
	delete p;
}

void StyleSheetEditor::initObjects() {
	m_htmlView = new QWebView( this );
	m_htmlView->load( QUrl( "about:blank" ) );
	m_htmlView->setMinimumHeight( 100 );

	m_sourceView = new XslTextEditor( this );

	connect( textEdit()->editor(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()) );

	m_completionModel = new XslCompletionNodeModel( rootNode(), this );
	m_completionModel->setCompleteTags( XslCompletionNodeModel::NoTags );

	QCompleter * completer = new QCompleter( textEdit() );
	completer->setModel( m_completionModel );
	textEdit()->setCompleter( completer );
	qobject_cast<XslTextEditor*>( textEdit() )->setModel( m_completionModel );

	m_tabWidget = new QTabWidget( this );
	m_tabWidget->setTabShape( QTabWidget::Triangular );
	m_tabWidget->setTabPosition( QTabWidget::South );
	connect( m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabIndexChange(int)) );

	XmlPresentationDockWidget * dock = XmlPresentationDockWidget::self();
	if( dock ) {
		connect( dock, SIGNAL(filenameChanged(QString)), this, SLOT(xmlPresentationDockFilenameChanged(QString)) );
	}
}

void StyleSheetEditor::initLayout() {
	m_tabWidget->addTab( textEdit(), tr("XSL Source") );
	m_tabWidget->addTab( m_sourceView, tr("HTML Source") );
	m_tabWidget->addTab( m_htmlView, tr("Web View") );

	XmlPresentationDockWidget * dock = XmlPresentationDockWidget::self();
	if( dock )
		xmlPresentationDockFilenameChanged( dock->filename() );

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget( m_tabWidget );

	hbox->setMargin( 0 );

	setLayout( hbox );
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
		emit message( lastFileName(), e.getMessage(), ERROR_MESSAGE );
		return false;
	}
	return true;
}

void StyleSheetEditor::xmlPresentationDockFilenameChanged( const QString & filename ) {
	m_tabWidget->setTabEnabled( 1, ! filename.isEmpty() );
	m_tabWidget->setTabEnabled( 2, ! filename.isEmpty() );
}

void StyleSheetEditor::tabIndexChange( int index ) {
	XmlPresentationDockWidget * dock = XmlPresentationDockWidget::self();
	if( dock && ( ! dock->filename().isEmpty() ) && ( ( index == 1 ) || ( index == 2 ) ) ) {
		launchStylesheetParsing( dock->filename() );
	}
}

void StyleSheetEditor::launchStylesheetParsing( const QString & xmlfile ) {
	XsltParser xsltParser;

	if( ! XINXProjectManager::self()->project() ) {
		qWarning( qPrintable( tr("Please use the project mode for use parsing ;)") ) );
		return;
	}
	QString moduleInternetAdresse = XINXProjectManager::self()->project() ?
			QDir( XINXProjectManager::self()->project()->projectPath() ).absoluteFilePath(
				XINXProjectManager::self()->project()->readProperty( "moduleInternetAdresse" ).toString()
				) : lastFileName();
	if( moduleInternetAdresse.isEmpty() ) {
		qWarning( qPrintable( tr( "Please give the internet adresse of the servlet control of the web module (like http://localhost/ear/war/dir/Servlet) in property project" ) ) );
	}

	QString result;
	QString utf8Text = textEdit()->toPlainText();
	QByteArray byte  = codec()->fromUnicode( utf8Text );

	//if( ! xsltParser.loadStylesheet( byte ) ) goto error;
	if( ! xsltParser.loadStylesheet( textEdit()->filename() ) ) goto error;
	if( ! xsltParser.loadXmlFile( xmlfile ) ) goto error;
	if( ! xsltParser.process() )              goto error;

	result = xsltParser.getOutput();
	m_sourceView->setPlainText( result );
	m_htmlView->setHtml( result, QUrl( moduleInternetAdresse ) );

	return;
error:
	qWarning( qPrintable( tr( "Can't parse stylesheet" ) ) );
}

XmlPresentationDockWidget * StyleSheetEditor::xmlPresentationDockWidget() {
	foreach( XinxPluginElement * element, XinxPluginsLoader::self()->plugins() ) {
		CorePlugin * plugin = qobject_cast<CorePlugin*>( element->plugin() );
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
