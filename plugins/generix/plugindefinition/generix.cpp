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
#include "generix.h"
#include <contentview/contentviewnode.h>
#include "docks/dictionary/dictionaryparser.h"
#include <project/xinxproject.h>
#include "pluginresolver/gce150fileresolver.h"
#include <config/customgeneriximpl.h>
#include "projectproperty/wizard/newgenerixinformationpageimpl.h"
#include "projectproperty/wizard/newgenerixderivation1page.h"
#include "projectproperty/wizard/newgenerixderivation2page.h"
#include "projectproperty/wizard/newgenerixderivation3page.h"
#include "projectproperty/std/generixderivationoptionspage.h"
#include "projectproperty/std/generixderivationpage.h"
#include "projectproperty/std/generixprojectpageform.h"
#include <project/externalfileresolver.h>
#include <configuration/configurationmanager.h>
#include <savedialog/derivationdialogimpl.h>
#include <rcs/rcsmanager.h>
#include "parser/generixxsltparser.h"

// Qt header
#include <QString>
#include <QVariant>
#include <QTranslator>
#include <QApplication>
#include <QDockWidget>
#include <QDir>

/* GenerixPlugin */

GenerixPlugin::GenerixPlugin() : m_dock( 0 ) {
	Q_INIT_RESOURCE(generix);
}

GenerixPlugin::~GenerixPlugin() {
}

bool GenerixPlugin::initializePlugin( const QString & lang ) {
	QTranslator * tranlator = new QTranslator( this );
	tranlator->load( QString(":/generix/translations/generix_%1").arg( lang ) );
	qApp->installTranslator(tranlator);

	m_resolver = new Gce150FileResolver();

	return true;
}

QVariant GenerixPlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
	switch( attr ) {
	case PLG_NAME:
		return tr("Plugin for Generix");
	case PLG_DESCRIPTION:
		return tr("This plugin add some fonctionnality usefull for Generix Group's XINX users");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
    case PLG_ICON:
		return QPixmap( ":/generix/images/logo_gce32.png" );
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

XsltParser * GenerixPlugin::createParser() {
	GenerixProject * project = static_cast<GenerixProject*>( XINXProjectManager::self()->project() );
	if( project && project->isGenerixActivated() ) {
		return new GenerixXsltParser();
	} else
		return 0;
}

bool GenerixPlugin::loadProject( XinxProject * project ) {
	Q_UNUSED( project );

	ConfigurationManager::self()->getInterfaceOfProject( project );

	return true;
}

bool GenerixPlugin::closeProject( XinxProject * project ) {
	Q_UNUSED( project );

	ConfigurationManager::self()->cleanCache();

	return true;
}

QIODevice * GenerixPlugin::loadFile( const QString & filename ) {
	Q_UNUSED( filename );
	return 0;
}

QIODevice * GenerixPlugin::saveFile( const QString & filename, const QString & oldfilename ) {
	GenerixProject * gnxProject = static_cast<GenerixProject*>( XINXProjectManager::self()->project() );
	if( !(gnxProject && gnxProject->isGenerixActivated() && gnxProject->copySourceFileInDerivationPath()) || ( filename == oldfilename ) ) {
		return 0;
	}

	/* When saving the file, the plugin the standard file in the project directory */
	const QString stdfilename = QDir( QFileInfo( filename ).absolutePath() ).absoluteFilePath( QFileInfo( oldfilename ).fileName() );

	if( ! QFile::exists( stdfilename ) ) {
		QFile::copy( oldfilename, stdfilename );
		RCSManager::self()->addFileOperation( RCSManager::RCS_ADD, QStringList() << stdfilename );
	}
	return 0;
}

QString GenerixPlugin::getFilename( const QString & filename, const QString & filter, bool saveAs, bool & accept, QWidget * widget ) {
	GenerixProject * gnxProject = static_cast<GenerixProject*>( XINXProjectManager::self()->project() );
	if( !(gnxProject && gnxProject->isGenerixActivated()) ) {
		accept = false;
		return QString();
	}

	accept = true;
	if( saveAs || DerivationDialogImpl::isDerivableFile( filename ) ) {
		DerivationDialogImpl dlg( widget );
		dlg.load( filename, filter );
		if( dlg.exec() == QDialog::Accepted ) {
			const QString filename = dlg.getNewPath();
			const QString pathname = QFileInfo( filename ).absolutePath();

			if( gnxProject->createMissingDirectory() )
				QDir::current().mkpath( pathname );

			return filename;
		} else {
			return QString();
		}
	}
	return filename;
}

QList<QDockWidget*> GenerixPlugin::createDocksWidget( QWidget * parent ) {
	QList<QDockWidget*> docks;
	if( ! m_dock ) {
		m_dock = new DictionaryDockWidgetImpl( parent );
		m_dock->setObjectName( QString::fromUtf8( "m_dictionaryDock" ) );

		m_gnxDock = new GenerixProjectDockImpl( parent );
		m_gnxDock->setObjectName( QString::fromUtf8( "m_gnxDock" ) );
	}
	docks << m_dock << m_gnxDock;
	return docks;
}

QList<IFileResolverPlugin*> GenerixPlugin::fileResolvers() {
	return QList<IFileResolverPlugin*>() << m_resolver;
}

QList<IXinxPluginConfigurationPage*> GenerixPlugin::createSettingsDialog( QWidget * parent ) {
	QList<IXinxPluginConfigurationPage*> pages;
	pages << new CustomGenerixImpl( parent );
	return pages;
}

QList<IXinxPluginProjectConfigurationPage*> GenerixPlugin::createProjectSettingsPage( QWidget * parent ) {
	QList<IXinxPluginProjectConfigurationPage*> pages;

	pages << new GenerixProjectPageFormImpl( parent );
	pages << new GenerixDerivationPathPageImpl( parent );
	pages << new GenerixDerivationOptionsPageImpl( parent );

	return pages;
}

QList<IXinxPluginNewProjectConfigurationPage*> GenerixPlugin::createNewProjectSettingsPages() {
	QList<IXinxPluginNewProjectConfigurationPage*> list;
	list << new NewGenerixInformationPageImpl;
	list << new NewGenerixDerivation1Page;
	list << new NewGenerixDerivation2Page;
	list << new NewGenerixDerivation3Page;
	return list;
}


Q_EXPORT_PLUGIN2(GenerixPlugin, GenerixPlugin)
