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

#include <QtGui>

#include "finddialog.h"

FindDialog::FindDialog(QWidget *parent) : QDialog(parent) {
	labelFind = new QLabel(tr("Find &what:"));
	lineEditFind = new QLineEdit;
	labelFind->setBuddy(lineEditFind);
	
	replaceCheckBox = new QCheckBox(tr("&Replace by:"));
	lineEditReplace = new QLineEdit;
	connect(replaceCheckBox, SIGNAL( toggled(bool) ), lineEditReplace, SLOT( setEnabled(bool) ));
	replaceCheckBox->setChecked(false);
	lineEditReplace->setEnabled(false);

	caseCheckBox = new QCheckBox(tr("Match c&ase"));
	fromStartCheckBox = new QCheckBox(tr("Search from &start"));

	findButton = new QPushButton(tr("&Find"));
	findButton->setDefault(true);
	connect(findButton, SIGNAL( pressed() ), this, SLOT( callFind() ));
	connect(findButton, SIGNAL( pressed() ), this, SLOT( hide() ));

	closeButton = new QPushButton(tr("&Close"));
	connect(closeButton, SIGNAL( pressed() ), this, SLOT( reject() ));

	moreButton = new QPushButton(tr("&More"));
	moreButton->setCheckable(true);
	moreButton->setAutoDefault(false);

	buttonBox = new QDialogButtonBox(Qt::Vertical);
	buttonBox->addButton(findButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(closeButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(moreButton, QDialogButtonBox::ActionRole);

	extension = new QWidget;

	wholeWordsCheckBox = new QCheckBox(tr("&Whole words"));
	backwardCheckBox = new QCheckBox(tr("Search &backward"));
	searchSelectionCheckBox = new QCheckBox(tr("Search se&lection"));
	regularExpressionCheckBox = new QCheckBox(tr("&Use regular expression"));

	connect(moreButton, SIGNAL(toggled(bool)), extension, SLOT(setVisible(bool)));

	QVBoxLayout *extensionLayout = new QVBoxLayout;
	extensionLayout->setMargin(0);
	extensionLayout->addWidget(wholeWordsCheckBox);
	extensionLayout->addWidget(backwardCheckBox);
	extensionLayout->addWidget(searchSelectionCheckBox);
	extensionLayout->addWidget(regularExpressionCheckBox);
	extension->setLayout(extensionLayout);

	QHBoxLayout *topLeftLayout = new QHBoxLayout;
	topLeftLayout->addWidget(labelFind);
	topLeftLayout->addWidget(lineEditFind);

	QHBoxLayout *topLeftLayout2 = new QHBoxLayout;
	topLeftLayout2->addWidget(replaceCheckBox);
	topLeftLayout2->addWidget(lineEditReplace);

	QVBoxLayout *leftLayout = new QVBoxLayout;
	leftLayout->addLayout(topLeftLayout);
	leftLayout->addLayout(topLeftLayout2);
	leftLayout->addWidget(caseCheckBox);
	leftLayout->addWidget(fromStartCheckBox);
	leftLayout->addStretch(1);

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	mainLayout->addLayout(leftLayout, 0, 0);
	mainLayout->addWidget(buttonBox, 0, 1);
	mainLayout->addWidget(extension, 1, 0, 1, 2);
	setLayout(mainLayout);

	setWindowTitle(tr("Extension"));
	extension->hide();

	connect(fromStartCheckBox, SIGNAL( toggled(bool) ), backwardCheckBox, SLOT( setDisabled(bool) ));
	connect(fromStartCheckBox, SIGNAL( toggled(bool) ), searchSelectionCheckBox, SLOT( setDisabled(bool) ));
	fromStartCheckBox->setChecked(false);
}

void FindDialog::setReplaceChecked(bool checked) { 
	replaceCheckBox->setChecked(checked) ; 
} 

void FindDialog::setText(const QString & text) {
	lineEditFind->setText( text );
}


void FindDialog::callFind() {
	struct FindOptions options;
	options.matchCase = caseCheckBox->checkState() == Qt::Checked;
	options.searchFromStart = fromStartCheckBox->checkState() == Qt::Checked;
	options.wholeWords = wholeWordsCheckBox->checkState() == Qt::Checked;
	options.backwardSearch = backwardCheckBox->checkState() == Qt::Checked;
	options.selectionOnly = searchSelectionCheckBox->checkState() == Qt::Checked;
	options.regularExpression = regularExpressionCheckBox->checkState() == Qt::Checked;
	options.replace = replaceCheckBox->checkState() == Qt::Checked;
	
	emit find(lineEditFind->text(), lineEditReplace->text(), options);
}

QString FindDialog::replaceStr(const struct FindOptions & options, const QString & src, const QString & dest, const QString & content) {
	if( ! options.replace ) return QString();
	if( ! options.regularExpression ) return dest;
		
	QRegExp	expression(src);
	expression.indexIn(content);
	QStringList list = expression.capturedTexts();
	QString result(dest);
	int index = 1;

	QStringList::iterator it = list.begin();
	while (it != list.end()) {
		result = result.replace(QString("\\%1").arg(index), *it);
		it++; index++;
	}
	
	return result;
}

void FindDialog::initialize() {
	lineEditFind->selectAll();
	lineEditFind->setFocus( Qt::ActiveWindowFocusReason );
	findButton->setDefault(true);
}
