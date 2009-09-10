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
#include "snipets/snipetmanager.h"
#include "snipets/snipetpropertydlgimpl.h"

// Qt header
#include <QRegExp>
#include <QSqlDatabase>

/* SnipetPropertyDlgImpl */

SnipetPropertyDlgImpl::SnipetPropertyDlgImpl( int snipetId, QSqlDatabase db, QWidget * parent, Qt::WindowFlags f ) : QDialog( parent, f ) {
	setupUi( this );
	m_keyLineEdit->setValidator( new QRegExpValidator( QRegExp( "[a-zA-Z0-9\\:\\-]" ), this ) );

	m_snipetModel = new QSqlRelationalTableModel( this, db );
	m_snipetModel->setTable( "snipets" );
	m_snipetModel->setFilter( QString( "id = %1" ).arg( snipetId ) );
	m_snipetModel->select();

	m_mapper = new QDataWidgetMapper( this );
	m_mapper->setSubmitPolicy( QDataWidgetMapper::ManualSubmit );
	m_mapper->setModel( m_snipetModel );
	m_mapper->addMapping( m_nameLineEdit, SnipetPropertyDlgImpl::Snipets_Name );
	m_mapper->addMapping( m_iconComboBox, SnipetPropertyDlgImpl::Snipets_Icon );
	m_mapper->addMapping( m_keyLineEdit, SnipetPropertyDlgImpl::Snipets_Shortcut );
	m_mapper->addMapping( m_autoComboBox, SnipetPropertyDlgImpl::Snipets_Auto );
	m_mapper->addMapping( m_descriptionTextEdit, SnipetPropertyDlgImpl::Snipets_Description );
	m_mapper->addMapping( m_availablePlainTextEdit, SnipetPropertyDlgImpl::Snipets_AvailableScript );

	m_mapper->toFirst();
}

SnipetPropertyDlgImpl::~SnipetPropertyDlgImpl() {
}
