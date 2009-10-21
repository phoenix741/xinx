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
#include "cvsthread.h"

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

QList<IXinxPluginConfigurationPage*> CVSPlugin::createSettingsDialog() {
	return QList<IXinxPluginConfigurationPage*>();
}
/*
QWidget * CVSPlugin::createSettingsDialog() {
	return new CustomizePlugin();
}

bool CVSPlugin::loadSettingsDialog( QWidget * widget ) {
	CustomizePlugin * dlg = qobject_cast<CustomizePlugin*>( widget );
	Q_ASSERT( dlg );

	// CVS: Progress message
	if( m_settings->config().progressMessages.isEmpty() ) 
		dlg->m_cvsVerboseComboBox->setCurrentIndex( 2 );
	else
	if( m_settings->config().progressMessages == "-q" ) 
		dlg->m_cvsVerboseComboBox->setCurrentIndex( 1 );
	else
	if( m_settings->config().progressMessages == "-Q" ) 
		dlg->m_cvsVerboseComboBox->setCurrentIndex( 0 );
	
	// CVS: Compression
	dlg->m_cvsCompressionComboBox->setCurrentIndex( m_settings->config().compressionLevel );
	
	// CVS: Prune empty directories
	dlg->m_cvsPruneCheckBox->setChecked( m_settings->config().pruneEmptyDirectories );
	
	// CVS: Create any directories that exist in the repository
	dlg->m_cvsCreateDirCheckBox->setChecked( m_settings->config().createDirectories );
	
	return true;
}

bool CVSPlugin::saveSettingsDialog( QWidget * widget ) {
	CustomizePlugin * dlg = qobject_cast<CustomizePlugin*>( widget );
	Q_ASSERT( dlg );

	// CVS: Progress message
	switch( dlg->m_cvsVerboseComboBox->currentIndex() ) {
	case 2: 
		m_settings->config().progressMessages = "";
		break;
	case 1: 
		m_settings->config().progressMessages = "-q";
		break;
	case 0: 
		m_settings->config().progressMessages = "-Q";
		break;
	}

	// CVS: Compression
	m_settings->config().compressionLevel = dlg->m_cvsCompressionComboBox->currentIndex();
	
	// CVS: Prune empty directories
	m_settings->config().pruneEmptyDirectories = dlg->m_cvsPruneCheckBox->isChecked();
	
	// CVS: Create any directories that exist in the repository
	m_settings->config().createDirectories = dlg->m_cvsCreateDirCheckBox->isChecked();

	m_settings->save();
	return true;
}
*/

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
		CVSThread::setPluginSettings( m_settings );
		return new RCS_CVS( basePath );
	}
	return NULL;
}

Q_EXPORT_PLUGIN2(cvsplugin, CVSPlugin)
