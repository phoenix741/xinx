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
#include <QDir>
#include <QDebug>

/* SnipetPropertyDlgImpl */

SnipetPropertyDlgImpl::SnipetPropertyDlgImpl( int snipetId, QSqlDatabase db, QWidget * parent, Qt::WindowFlags f ) : QDialog( parent, f ) {
	setupUi();

	m_snipetModel = new QSqlRelationalTableModel( this, db );
	m_snipetModel->setTable( "snipets" );
	m_snipetModel->setFilter( QString( "id = %1" ).arg( snipetId ) );
	m_snipetModel->select();

	createMapper();

	m_mapper->toFirst();
}

SnipetPropertyDlgImpl::SnipetPropertyDlgImpl( QSqlDatabase db, QWidget * parent, Qt::WindowFlags f ) : QDialog( parent, f ) {
	setupUi();

	m_snipetModel = new QSqlRelationalTableModel( this, db );
	m_snipetModel->setTable( "snipets" );
	m_snipetModel->select();

	int field = m_snipetModel->fieldIndex( "category_id" );
	int row = m_snipetModel->rowCount();
	m_snipetModel->insertRow( row );
	QModelIndex index = m_snipetModel->index( row, field );
	m_snipetModel->setData( index, 0 );

	createMapper();

	m_mapper->setCurrentIndex( row );
}

SnipetPropertyDlgImpl::~SnipetPropertyDlgImpl() {
}

void SnipetPropertyDlgImpl::setupUi() {
	Ui::SnipetPropertyDialog::setupUi( this );

	// List of icon
	QDir iconsDir = QDir( ":/images" );

	m_iconComboBox->addItem( QString() );
	foreach( const QString & fileName, iconsDir.entryList( QDir::Files ) ) {
		const QString completeFileName = iconsDir.absoluteFilePath( fileName );
		m_iconComboBox->addItem( QIcon( completeFileName ), completeFileName );
	}

	// Snipet shortcut
	m_keyLineEdit->setValidator( new QRegExpValidator( QRegExp( "[a-zA-Z0-9\\:\\-]*" ), this ) );

	// List of category
	m_categoryModel = SnipetDatabaseManager::self()->createCategoryItemModel( m_categoryTreeView );
	m_categoryModel->select();
	m_categoryTreeView->setModel( m_categoryModel );
	m_categoryTreeView->expandAll();
}

void SnipetPropertyDlgImpl::createMapper() {
	m_mapper = new QDataWidgetMapper( this );
	m_mapper->setSubmitPolicy( QDataWidgetMapper::ManualSubmit );
	m_mapper->setModel( m_snipetModel );
	m_mapper->addMapping( m_nameLineEdit, m_snipetModel->fieldIndex( "name" ) );
	m_mapper->addMapping( m_iconComboBox, m_snipetModel->fieldIndex( "icon" ), "value" );
	m_mapper->addMapping( m_keyLineEdit, m_snipetModel->fieldIndex( "shortcut" ) );
	m_mapper->addMapping( m_autoComboBox, m_snipetModel->fieldIndex( "auto" ), "currentIndex" );
	m_mapper->addMapping( m_dialogComboBox, m_snipetModel->fieldIndex( "show_dialog" ), "currentIndex" );
	m_mapper->addMapping( m_descriptionTextEdit, m_snipetModel->fieldIndex( "description" ) );
	m_mapper->addMapping( m_categoryTreeView, m_snipetModel->fieldIndex( "category_id" ) );
	m_mapper->addMapping( m_textEdit, m_snipetModel->fieldIndex( "text" ), "plainText" );
	m_mapper->addMapping( m_availablePlainTextEdit, m_snipetModel->fieldIndex( "available_script" ) );
}

void SnipetPropertyDlgImpl::duplicate() {
	int field = m_snipetModel->fieldIndex( "category_id" );
	int row = m_snipetModel->rowCount();
	m_snipetModel->insertRow( row );
	QModelIndex index = m_snipetModel->index( row, field );
	m_snipetModel->setData( index, 0 );
	m_mapper->setCurrentIndex( row );
}

void SnipetPropertyDlgImpl::setParentId( int id ) {
	m_categoryTreeView->setCategoryId( id );
}

void SnipetPropertyDlgImpl::on_m_categoryTreeView_activated ( const QModelIndex & index ) {
	int id = index.data( CategoryItemModel::CategoryIdRole ).toInt();

	m_addCategoryButton->setEnabled( index != QModelIndex() );
	m_removeCategoryButton->setEnabled( ( id != 0 ) && ( index != QModelIndex() ) );
}

void SnipetPropertyDlgImpl::on_m_addCategoryButton_clicked() {
	QModelIndexList list = m_categoryTreeView->selectionModel()->selectedIndexes();
	Q_ASSERT( list.size() );

	int id = list.at( 0 ).data( CategoryItemModel::CategoryIdRole ).toInt();

	SnipetDatabaseManager::self()->addCategory( id, this );

	m_categoryModel->select();
	m_categoryTreeView->expandAll();
}

void SnipetPropertyDlgImpl::on_m_removeCategoryButton_clicked() {
	QModelIndexList list = m_categoryTreeView->selectionModel()->selectedIndexes();
	Q_ASSERT( list.size() );

	int id = list.at( 0 ).data( CategoryItemModel::CategoryIdRole ).toInt();

	SnipetDatabaseManager::self()->removeCategory( id, this );

	m_categoryModel->select();
	m_categoryTreeView->expandAll();
}

void SnipetPropertyDlgImpl::on_m_buttons_accepted() {
	m_mapper->submit();
}

