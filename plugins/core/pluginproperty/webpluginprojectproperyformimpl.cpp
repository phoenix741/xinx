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
#include "webpluginprojectpropertyformimpl.h"

/* WebPluginProjectPropertyFormImpl */

WebPluginProjectPropertyFormImpl::WebPluginProjectPropertyFormImpl( QWidget * parent, Qt::WindowFlags f ) : QWidget( parent, f ) {
	setupUi( this );
}

WebPluginProjectPropertyFormImpl::~WebPluginProjectPropertyFormImpl() {
}

/* WebPluginProjectPropertyWizard */

WebPluginProjectPropertyWizard::WebPluginProjectPropertyWizard( int nextId, QWidget * parent ) : QWizardPage( parent ), m_nextId( nextId ) {
	setupUi( this );
	setTitle( tr("Web Module Property Page") );
	setSubTitle( tr("Define information about the web module, like the adresse of the servlet control.") );

	m_internetAdresseLineEdit->setText( "http://localhost:8888/gce/btoe/GCE/ServletControl" );

	registerField( "webplugin.adresse", m_internetAdresseLineEdit );
}

int WebPluginProjectPropertyWizard::nextId() const {
	return m_nextId + 1;
}
