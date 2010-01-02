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
#include "newgenerixderivation2page.h"
#include "projectproperty/generixproject.h"
#include "configuration/configurationfile.h"
#include "config/selfgenerixsettings.h"

// Qt header
#include <QDir>

/* NewGenerixDerivation2Page */

NewGenerixDerivation2Page::NewGenerixDerivation2Page() {
	setupUi( this );

	registerField( "generix.derivationPath", m_derivationPath );
	registerField( "generix.sourcePath",     m_sourcePath );

	m_derivationLabel->setBuddy( m_derivationPath );
	m_sourceLabel->setBuddy( m_sourcePath );
}

void NewGenerixDerivation2Page::initializePage() {
	ConfigurationVersion version = field( "generix.version" ).value<ConfigurationVersion>();
	ConfigurationVersion version140( 6, 1, 40 );

	QString projet = SelfGenerixSettings::self()->config().defaultProjectPathName;

	QString destPath, srcPath;
	destPath = srcPath = field( "generix.webmodule" ).toString();
	if( version <= version140 ) {
		srcPath = QDir( srcPath ).absoluteFilePath( "langue/fra/nav/" );
		destPath = QDir( srcPath ).absoluteFilePath( projet );
	} else {
		srcPath = QDir( srcPath ).absoluteFilePath( "presentation/.*" );
		destPath = QDir( destPath ).absoluteFilePath( "presentation/" + projet );
	}
	m_derivationPath->lineEdit()->setText( destPath );
	m_sourcePath->lineEdit()->setText( srcPath );
}

QString NewGenerixDerivation2Page::pagePluginId() const {
	return "GenerixDerivation2";
}

bool NewGenerixDerivation2Page::pageIsVisible() const {
	return field( "generix.derivation" ).toBool();
}

bool NewGenerixDerivation2Page::saveSettingsDialog( XinxProject * project ) {
	QDir path;
	path.mkpath( m_derivationPath->lineEdit()->text() );
	//static_cast<GenerixProject*>( project )->setDerivationPath( m_derivationPath->lineEdit()->text() );
	return true;
}
