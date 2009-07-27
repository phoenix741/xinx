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

// Qt header
#include <QLineEdit>
#include <QCompleter>

// Xinx header
#include "replacedialogimpl.h"
#include <core/xinxproject.h>

ReplaceDialogImpl::ReplaceDialogImpl( QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {
	setupUi(this);

	m_findButton = m_buttonBox->button( QDialogButtonBox::Ok );
	m_findButton->setText( tr("Find") );
	m_findButton->setIcon( QIcon( ":/images/find.png" ) );

	connect( m_findButton, SIGNAL(clicked()), this, SLOT(m_findButton_clicked()) );
}
//

void ReplaceDialogImpl::on_m_replaceCheckBox_toggled( bool checked ) {
	if( checked )
		m_findButton->setText( tr("Replace") );
	else
		m_findButton->setText( tr("Find") );
}

void ReplaceDialogImpl::initialize( bool hasEditor ) {
	m_searchAllRadioButton->setEnabled( hasEditor );
	m_searchSelectionRadioButton->setEnabled( hasEditor );
	m_searchAllRadioButton->setChecked( hasEditor );
	m_projectFilesRadioButton->setChecked( ! hasEditor );

	m_comboFind->lineEdit()->selectAll();
	m_comboFind->lineEdit()->setFocus( Qt::ActiveWindowFocusReason );

	if( XINXProjectManager::self()->project() ) {
		m_directoryWidget->setDefaultValue( XINXProjectManager::self()->project()->projectPath() );
		m_directoryWidget->lineEdit()->setText( XINXProjectManager::self()->project()->projectPath() );
	} else {
		m_directoryWidget->setDefaultValue( QString() );
		m_directoryWidget->lineEdit()->setText( QString() );
	}

	m_projectFilesRadioButton->setEnabled( XINXProjectManager::self()->project() != 0 );

	m_findButton->setDefault(true);
}

void ReplaceDialogImpl::setText( const QString & str ) {
	m_comboFind->setEditText( str );
	m_comboFind->completer()->setCompletionPrefix( str );
}

void ReplaceDialogImpl::setReplace( bool value ) {
	m_replaceCheckBox->setChecked( value ) ;
}

void ReplaceDialogImpl::on_m_projectFilesRadioButton_toggled(bool checked) {
	if( checked ) {
		m_directoryWidget->lineEdit()->setText( XINXProjectManager::self()->project()->projectPath() );
	}
}

void ReplaceDialogImpl::m_findButton_clicked() {
	if( ! m_comboFind->lineEdit()->text().isEmpty() && ( m_comboFind->findText( m_comboFind->lineEdit()->text() ) == -1 ) )
		m_comboFind->addItem( m_comboFind->lineEdit()->text() );
	if( ! m_comboReplace->lineEdit()->text().isEmpty() && ( m_comboReplace->findText( m_comboFind->lineEdit()->text() ) == -1 ) )
		m_comboReplace->addItem( m_comboReplace->lineEdit()->text() );

	AbstractEditor::SearchOptions options;
	if( m_searchSelectionRadioButton->isChecked() )					options |= AbstractEditor::ONLY_SELECTION;
	if( m_UpRadioButton->isChecked() )								options |= AbstractEditor::BACKWARD;
	if( m_caseCheckBox->checkState() == Qt::Checked )				options |= AbstractEditor::MATCH_CASE;
	if( m_fromStartCheckBox->checkState() == Qt::Checked )			options |= AbstractEditor::SEARCH_FROM_START;
	if( m_wholeWordsCheckBox->checkState() == Qt::Checked )			options |= AbstractEditor::WHOLE_WORDS;
	if( m_regularExpressionCheckBox->checkState() == Qt::Checked )	options |= AbstractEditor::REGULAR_EXPRESSION;

	if( ! (m_projectFilesRadioButton->isChecked() || m_customFilesRadioButton->isChecked()) ) {
		if( m_replaceCheckBox->checkState() == Qt::Checked )
			emit find( m_comboFind->lineEdit()->text(), m_comboReplace->lineEdit()->text(), options );
		else
			emit find( m_comboFind->lineEdit()->text(), QString(), options );
	} else {
		if( m_replaceCheckBox->checkState() == Qt::Checked )
			emit findInFiles( m_directoryWidget->lineEdit()->text(), m_comboFind->lineEdit()->text(), m_comboReplace->lineEdit()->text(), options );
		else
			emit findInFiles( m_directoryWidget->lineEdit()->text(), m_comboFind->lineEdit()->text(), QString(), options );
	}
}

