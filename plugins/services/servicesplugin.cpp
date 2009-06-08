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
#include <core/xinxproject.h>
#include "servicesplugin.h"
#include "servicesprojectpropertyimpl.h"
#include "servicesprojectwizard.h"
#include "webservices.h"
#include "webservicesfiletype.h"
#include "webservicesaction.h"

// Qt header
#include <QString>
#include <QVariant>
#include <QTranslator>
#include <QApplication>

/* ServicesPlugin */

ServicesPlugin::ServicesPlugin() {
	Q_INIT_RESOURCE(servicesplugin);

	qRegisterMetaType<WebServicesEditor>( "WebServicesEditor" );

	WebServicesManager::self();
	m_fileTypes << new WebServicesFileType;
}

ServicesPlugin::~ServicesPlugin() {
	qDeleteAll( m_fileTypes );
}

bool ServicesPlugin::initializePlugin( const QString & lang ) {
	QTranslator * tranlator = new QTranslator( this );
	tranlator->load( QString(":/services/translations/servicesplugin_%1").arg( lang ) );
	qApp->installTranslator(tranlator);

	return true;
}

QVariant ServicesPlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
	switch( attr ) {
	case PLG_NAME:
		return tr("XINX WebServices");
	case PLG_DESCRIPTION:
		return tr("Enable the WebServices Capabilities to XINX.");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
    case PLG_ICON:
		return QPixmap( ":/services/images/webservices.png" );
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

QList<IFileTypePlugin*> ServicesPlugin::fileTypes() {
	return m_fileTypes;
}

XinxAction::MenuList ServicesPlugin::actions() {
	if( m_menus.size() == 0 ) {
		XinxAction::Action * refreshAction = new WebServicesRefreshAction( QIcon(":/images/reload.png"), tr("Update WebServices List"), QString(), this );
		XinxAction::Action * runAction = new WebServicesRefreshAction( QIcon(":/images/run.png"), tr("Call the service"), QString( "F9" ), this );

		XinxAction::ActionList menu( tr("&Execute") );
		menu.append( new XinxAction::Separator() );
		menu.append( refreshAction );
		menu.append( runAction );
		m_menus.append( menu );
	}
	return m_menus;
}

QWidget * ServicesPlugin::createProjectSettingsPage() {
	return new ServicesProjectPropertyImpl();
}

bool ServicesPlugin::loadProjectSettingsPage( QWidget * widget ) {
	ServicesProjectPropertyImpl * page = qobject_cast<ServicesProjectPropertyImpl*>( widget );
	Q_ASSERT( page );

	page->loadProjectProperty();
	return true;
}

bool ServicesPlugin::saveProjectSettingsPage( QWidget * widget ) {
	ServicesProjectPropertyImpl * page = qobject_cast<ServicesProjectPropertyImpl*>( widget );
	Q_ASSERT( page );

	page->saveProjectProperty();
	return true;
}

QList<QWizardPage*> ServicesPlugin::createNewProjectSettingsPages( int nextid ) {
	QList<QWizardPage*> pages;
	pages << new ServicesPageImpl( nextid );
	pages << new ServicesListPageImpl( nextid );
	return pages;
}

bool ServicesPlugin::saveNewProjectSettingsPage( XinxProject * project, QWizardPage * page ) {
	ServicesListPageImpl * servicesPage = qobject_cast<ServicesListPageImpl*>( page );
	if( servicesPage && servicesPage->field( "project.services" ).toBool() ) {
		XinxProject::ProjectOptions options = project->options();
		options |= XinxProject::hasWebServices;
		project->setOptions( options );

		QStringList services;
		foreach( const QString & value, servicesPage->m_webServicesWidget->values() )
			services += value;


		int index = 0;
		foreach( const QString & link, services ) {
			project->writeProperty( QString( "webServiceLink_%1" ).arg( index ), link );
			index++;
		}
	}

	return true;
}

Q_EXPORT_PLUGIN2(servicesplugin, ServicesPlugin)
