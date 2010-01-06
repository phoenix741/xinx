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
#include "cvsplugin.h"
#include "customizeplugin.h"
#include "rcs_cvs.h"

// Qt header
#include <QString>
#include <QVariant>
#include <QSettings>
#include <QTranslator>

/* CVSPlugin */

CVSPlugin::CVSPlugin() {
    Q_INIT_RESOURCE(cvsplugin);
	m_settings = new PluginSettings();
}

CVSPlugin::~CVSPlugin() {
	delete m_settings;
}

bool CVSPlugin::initializePlugin( const QString & lang ) {
	QTranslator * tranlator = new QTranslator( this );
	tranlator->load( QString(":/translations/cvsplugin_%1").arg( lang ) );
	qApp->installTranslator(tranlator);
	
	Q_ASSERT( m_settings );
	m_settings->load();

	return true;
}

QVariant CVSPlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
	switch( attr ) {
	case PLG_NAME:
		return tr("Wrapper of CVS");
	case PLG_DESCRIPTION:
		return tr("Enable using CVS as a wrapper of the cvs commande.");
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
	case PLG_ICON:
		return QPixmap(":/images/hi32-app-cervisia.png");
	default:
		;
	}
	return QVariant();
}

QList<IXinxPluginConfigurationPage*> CVSPlugin::createSettingsDialog( QWidget * parent ) {
	QList<IXinxPluginConfigurationPage*> list;
	list << new CustomizePlugin( m_settings, parent );
	return list;
}


QList< QPair<QString,QString> > CVSPlugin::pluginTools() {
	QList< QPair<QString,QString> > tools;
#ifdef Q_WS_WIN
	tools.append( qMakePair( QString("cvs"), QString("%1/CVSNT/cvs.exe").arg( "C:/Program Files" ) ) );
#else
	tools.append( qMakePair( QString("cvs"), QString("/usr/bin/cvs") ) );
#endif // Q_WS_WIN
	return tools;
}

QStringList CVSPlugin::rcs() {
	return QStringList() << "cvs";
}

QString CVSPlugin::descriptionOfRCS( const QString & rcs ) {
	if( rcs.toLower() == "cvs" )
		return tr( "CVS - Concurent Version System" );
	return QString();
}

RCS * CVSPlugin::createRCS( const QString & rcs, const QString & basePath ) {
	if( rcs.toLower() == "cvs" ) {
		RCS_CVS * rcs = new RCS_CVS( basePath );
		rcs->setPluginSettings( m_settings );
		return rcs;
	}
	return NULL;
}

Q_EXPORT_PLUGIN2(cvsplugin, CVSPlugin)
