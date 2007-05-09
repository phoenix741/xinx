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

#include "replacedialogimpl.h"
//
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
	
	struct FindOptions options;
	options.toReplace         = m_replaceCheckBox->checkState() == Qt::Checked;
	options.matchCase         = m_caseCheckBox->checkState() == Qt::Checked;
	options.searchFromStart   = m_fromStartCheckBox->checkState() == Qt::Checked;
	options.wholeWords        = m_wholeWordsCheckBox->checkState() == Qt::Checked;
	options.regularExpression = m_regularExpressionCheckBox->checkState() == Qt::Checked;

	options.searchDirection   = m_UpRadioButton->isChecked() ? FindOptions::SEARCHUP : FindOptions::SEARCHDOWN;
	options.searchExtend      = m_searchAllRadioButton->isChecked() ? FindOptions::SEARCHALL : FindOptions::SEARCHSELECTION;
	
	emit find( m_comboFind->lineEdit()->text(), m_comboReplace->lineEdit()->text(), options );
}

QString ReplaceDialogImpl::replaceStr( const struct FindOptions & options, const QString & src, const QString & dest, const QString & content ) {
	if( ! options.toReplace ) return QString();
	if( ! options.regularExpression ) return dest;
		
	QRegExp	expression( src );
	expression.indexIn( content );
	QStringList list = expression.capturedTexts();
	QString result( dest );
	int index = 1;

	QStringList::iterator it = list.begin();
	while (it != list.end()) {
		result = result.replace(QString("\\%1").arg(index), *it);
		it++; index++;
	}
	
	return result;
}


