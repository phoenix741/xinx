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

#ifndef REPLACEDIALOGIMPL_H
#define REPLACEDIALOGIMPL_H

// Xinx header
#include "ui_replace.h"
#include <abstracteditor.h>

class ReplaceDialogImpl : public QDialog, public Ui::ReplaceDialog {
	Q_OBJECT
public:
	ReplaceDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );

	void initialize();
	void setText( const QString & );
	void setReplace( bool );

signals:
	void find( const QString & from, const QString & to, const AbstractEditor::SearchOptions & options );
	void findInFiles( const QString & directory, const QString & from, const QString & to, const AbstractEditor::SearchOptions & options );

private slots:
	void on_m_findButton_clicked();
	void on_m_replaceCheckBox_toggled(bool checked);
};
#endif






