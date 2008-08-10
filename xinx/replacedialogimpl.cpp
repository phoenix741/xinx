/****************************************************************************
 **
 ** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
 **
 ** This file is part of the example classes of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License version 2.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of
 ** this file.  Please review the following information to ensure GNU
 ** General Public Licensing requirements will be met:
 ** http://www.trolltech.com/products/qt/opensource.html
 **
 ** If you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://www.trolltech.com/products/qt/licensing.html or contact the
 ** sales department at sales@trolltech.com.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

#include <QLineEdit>

// Xinx header
#include "replacedialogimpl.h"
#include "xslproject.h"

ReplaceDialogImpl::ReplaceDialogImpl( QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {
	setupUi(this);
}
//

void ReplaceDialogImpl::on_m_replaceCheckBox_toggled( bool checked ) {
	if( checked )
		m_findButton->setText( tr("Replace") );
	else
		m_findButton->setText( tr("Find") );
}

void ReplaceDialogImpl::initialize() {
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
	m_comboFind->lineEdit()->setText( str );
}

void ReplaceDialogImpl::setReplace( bool value ) {
	m_replaceCheckBox->setChecked( value ) ;
}

void ReplaceDialogImpl::on_m_findButton_clicked() {
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
			emit findInFiles( m_directoryWidget->lineEdit()->text(), QString(), m_comboReplace->lineEdit()->text(), options );
	}
}

