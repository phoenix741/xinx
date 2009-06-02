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

#ifndef CUSTOMDIALOGIMPL_H
#define CUSTOMDIALOGIMPL_H
#pragma once

// Xinx header
#include "ui_custom.h"
#include <core/xinxconfig.h>

class SnipetModelIndex;

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
	~CustomDialogImpl();

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
	void m_descriptionPathLineEdit_textChanged( QString text );

	void on_m_buttonBox_clicked( QAbstractButton * button );
	void on_m_highlighterComboBox_activated( QString text );
	void on_m_labelLink_linkActivated( const QString & link );

	void on_m_importPushButton_clicked();
	void on_m_exportPushButton_clicked();

	void on_m_addPushButton_clicked();
	void on_m_removePushButton_clicked();
	void on_m_modifyPushButton_clicked();
	void on_m_duplicatePushButton_clicked();

	void m_snipetTreeView_selectionChanged();
private slots:
	void configurePlugin( PluginElement * plugin );
	void aboutPlugin( PluginElement * plugin );
	void aboutScript( PluginElement * plugin );

private:
	void showConfig();
	void storeConfig();

	XINXConfig m_config;
	SnipetModelIndex * m_snipetModel;
};

#endif














