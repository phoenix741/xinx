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

#include "xmlpresconfigformimpl.h"
#include "config/selfwebpluginsettings.h"

/* XmlPresConfigFormImpl */

XmlPresConfigFormImpl::XmlPresConfigFormImpl( QWidget * parent ) : QWidget( parent ) {
	setupUi( this );
	
	// XML Pres
	m_hideElementList->setDefaultVisible( false );
}

XmlPresConfigFormImpl::~XmlPresConfigFormImpl() {
	
}

QPixmap XmlPresConfigFormImpl::image() {
	return QPixmap( ":/images/preferences-xmlpres.png" );
}

QString XmlPresConfigFormImpl::name() {
	return tr( "XML Pres." );
}

QWidget * XmlPresConfigFormImpl::settingsDialog() {
	return this;
}

bool XmlPresConfigFormImpl::loadSettingsDialog() {
	/*
	m_expandPathLineEdit->setText( m_config.config().xmlPres.autoExpandedPath );
	m_hideElementList->setValues( m_config.config().xmlPres.hidePath );
	m_showSubEltCheckBox->setChecked( m_config.config().xmlPres.showFilteredSubTree );
	m_showNameAttributeCheckBox->setChecked( m_config.config().xmlPres.showNameAttributeIfExists );
	m_viewColorBox->setColor( m_config.config().xmlPres.viewColor );
	m_errorColorBox->setColor( m_config.config().xmlPres.errorColor );
	m_screenColorBox->setColor( m_config.config().xmlPres.screenDataColor );
	*/
	return true;
}

bool XmlPresConfigFormImpl::saveSettingsDialog() {
	/*
	m_config.config().xmlPres.autoExpandedPath = m_expandPathLineEdit->text();
	m_config.config().xmlPres.hidePath = m_hideElementList->values();
	m_config.config().xmlPres.showFilteredSubTree = m_showSubEltCheckBox->isChecked();
	m_config.config().xmlPres.viewColor = m_viewColorBox->color();
	m_config.config().xmlPres.errorColor = m_errorColorBox->color();
	m_config.config().xmlPres.screenDataColor = m_screenColorBox->color();
	m_config.config().xmlPres.showNameAttributeIfExists = m_showNameAttributeCheckBox->isChecked();

	*/
	return true;
}
