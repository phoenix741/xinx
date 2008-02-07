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

// Xinx header
#include "ui_custom.h"
#include "xinxconfig.h"

class PrivateCustomDialogImpl;

/*!
 * Implementation of dialog used to configure custom options in XINX. The options is globals for the application.
 * This dialog save and restore modification in a XINXConfig class.
 */
class CustomDialogImpl : public QDialog, public Ui::CustomDialog {
	Q_OBJECT
public:
	/*! 
	 * Custom dialog constructor. The dialog is create with a fixed size 
	 * \param parent Parent of the dialog
	 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
	 */
	CustomDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	
	/*!
	 * Restore the XINXConfig class in the custom dialog to reflect the configuration of application.
	 * \param config The config file used to load options.
	 */
	void loadFromConfig( XINXConfig * config );
	/*!
	 * Store the modification of options in a XINXConfig class.
	 * \param config The config file where to store options
	 */
	void saveToConfig( XINXConfig * config );
private slots:
	void on_m_defaultPushButton_clicked();
	void on_m_canBeSpecifiqueCheckBox_toggled(bool checked);
	void on_m_subDirectoryLineEdit_textChanged(QString );
	void on_m_extentionsListWidget_currentRowChanged(int currentRow);
	void on_m_colorComboBox_activated(QColor color);
	void on_m_boldCheckBox_toggled(bool checked);
	void on_m_italicCheckBox_toggled(bool checked);
	void on_m_StrikeoutCheckBox_toggled(bool checked);
	void on_m_underlineCheckBox_toggled(bool checked);
	void on_m_formatsListView_currentRowChanged(int currentRow);
	void on_m_mergeToolBtn_clicked();
	void on_m_cvsToolBtn_clicked();
	void on_m_changeProjectPathBtn_clicked();
	void on_m_changeApplicationDescriptionPathBtn_clicked();
private:
	PrivateCustomDialogImpl * d;
	friend class PrivateCustomDialogImpl;
};

#endif














