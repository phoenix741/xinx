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
#include "core/xinxconfig.h"
#include "snipets/callsnipetdlg.h"
#include "snipets/snipetmanager.h"
#include "snipets/snipet.h"

// Qt header
#include <QSqlQuery>
#include <QDebug>

/* SnipetParameterNameItem */

SnipetParameterNameItem::SnipetParameterNameItem() : QTableWidgetItem() {
}

SnipetParameterNameItem::SnipetParameterNameItem( const QString & name ) : QTableWidgetItem() {
	setData( Qt::DisplayRole, name );
}

/* SnipetParameterValueItem */

SnipetParameterValueItem::SnipetParameterValueItem() : QTableWidgetItem() {
}

SnipetParameterValueItem::SnipetParameterValueItem( const QString & defaultValue ) : QTableWidgetItem(), m_defaultValue( defaultValue ) {
	setData( Qt::DisplayRole, m_defaultValue );
}

void SnipetParameterValueItem::setDefault() {
	setData( Qt::DisplayRole, m_defaultValue );
}

void SnipetParameterValueItem::setDefaultValue( const QString & value ) {
	if( m_defaultValue != value ) {
		if( data( Qt::DisplayRole ) == m_defaultValue ) {
			setData( Qt::DisplayRole, m_defaultValue );
		}
		m_defaultValue = value;
	}
}

const QString & SnipetParameterValueItem::defaultValue() const {
	return m_defaultValue;
}

/* CallSnipetDialogImpl */

CallSnipetDialogImpl::CallSnipetDialogImpl( QSqlDatabase db, int snipetId, QWidget * parent, Qt::WFlags f ) : QDialog( parent, f ) {
	setupUi( db, snipetId, this );
}

CallSnipetDialogImpl::~CallSnipetDialogImpl() {

}

const QString & CallSnipetDialogImpl::snipetText() const {
	return m_snipetText;
}

QStringList CallSnipetDialogImpl::values() const {
	QStringList parameters;
	for( int row = 0 ; row < m_paramTableWidget->rowCount() ; row++ ) {
		parameters += m_paramTableWidget->item( row, 1 )->data( Qt::DisplayRole ).toString();
	}
	return parameters;
}

int CallSnipetDialogImpl::exec() {
	if( XINXConfig::self()->config().snipets.alwaysShowDialog || m_showDialog ) {
		return QDialog::exec();
	} else {
		return QDialog::Accepted;
	}
}

void CallSnipetDialogImpl::on_m_tabWidget_currentChanged( int value ) {
	if( value == 1 ) { // Show result tab, so we calculate the result
		QString result;
		QStringList parameters = values();
		if( SnipetDatabaseManager::self()->executeSnipetScript( m_snipetText, parameters, &result ) )
			m_resultEdit->setPlainText( result );
		else
			m_resultEdit->setPlainText( tr("Cannot execute the snipet. Please check the script in the snipet in configuration dialog.") );
	}
}

void CallSnipetDialogImpl::setupUi( QSqlDatabase db, int snipetId, QDialog * parent ) {
	Ui::CallSnipetDialog::setupUi( parent );

	/* Initialise the snipet description */
	QSqlQuery snipetQuery( "SELECT name, description, text, auto, show_dialog FROM snipets WHERE id=:id", db );
	snipetQuery.bindValue( ":id", snipetId );

	bool result = snipetQuery.exec();
	Q_ASSERT( result );

	if( ! snipetQuery.next() ) {
		qCritical() << tr("CallSnipetDialogImpl: Can't find snipet id '%1'").arg( snipetId );
		return;
	}

	setWindowTitle( tr("Use the snipet \"%1\"").arg( snipetQuery.value( 0 ).toString() ) );
	m_labelName->setText( "<b>" + snipetQuery.value( 0 ).toString() + "</b>" );
	m_descriptionLabel->setText( snipetQuery.value( 1 ).toString() );
	m_snipetText = snipetQuery.value( 2 ).toString();
	m_isAutomatic = snipetQuery.value( 3 ).toBool();
	m_showDialog  = snipetQuery.value( 4 ).toBool();

	/* Initialise the snipet parameter */
	QSqlQuery paramsQuery( "SELECT name, default_value FROM snipets_params WHERE snipet_id=:id ORDER BY params_order", db );
	paramsQuery.bindValue( ":id", snipetId );

	result = paramsQuery.exec();
	Q_ASSERT( result );

	int row = 0;
	while( paramsQuery.next() ) {
		m_paramTableWidget->setRowCount( row + 1 );

		SnipetParameterNameItem * name = new SnipetParameterNameItem( paramsQuery.value( 0 ).toString() );
		SnipetParameterValueItem * defaultValue = new SnipetParameterValueItem( paramsQuery.value( 1 ).toString() );

		m_paramTableWidget->setVerticalHeaderItem( row, new QTableWidgetItem( tr("Parameter %1").arg( row ) ) );
		m_paramTableWidget->setItem( row, 0, name );
		m_paramTableWidget->setItem( row, 1, defaultValue );
	
		row++;
	}
}

