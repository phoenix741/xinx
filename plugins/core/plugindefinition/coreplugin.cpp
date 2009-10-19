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
#include "coreplugin.h"

#include "config/webpluginformimpl.h"
#include "config/selfwebpluginsettings.h"

#include "filetypeplugin.h"

#include "actions/stylesheetaction.h"

#include <project/xinxproject.h>
#include "pluginproperty/webpluginprojectpropertyformimpl.h"

// Qt header
#include <QStringList>
#include <QHash>
#include <QDir>
#include <QApplication>
#include <QTextEdit>
#include <QTranslator>
#include <QApplication>
#include <QAction>

/* CorePlugin */

CorePlugin::CorePlugin() : m_dock( 0 ) {
	Q_INIT_RESOURCE(coreplugin);

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
	m_fileTypes << new TextFileType;
}

CorePlugin::~CorePlugin() {
	qDeleteAll( m_fileTypes );
	delete SelfWebPluginSettings::self();
}

bool CorePlugin::initializePlugin( const QString & lang ) {
	QTranslator * tranlator = new QTranslator( this );
	tranlator->load( QString(":/translations/coreplugin_%1").arg( lang ) );
	qApp->installTranslator(tranlator);

	return true;
}

QVariant CorePlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
	switch( attr ) {
	case PLG_NAME:
		return tr("Core Plugin");
	case PLG_DESCRIPTION:
		return tr("Necessary editor, actions, and dock used by XINX.");
	case PLG_ICON:
		return QPixmap( ":/images/planet.png" );
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "1.0";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

QList< QPair<QString,QString> > CorePlugin::pluginTools() {
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

QList<IFileTypePlugin*> CorePlugin::fileTypes() {
	return m_fileTypes;
}

XinxAction::MenuList CorePlugin::actions() {
	if( m_menus.size() == 0 ) {
		XinxAction::Action * runAction = new StyleSheetAction( QIcon( ":/images/run.png" ), tr("Process stylesheet"), QString( "F9" ), this );

		XinxAction::ActionList menu( tr("&Execute") );
		menu.append( runAction );
		m_menus.append( menu );
	}
	return m_menus;
}


QWidget * CorePlugin::createSettingsDialog() {
	return new WebPluginFormImpl();
}

bool CorePlugin::loadSettingsDialog( QWidget * widget ) {
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

bool CorePlugin::saveSettingsDialog( QWidget * widget ) {
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

QWidget * CorePlugin::createProjectSettingsPage() {
	return new WebPluginProjectPropertyFormImpl();
}

bool CorePlugin::loadProjectSettingsPage( QWidget * widget ) {
	WebPluginProjectPropertyFormImpl * page = qobject_cast<WebPluginProjectPropertyFormImpl*>( widget );
	Q_ASSERT( page );

	page->m_internetAdresseLineEdit->setText( XINXProjectManager::self()->project()->readProperty( "moduleInternetAdresse" ).toString() );
	return true;
}

bool CorePlugin::saveProjectSettingsPage( QWidget * widget ) {
	WebPluginProjectPropertyFormImpl * page = qobject_cast<WebPluginProjectPropertyFormImpl*>( widget );
	Q_ASSERT( page );

	XINXProjectManager::self()->project()->writeProperty( "moduleInternetAdresse", page->m_internetAdresseLineEdit->text() );
	return true;
}

QList<QWizardPage*> CorePlugin::createNewProjectSettingsPages( int nextid ) {
	QList<QWizardPage*> pages;
	pages << new WebPluginProjectPropertyWizard( nextid );
	return pages;
}

bool CorePlugin::saveNewProjectSettingsPage( XinxProject * project, QWizardPage * page ) {
	WebPluginProjectPropertyWizard * modulePage = qobject_cast<WebPluginProjectPropertyWizard*>( page );
	if( modulePage ) {
		project->writeProperty( "moduleInternetAdresse", modulePage->m_internetAdresseLineEdit->text() );
	}

	return true;
}

QDockWidget * CorePlugin::createDockWidget( QWidget * parent ) {
    if( ! m_dock ) {
        m_dock = new XmlPresentationDockWidget( parent );
        m_dock->setObjectName( QString::fromUtf8( "m_xmlPresDock" ) );
    }
    if( ( m_dock->parent() != parent ) && ( parent != 0 ) ) {
        m_dock->setParent( parent );
    }
    return m_dock;
}

XmlPresentationDockWidget * CorePlugin::dock() {
    return static_cast<XmlPresentationDockWidget*>( createDockWidget( 0 ) );
}

Q_EXPORT_PLUGIN2(coreplugin, CorePlugin)
