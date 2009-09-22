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
#include "snipets/categorypropertydlgimpl.h"

// Qt header
#include <QRegExp>
#include <QSqlDatabase>
#include <QDir>
#include <QDebug>

/* CategoryPropertyDlgImpl */

CategoryPropertyDlgImpl::CategoryPropertyDlgImpl( int categoryId, QSqlDatabase db, QWidget * parent, Qt::WindowFlags f ) : QDialog( parent, f ) {
	setupUi();

	m_categoryTableModel = new QSqlTableModel( this, db );
	m_categoryTableModel->setTable( "categories" );
	m_categoryTableModel->setFilter( QString( "id = %1" ).arg( categoryId ) );
	m_categoryTableModel->select();

	createMapper();

	m_mapper->toFirst();
}

CategoryPropertyDlgImpl::CategoryPropertyDlgImpl( QSqlDatabase db, QWidget * parent, Qt::WindowFlags f ) : QDialog( parent, f ) {
	setupUi();

	m_categoryTableModel = new QSqlTableModel( this, db );
	m_categoryTableModel->setTable( "categories" );
	m_categoryTableModel->select();

	int field = m_categoryTableModel->fieldIndex( "parent_id" );
	int row = m_categoryTableModel->rowCount();
	m_categoryTableModel->insertRow( row );
	QModelIndex index = m_categoryTableModel->index( row, field );
	m_categoryTableModel->setData( index, 0 );

	createMapper();

	m_mapper->setCurrentIndex( row );
}

CategoryPropertyDlgImpl::~CategoryPropertyDlgImpl() {
}

void CategoryPropertyDlgImpl::setupUi() {
	Ui::CategoryPropertyDialog::setupUi( this );

	// List of category
	m_categoryModel = SnipetManager::self()->createCategoryItemModel( m_categoryTreeView );
	m_categoryModel->select();
	m_categoryTreeView->setModel( m_categoryModel );
	//m_categoryTreeView->expandAll();
}

void CategoryPropertyDlgImpl::createMapper() {
	m_mapper = new QDataWidgetMapper( this );
	m_mapper->setSubmitPolicy( QDataWidgetMapper::ManualSubmit );
	m_mapper->setModel( m_categoryTableModel );
	m_mapper->addMapping( m_nameLineEdit, m_categoryTableModel->fieldIndex( "name" ) );
	m_mapper->addMapping( m_descriptionTextEdit, m_categoryTableModel->fieldIndex( "description" ) );
	m_mapper->addMapping( m_categoryTreeView, m_categoryTableModel->fieldIndex( "parent_id" ) );
	m_mapper->addMapping( m_availablePlainTextEdit, m_categoryTableModel->fieldIndex( "available_script" ) );
}

void CategoryPropertyDlgImpl::setParentId( int id ) {
	m_categoryTreeView->setCategoryId( id );
}

void CategoryPropertyDlgImpl::setCategoryAccess( bool value ) {
	m_addCategoryButton->setVisible( value );
	m_removeCategoryButton->setVisible( value );
}

void CategoryPropertyDlgImpl::on_m_categoryTreeView_activated ( const QModelIndex & index ) {
	int id = index.data( CategoryItemModel::CategoryIdRole ).toInt();

	m_addCategoryButton->setEnabled( index != QModelIndex() );
	m_removeCategoryButton->setEnabled( ( id != 0 ) && ( index != QModelIndex() ) );
}

void CategoryPropertyDlgImpl::on_m_addCategoryButton_clicked() {
	QModelIndexList list = m_categoryTreeView->selectionModel()->selectedIndexes();
	Q_ASSERT( list.size() );

	int id = list.at( 0 ).data( CategoryItemModel::CategoryIdRole ).toInt();

	SnipetManager::self()->addCategory( id, false, this );

	m_categoryModel->select();
	//m_categoryTreeView->expandAll();
}

void CategoryPropertyDlgImpl::on_m_removeCategoryButton_clicked() {
	QModelIndexList list = m_categoryTreeView->selectionModel()->selectedIndexes();
	Q_ASSERT( list.size() );

	int id = list.at( 0 ).data( CategoryItemModel::CategoryIdRole ).toInt();

	SnipetManager::self()->removeCategory( id, this );

	m_categoryModel->select();
	//m_categoryTreeView->expandAll();
}

void CategoryPropertyDlgImpl::on_m_buttons_accepted() {
	m_mapper->submit();
}
