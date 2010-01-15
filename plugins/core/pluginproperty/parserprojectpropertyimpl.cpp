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
#include "parserprojectpropertyimpl.h"
#include <project/xinxproject.h>
#include <core/xinxconfig.h>

/* ParserProjectPropertyImpl */

ParserProjectPropertyImpl::ParserProjectPropertyImpl( QWidget * parent, Qt::WindowFlags f ) : QWidget( parent, f ) {
	setupUi( this );
	m_project = XINXProjectManager::self()->project();
	m_dataStreamEdit->setDefaultValue( XINXConfig::self()->config().project.defaultPath );
}

ParserProjectPropertyImpl::~ParserProjectPropertyImpl() {
}

void ParserProjectPropertyImpl::setProject( XinxProject * project ) {
	m_project = project;
}

QPixmap ParserProjectPropertyImpl::image() {
	return QPixmap();
}

QString ParserProjectPropertyImpl::name() {
	return this->windowTitle();
}

bool ParserProjectPropertyImpl::loadSettingsDialog() {
	const QString moduleAddress = m_project->readProperty( "moduleInternetAdresse" ).toString();

	if( QFileInfo( moduleAddress ).isRelative() )
		m_urlLocationEdit->setText( QDir( m_project->projectPath() ).absoluteFilePath( moduleAddress ) );
	else
		m_urlLocationEdit->setText( moduleAddress );

	m_dataStreamEdit->lineEdit()->setText( QDir( m_project->projectPath() ).absoluteFilePath( m_project->readProperty( "dataStreamLocation" ).toString() ) );
	return true;
}

bool ParserProjectPropertyImpl::saveSettingsDialog() {
	m_project->writeProperty( "moduleInternetAdresse", QDir( m_project->projectPath() ).relativeFilePath(  m_urlLocationEdit->text() ) );
	m_project->writeProperty( "dataStreamLocation", QDir( m_project->projectPath() ).relativeFilePath( m_dataStreamEdit->lineEdit()->text() ) );
	return true;
}

bool ParserProjectPropertyImpl::cancelSettingsDialog() {
	return true;
}

QWidget * ParserProjectPropertyImpl::settingsDialog() {
	return this;
}

bool ParserProjectPropertyImpl::isSettingsValid() {
	return true;
}

bool ParserProjectPropertyImpl::isVisible() {
	return true;
}

/* WebPluginProjectPropertyWizard */

WebPluginProjectPropertyWizard::WebPluginProjectPropertyWizard() {
	setTitle( tr("Web Module Property Page") );
	setSubTitle( tr("Define information about the web module, like the adresse of the servlet control.") );

	QGridLayout * gridLayout = new QGridLayout( this );

	QLabel * labelUrl = new QLabel( this );
	gridLayout->addWidget( labelUrl, 0, 0 );

	m_urlLocationEdit = new QLineEdit( this );
	gridLayout->addWidget( m_urlLocationEdit, 0, 1 );

	QLabel * labelDataStream = new QLabel( this );
	gridLayout->addWidget( labelDataStream, 1, 0 );

	m_dataStreamEdit = new DirectoryEditWidget( this );
	gridLayout->addWidget( m_dataStreamEdit, 1, 1 );

	gridLayout->addItem( new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 0 );

	labelUrl->setBuddy(m_urlLocationEdit);
	labelUrl->setText( tr("Location of the &running page :") );
	m_urlLocationEdit->setText( field( "project.path" ).toString() );

	labelDataStream->setBuddy(m_dataStreamEdit);
	labelDataStream->setText( tr("Location of data stream :") );
	m_dataStreamEdit->lineEdit()->setText( field( "project.path" ).toString() );

	registerField( "webplugin.adresse", m_urlLocationEdit );
	registerField( "webplugin.dataStream", m_urlLocationEdit );
}

void WebPluginProjectPropertyWizard::initializePage() {
	m_dataStreamEdit->lineEdit()->setText( field( "project.path" ).toString() );
}

QString WebPluginProjectPropertyWizard::pagePluginId() const {
	return "CoreInformation";
}

bool WebPluginProjectPropertyWizard::pageIsVisible() const {
	return true;
}

bool WebPluginProjectPropertyWizard::saveSettingsDialog( XinxProject * project ) {
	project->writeProperty( "dataStreamLocation", m_dataStreamEdit->lineEdit()->text() );
	project->writeProperty( "moduleInternetAdresse", m_urlLocationEdit->text() );

	return true;
}
