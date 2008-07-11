/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "servicesplugin.h"
#include "servicesprojectpropertyimpl.h"
#include "servicesprojectwizard.h"
#include <xslproject.h>

// Qt header
#include <QString>
#include <QVariant>
#include <QTranslator>
#include <QApplication>

/* ServicesPlugin */

ServicesPlugin::ServicesPlugin() {
    Q_INIT_RESOURCE(servicesplugin);
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

QWidget * ServicesPlugin::createProjectSettingsPage() {
	return new ServicesProjectPropertyImpl();
}

bool ServicesPlugin::initializeProject( XSLProject * project ) {
	if( project->options().testFlag( XSLProject::hasWebServices ) ) {
		return false;
	} else
		return true;
}

bool ServicesPlugin::loadProjectSettingsPage( QWidget * widget ) {
	ServicesProjectPropertyImpl * page = qobject_cast<ServicesProjectPropertyImpl*>( widget );
	Q_ASSERT( page );

	XSLProject::ProjectOptions options = XINXProjectManager::self()->project()->options();
	page->m_webServiceGroupBox->setChecked( options.testFlag( XSLProject::hasWebServices ) );
	page->m_servicesList->setValues( XINXProjectManager::self()->project()->readProperty( "webServiceLink" ).toString().split(";;",QString::SkipEmptyParts) );
	return true;
}

bool ServicesPlugin::saveProjectSettingsPage( QWidget * widget ) {
	ServicesProjectPropertyImpl * page = qobject_cast<ServicesProjectPropertyImpl*>( widget );
	Q_ASSERT( page );

	XSLProject::ProjectOptions options = XINXProjectManager::self()->project()->options();
	if( page->m_webServiceGroupBox->isChecked() )
		options |= XSLProject::hasWebServices;
	else
		options &= ~XSLProject::hasWebServices;
	XINXProjectManager::self()->project()->setOptions( options );

	XINXProjectManager::self()->project()->writeProperty( "webServiceLink", page->m_servicesList->values().join(";;") );
	return true;
}

QList<QWizardPage*> ServicesPlugin::createNewProjectSettingsPages( int nextid ) {
	QList<QWizardPage*> pages;
	pages << new ServicesPageImpl( nextid );
	pages << new ServicesListPageImpl( nextid );
	return pages;
}

bool ServicesPlugin::saveNewProjectSettingsPage( XSLProject * project, QWizardPage * page ) {
	ServicesListPageImpl * servicesPage = qobject_cast<ServicesListPageImpl*>( page );
	if( servicesPage->field( "project.services" ).toBool() ) {
		XSLProject::ProjectOptions options = project->options();
		options |= XSLProject::hasWebServices;
		project->setOptions( options );

		QStringList & services = project->serveurWeb();
		foreach( QString value, servicesPage->m_webServicesWidget->values() )
			services += value;
	}

	return false;
}

Q_EXPORT_PLUGIN2(servicesplugin, ServicesPlugin)
