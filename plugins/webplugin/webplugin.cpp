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

// Qt header
#include <QStringList>
#include <QHash>
#include <QDir>
#include <QApplication>
#include <QTextEdit>
#include <QTranslator>
#include <QApplication>

/* BasePlugin */

WebPlugin::WebPlugin() {
	Q_INIT_RESOURCE(webplugin);

	qRegisterMetaType<XslContentEditor>( "XslContentEditor" );
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

QList<IFileTypePlugin*> WebPlugin::fileTypes() {
	return m_fileTypes;
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

	settings->config().javascript.activeCompletion = form->m_javaScriptGroupBox->isChecked();

	settings->save();
	return true;
}

Q_EXPORT_PLUGIN2(webplugin, WebPlugin)
