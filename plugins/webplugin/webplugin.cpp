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
#include "webplugin.h"

#include "config/webpluginformimpl.h"
#include "config/selfwebpluginsettings.h"

#include "filetypeplugin.h"

#include "xsl/stylesheetaction.h"

// Qt header
#include <QStringList>
#include <QHash>
#include <QDir>
#include <QApplication>
#include <QTextEdit>
#include <QTranslator>
#include <QApplication>
#include <QAction>

/* BasePlugin */

WebPlugin::WebPlugin() {
	Q_INIT_RESOURCE(webplugin);

	qRegisterMetaType<StyleSheetEditor>( "StyleSheetEditor" );
	qRegisterMetaType<XmlFileEditor>( "XmlFileEditor" );
	qRegisterMetaType<HtmlFileEditor>( "HtmlFileEditor" );
	qRegisterMetaType<JSFileEditor>( "JSFileEditor" );
	qRegisterMetaType<CSSFileEditor>( "CSSFileEditor" );
	qRegisterMetaType<XQFileEditor>( "XQFileEditor" );

	m_fileTypes << new XSLStyleSheetFileType;
	m_fileTypes << new XMLFileType;
	m_fileTypes << new HTMLFileType;
	m_fileTypes << new JSFileType;
	m_fileTypes << new CSSFileType;
	m_fileTypes << new XQFileType;
}

WebPlugin::~WebPlugin() {
	qDeleteAll( m_fileTypes );
	delete SelfWebPluginSettings::self();
}

bool WebPlugin::initializePlugin( const QString & lang ) {
	QTranslator * tranlator = new QTranslator( this );
	tranlator->load( QString(":/translations/webplugin_%1").arg( lang ) );
	qApp->installTranslator(tranlator);

	return true;
}

QVariant WebPlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
	switch( attr ) {
	case PLG_NAME:
		return tr("Web Plugin");
	case PLG_DESCRIPTION:
		return tr("Editor helper for HTML, XSL, XML, JS, CSS files.");
	case PLG_ICON:
		return QPixmap( ":/images/planet.png" );
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "0.1";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

QList< QPair<QString,QString> > WebPlugin::pluginTools() {
	QList< QPair<QString,QString> > tools;
#ifdef Q_WS_WIN
	tools.append( qMakePair( QString("java"), QString("c:\\jdk") ) );
	tools.append( qMakePair( QString("viewer"), QString("c:\\Program Files\\Internet Explorer\\iexplorer.exe") ) );
	tools.append( qMakePair( QString("xsltproc"), QString() ) );
#else
	tools.append( qMakePair( QString("java"), QString("/usr/bin/java") ) );
	tools.append( qMakePair( QString("viewer"), QString("/usr/bin/www-browser") ) );
	tools.append( qMakePair( QString("xsltproc"), QString("/usr/bin/xsltproc") ) );
#endif // Q_WS_WIN
	return tools;
}

QList<IFileTypePlugin*> WebPlugin::fileTypes() {
	return m_fileTypes;
}

XinxAction::MenuList WebPlugin::actions() {
	if( m_menus.size() == 0 ) {
		XinxAction::Action * runAction = new StyleSheetAction( QIcon( ":/images/run.png" ), tr("Process stylesheet"), QString( "F9" ), this );

		XinxAction::ActionList menu( tr("Execute") );
		menu.append( runAction );
		m_menus.append( menu );
	}
	return m_menus;
}


QWidget * WebPlugin::createSettingsDialog() {
	return new WebPluginFormImpl();
}

bool WebPlugin::loadSettingsDialog( QWidget * widget ) {
	SelfWebPluginSettings * settings = SelfWebPluginSettings::self();
	WebPluginFormImpl * form = qobject_cast<WebPluginFormImpl*>( widget );
	Q_ASSERT( form );

	form->m_activeXmlGroupBox->setChecked( settings->config().xml.activeCompletion );
	form->m_addClosedBaliseCheckBox->setChecked( settings->config().xml.addClosedBalise );
	form->m_addDefaultAttributeCheckBox->setChecked( settings->config().xml.addDefaultAttribute );
	form->m_addDefaultSubBaliseCheckBox->setChecked( settings->config().xml.addDefaultSubBalise );

	if( settings->config().stylesheetParsing.viewer.type == "internal" ) {
		form->m_viewerComboBox->setCurrentIndex( 0 );
	} else if( settings->config().stylesheetParsing.viewer.type == "external" ) {
		form->m_viewerComboBox->setCurrentIndex( 1 );
	}

	if( settings->config().stylesheetParsing.parser.type == "oracle" ) {
		form->m_parserComboBox->setCurrentIndex( 0 );
	} else if( settings->config().stylesheetParsing.parser.type == "xsltproc" ) {
		form->m_parserComboBox->setCurrentIndex( 1 );
	} else if( settings->config().stylesheetParsing.parser.type == "internal" ) {
		form->m_parserComboBox->setCurrentIndex( 2 );
	}

	form->m_classPathEdit->lineEdit()->setText( settings->config().stylesheetParsing.parser.oracleParser.jarName );
	form->m_classNameEdit->setText( settings->config().stylesheetParsing.parser.oracleParser.classPath );
	form->m_parameterEdit->setText( settings->config().stylesheetParsing.parser.oracleParser.parameters );

	form->m_javaScriptGroupBox->setChecked( settings->config().javascript.activeCompletion );
	return true;
}

bool WebPlugin::saveSettingsDialog( QWidget * widget ) {
	SelfWebPluginSettings * settings = SelfWebPluginSettings::self();
	WebPluginFormImpl * form = qobject_cast<WebPluginFormImpl*>( widget );
	Q_ASSERT( form );

	settings->config().xml.activeCompletion = form->m_activeXmlGroupBox->isChecked();
	settings->config().xml.addClosedBalise = form->m_addClosedBaliseCheckBox->isChecked();
	settings->config().xml.addDefaultAttribute = form->m_addDefaultAttributeCheckBox->isChecked();
	settings->config().xml.addDefaultSubBalise = form->m_addDefaultSubBaliseCheckBox->isChecked();

	switch( form->m_viewerComboBox->currentIndex() ) {
	case 0:
		settings->config().stylesheetParsing.viewer.type = "internal";
		break;
	case 1:
		settings->config().stylesheetParsing.viewer.type = "external";
		break;
	}
	switch( form->m_parserComboBox->currentIndex() ) {
	case 0:
		settings->config().stylesheetParsing.parser.type = "oracle";
		break;
	case 1:
		settings->config().stylesheetParsing.parser.type = "xsltproc";
		break;
	case 2:
		settings->config().stylesheetParsing.parser.type = "internal";
		break;
	}

	settings->config().stylesheetParsing.parser.oracleParser.jarName    = form->m_classPathEdit->lineEdit()->text();
	settings->config().stylesheetParsing.parser.oracleParser.classPath  = form->m_classNameEdit->text();
	settings->config().stylesheetParsing.parser.oracleParser.parameters = form->m_parameterEdit->text();

	settings->config().javascript.activeCompletion = form->m_javaScriptGroupBox->isChecked();

	settings->save();
	return true;
}

Q_EXPORT_PLUGIN2(webplugin, WebPlugin)
