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
#include "xinxlistwidgetimpl.h"

/* XinxListWidgetImpl */

XinxListWidgetImpl::XinxListWidgetImpl( QWidget * parent, Qt::WindowFlags f ) : QWidget( parent, f ) {
	setupUi( this );

	m_btnDel->setEnabled( m_list->count() > 0 );
	m_btnDef->setEnabled( m_list->count() > 0 );
}

XinxListWidgetImpl::~XinxListWidgetImpl() {

}

void XinxListWidgetImpl::updateDefault( const QString & def ) {
	m_defaultValue = -1;
	bool hasDefault = def.isEmpty() ? true : false;
	for( int i = 0; i < m_list->count() ; i++ ) {
		QListWidgetItem * item = m_list->item( i );
		bool isDefault = item->text() == def;
		if( isDefault ) {
			hasDefault = true;
			m_defaultValue = i;
		}
		QFont font = item->font();
		font.setBold( isDefault );
		item->setFont( font );
	}
	if( ! hasDefault ) {
		QListWidgetItem * item = new QListWidgetItem( def, m_list );
		item->setFlags( item->flags() | Qt::ItemIsEditable );
		QFont font = item->font();
		font.setBold( true );
		item->setFont( font );
		m_list->addItem( item );
		m_defaultValue = m_list->count() - 1;
	}
}

QString XinxListWidgetImpl::defaultValue() const {
	if( m_list->item( m_defaultValue ) )
		return m_list->item( m_defaultValue )->text();
	else
		return QString();
}

void XinxListWidgetImpl::setDefaultValue( const QString & value ) {
	updateDefault( value );
}

bool XinxListWidgetImpl::defaultVisible() const {
	return m_btnDef->isVisible();
}

void XinxListWidgetImpl::setDefaultVisible( bool visible ) {
	m_btnDef->setVisible( visible );
}

QStringList XinxListWidgetImpl::values() const {
	QStringList result;
	for( int i = 0; i < m_list->count(); i++ ) {
		result.append( m_list->item( i )->text() );
	}
	return result;
}

void XinxListWidgetImpl::setValues( const QStringList & values ) {
	QString def = m_list->item( m_defaultValue ) ? m_list->item( m_defaultValue )->text() : QString();

	m_list->clear();
	foreach( QString value, values ) {
		QListWidgetItem * item = new QListWidgetItem( value, m_list );
		item->setFlags( item->flags() | Qt::ItemIsEditable );
		m_list->addItem( item );
	}
	m_btnDel->setEnabled( m_list->count() > 0 );
	m_btnDef->setEnabled( m_list->count() > 0 );

	updateDefault( def );
}

QString XinxListWidgetImpl::defaultProposedValue() const {
	return m_defaultProposedValue;
}

void XinxListWidgetImpl::setDefaultProposedValue( const QString & value ) {
	m_defaultProposedValue = value;
}

void XinxListWidgetImpl::on_m_btnDef_clicked() {
	QListWidgetItem * item = m_list->currentItem();
	if( item ) {
		m_defaultValue = m_list->currentRow();
		updateDefault( item->text() );
		emit defaultValueChanged( item->text() );
	}
}

void XinxListWidgetImpl::on_m_btnAdd_clicked() {
	QListWidgetItem * item = new QListWidgetItem( m_defaultProposedValue.isEmpty() ? "..." : m_defaultProposedValue, m_list );
	item->setFlags( item->flags() | Qt::ItemIsEditable );
	m_list->addItem( item );
	m_list->setCurrentItem( item );
	m_list->editItem( item );

	m_btnDel->setEnabled( m_list->count() > 0 );
	m_btnDef->setEnabled( m_list->count() > 0 );
}

void XinxListWidgetImpl::on_m_btnDel_clicked() {
	Q_ASSERT( m_list->currentRow() >= 0 );

	delete m_list->currentItem();

	m_btnDel->setEnabled( m_list->count() > 0 );
	m_btnDef->setEnabled( m_list->count() > 0 );
}

