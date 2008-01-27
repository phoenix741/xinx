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
#include "exceptions.h"

/* XinxListWidgetImpl */

XinxListWidgetImpl::XinxListWidgetImpl( QWidget * parent, Qt::WindowFlags f ) : QWidget( parent, f ) {
	setupUi( this );
	
	m_btnDel->setEnabled( m_list->count() > 0 );
	m_btnDef->setEnabled( m_list->count() > 0 );
}

XinxListWidgetImpl::~XinxListWidgetImpl() {
	
}

void XinxListWidgetImpl::updateDefault() {
	for( int i = 0; i < m_list->count() ; i++ ) {
		QListWidgetItem * item = m_list->item( i );
		QFont font = item->font();
		font.setBold( item->text() == m_defaultValue );
		item->setFont( font );
	}
}
	
QString XinxListWidgetImpl::defaultValue() const {
	return m_defaultValue;
}

void XinxListWidgetImpl::setDefaultValue( const QString & value ) {
	m_defaultValue = value;
	updateDefault();
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
	m_list->clear();
	foreach( QString value, values ) 
		m_list->addItem( new QListWidgetItem( value, m_list ) );
	m_btnDel->setEnabled( m_list->count() > 0 );
	m_btnDef->setEnabled( m_list->count() > 0 );
	updateDefault();
}

QString XinxListWidgetImpl::valueName() const {
	return m_valueName;
}

void XinxListWidgetImpl::setValueName( const QString & value ) {
	m_valueName = value; 
}

QString XinxListWidgetImpl::defaultProposedValue() const {
	return m_defaultProposedValue;
}

void XinxListWidgetImpl::setDefaultProposedValue( const QString & value ) {
	m_defaultProposedValue = value;
}

void XinxListWidgetImpl::on_m_lineEdit_textChanged( QString text ) {
	QListWidgetItem * item = m_list->currentItem();
	if( item ) item->setText( text );	
}

void XinxListWidgetImpl::on_m_btnDef_clicked() {
	QListWidgetItem * item = m_list->currentItem();
	if( item ) {
		m_defaultValue = item->text() ;
		updateDefault();
		emit defaultValueChanged( m_defaultValue );
	}
}

void XinxListWidgetImpl::on_m_btnAdd_clicked() {
	QListWidgetItem * item = new QListWidgetItem( m_list );
	m_list->addItem( item );
	m_list->setCurrentItem( item );
	m_lineEdit->setFocus();

	m_btnDel->setEnabled( m_list->count() > 0 );
	m_btnDef->setEnabled( m_list->count() > 0 );
}

void XinxListWidgetImpl::on_m_btnDel_clicked() {
	XINX_ASSERT( m_list->currentRow() >= 0 );
	
	delete m_list->currentItem();
	
	m_btnDel->setEnabled( m_list->count() > 0 );
	m_btnDef->setEnabled( m_list->count() > 0 );
}

