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

#ifndef _FINDDIALOG_H
#define _FINDDIALOG_H

#include <QDialog>

class QCheckBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

class FindDialog : public QDialog {
	Q_OBJECT
public:
	struct FindOptions {
		bool matchCase;
		bool searchFromStart;
		bool wholeWords;
		bool backwardSearch;
		bool selectionOnly;
		bool regularExpression;
		bool replace;
	};
	FindDialog(QWidget *parent = 0);
	
	void setReplaceChecked(bool);
	
	/** @src regular expression for search
	    @dest result type
	    @content the content find in the editor
	*/
	static QString replaceStr(const struct FindOptions &, const QString & src, const QString & dest, const QString & content);
	
signals:
	void find(const QString &, const QString &, const struct FindDialog::FindOptions &);

private slots:
	void callFind();

private:

	QLabel *labelFind;
	QCheckBox *replaceCheckBox;
	QLineEdit *lineEditFind;
	QLineEdit *lineEditReplace;
	QCheckBox *caseCheckBox;
	QCheckBox *fromStartCheckBox;
	QCheckBox *wholeWordsCheckBox;
	QCheckBox *searchSelectionCheckBox;
	QCheckBox *backwardCheckBox;
	QCheckBox *regularExpressionCheckBox;
	QDialogButtonBox *buttonBox;
	QPushButton *findButton;
	QPushButton *closeButton;
	QPushButton *moreButton;
	QWidget *extension;
};

#endif
