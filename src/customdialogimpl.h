/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef CUSTOMDIALOGIMPL_H
#define CUSTOMDIALOGIMPL_H
//
#include "ui_custom.h"
#include "xinxconfig.h"

class SyntaxHighlighter;
//
class CustomDialogImpl : public QDialog, public Ui::CustomDialog {
	Q_OBJECT
public:
	CustomDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	
	void loadFromConfig( XINXConfig * config );
	void saveToConfig( XINXConfig * config );
private slots:
	void on_m_cvsToolButton_clicked();
	void on_m_CVSToolsLineEdit_textChanged(QString );
	void on_m_saveSessionCheckBox_toggled(bool checked);
	void on_m_changeObjectPathBtn_clicked();
	void on_m_changeProjectPathBtn_clicked();
	void on_m_objectDescriptionPathLineEdit_textChanged(QString );
	void on_m_underlineCheckBox_toggled(bool checked);
	void on_m_StrikeoutCheckBox_toggled(bool checked);
	void on_m_italicCheckBox_toggled(bool checked);
	void on_m_boldCheckBox_toggled(bool checked);
	void on_m_fontComboBox_currentFontChanged(QFont f);
	void on_m_projectPathLineEdit_textChanged(QString );
	void on_m_fileTypePath_textChanged(QString );
	void on_m_specifiqueCheckBox_toggled(bool checked);
	void on_m_createBakCheckBox_toggled(bool checked);
	void on_m_alertStandardCheckBox_toggled(bool checked);
	void on_m_langComboBox_currentIndexChanged(QString );
	void on_m_fileTypeComboBox_currentIndexChanged(int);
	void on_m_syntaxFileTypeComboBox_currentIndexChanged(QString);
	void on_m_syntaxTypeComboBox_currentIndexChanged(QString);
	void on_m_colorComboBox_activated( QColor c );
private:
	XINXConfig m_config;
	SyntaxHighlighter * m_highlighter;
};
#endif














