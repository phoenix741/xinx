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
	label = new QLabel(tr("Find &what:"));
	lineEdit = new QLineEdit;
	label->setBuddy(lineEdit);

	caseCheckBox = new QCheckBox(tr("Match &case"));
	fromStartCheckBox = new QCheckBox(tr("Search from &start"));
	fromStartCheckBox->setChecked(true);

	findButton = new QPushButton(tr("&Find"));
	findButton->setDefault(true);

	moreButton = new QPushButton(tr("&More"));
	moreButton->setCheckable(true);
	moreButton->setAutoDefault(false);

	buttonBox = new QDialogButtonBox(Qt::Vertical);
	buttonBox->addButton(findButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(moreButton, QDialogButtonBox::ActionRole);

	extension = new QWidget;

	wholeWordsCheckBox = new QCheckBox(tr("&Whole words"));
	backwardCheckBox = new QCheckBox(tr("Search &backward"));
	searchSelectionCheckBox = new QCheckBox(tr("Search se&lection"));

	connect(moreButton, SIGNAL(toggled(bool)), extension, SLOT(setVisible(bool)));

	QVBoxLayout *extensionLayout = new QVBoxLayout;
	extensionLayout->setMargin(0);
	extensionLayout->addWidget(wholeWordsCheckBox);
	extensionLayout->addWidget(backwardCheckBox);
	extensionLayout->addWidget(searchSelectionCheckBox);
	extension->setLayout(extensionLayout);

	QHBoxLayout *topLeftLayout = new QHBoxLayout;
	topLeftLayout->addWidget(label);
	topLeftLayout->addWidget(lineEdit);

	QVBoxLayout *leftLayout = new QVBoxLayout;
	leftLayout->addLayout(topLeftLayout);
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
}
