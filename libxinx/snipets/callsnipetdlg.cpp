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
#include "snipets/callsnipetdlg.h"

/* CallSnipetDialogImpl */

CallSnipetDialogImpl::CallSnipetDialogImpl( QSqlDatabase db, int snipetId, QWidget * parent, Qt::WFlags f ) : QDialog( parent, f ) {
	setupUi( db, snipetId, this );
}

CallSnipetDialogImpl::~CallSnipetDialogImpl() {

}

void CallSnipetDialogImpl::setupUi( QSqlDatabase db, int snipetId, QDialog * parent ) {
	Ui::SnipetDialog::setupUi( parent );

	/* Initialise the snipet description */
	QSqlQuery snipetQuery( "SELECT name, description, text FROM snipets WHERE snipet_id=:id", db );
	snipetQuery.bindValue( ":id", snipetId );

	bool result = snipetQuery.exec();
	Q_ASSERT( result );

	setWindowTitle( tr("Use the snipet \"%1\"").arg( snipetQuery.value( "name" ).toString() ) );
	m_labelName->setText( snipetQuery.value( "name" ).toString() );
	m_descriptionLabel->setText( snipetQuery.value( "description" ).toString() );
	m_snipetText = snipetQuery.value( "text" ).toString();

	/* Initialise the snipet parameter */
	QSqlQuery paramsQuery( "SELECT name, default FROM snipets_params WHERE snipet_id=:id ORDER BY params_order", db );
	paramsQuery.bindValue( ":id", snipetId );

	result = paramsQuery.exec();
	Q_ASSERT( result );


}
